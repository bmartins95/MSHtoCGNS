#include "MSHtoCGNS/Manipulation/RegionGenerator.hpp"
#include <cgnslib.h>

RegionGenerator::RegionGenerator(boost::shared_ptr<GridData> gridData, std::string regionGeneratorScript) : gridData(gridData) {
    boost::property_tree::read_json(regionGeneratorScript, this->propertyTree);
    this->readScript();
    this->generateCoordinates();
    this->generateRegions();
}

void RegionGenerator::readScript() {
    for (const auto& wellRegion : this->propertyTree.get_child("regions")) {
        this->regionGeneratorDatum.emplace_back();

        this->regionGeneratorDatum.back().name = wellRegion.second.get<std::string>("name");

        Point begin;
        begin.set<0>(wellRegion.second.get<double>("begin.x"));
        begin.set<1>(wellRegion.second.get<double>("begin.y"));
        begin.set<2>(wellRegion.second.get<double>("begin.z"));

        Point end;
        end.set<0>(wellRegion.second.get<double>("end.x"));
        end.set<1>(wellRegion.second.get<double>("end.y"));
        end.set<2>(wellRegion.second.get<double>("end.z"));

        this->regionGeneratorDatum.back().box = Box(begin, end);
    }
}

void RegionGenerator::generateCoordinates() {
    this->coordinates.reserve(this->gridData->coordinates.size());
    for (auto c : this->gridData->coordinates) {
        this->coordinates.emplace_back(Point(c[0], c[1], c[2]));
    }
}

void RegionGenerator::generateRegions() {
    for (auto rGD : this->regionGeneratorDatum) {
        std::vector<std::vector<SectionData>::iterator> regions;
        for (auto section = this->gridData->sections.begin(); section != this->gridData->sections.end(); ++section) {
            if (section->dimension == this->gridData->dimension) {
                regions.emplace_back(section);
            }
        }
        std::sort(regions.begin(), regions.end(), [](const auto& a, const auto& b){return a->begin < b->begin;});

        for (auto region : regions) {
            std::cout << boost::str(boost::format("\n\t%6s %3i %3i") % region->name % region->begin % region->end);
        }
        std::cout << std::endl;
        std::cout << boost::str(boost::format("\n\t%s") % rGD.name);
        std::cout << boost::str(boost::format("\n\t\t%.3f %.3f %.3f") % rGD.box.min_corner().get<0>() % rGD.box.min_corner().get<1>() % rGD.box.min_corner().get<2>());
        std::cout << boost::str(boost::format("\n\t\t%.3f %.3f %.3f") % rGD.box.max_corner().get<0>() % rGD.box.max_corner().get<1>() % rGD.box.max_corner().get<2>());
        std::cout << std::endl;

        auto& cs = this->gridData->connectivities;

        for (auto region = regions.begin(); region != regions.end(); ++region) {
            for (auto connectivity = cs.begin() + (*region)->begin; connectivity != cs.begin() + (*region)->end; ++connectivity) {
                if (connectivity->at(0) == TETRA_4 || connectivity->at(0) == HEXA_8 || connectivity->at(0) == PENTA_6 || connectivity->at(0) == PYRA_5) {
                    if (this->isElementInRange(connectivity, rGD.box)) {
                        connectivity->back() = this->marker;
                        for (auto former = regions.begin(); former != region; ++former) {
                            (*former)->end++;
                        }
                    }
                }
            }
        }

        std::stable_sort(cs.begin(), cs.end(), [](const auto& a, const auto& b){return a.back() < b.back();});
        auto it = std::find_if_not(cs.begin(), cs.end(), [=](const auto& c){return c.back() == this->marker;});
        if (it == cs.end()) {
            throw std::runtime_error(error(__PRETTY_FUNCTION__, "There are no elements inside box"));
        }
        int end = std::distance(cs.begin(), it);

        for (auto region : regions) {
            region->begin += end;
        }

        this->gridData->sections.emplace_back(SectionData(rGD.name, this->gridData->dimension, 0, end, std::vector<int>{}));

        for (auto connectivity = cs.begin(); connectivity != cs.end(); ++connectivity) {
            connectivity->back() = std::distance(cs.begin(), connectivity);
        }
        std::sort(this->gridData->sections.begin(), this->gridData->sections.end(), [](const auto& a, const auto& b){return a.begin < b.begin;});

        printGridDataInformation(this->gridData);
    }
}

bool RegionGenerator::isElementInRange(std::vector<std::vector<int>>::iterator connectivity, const Box& box) {
    MultiPoint element;
    for (auto vertex = connectivity->cbegin() + 1; vertex != connectivity->cend() - 1; ++vertex) {
        boost::geometry::append(element, this->coordinates[*vertex]);
    }

    Point centroid;
    boost::geometry::centroid(element, centroid);

    return boost::geometry::within(centroid, box);
}
