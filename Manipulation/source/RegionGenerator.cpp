#include "MSHtoCGNS/Manipulation/RegionGenerator.hpp"
#include <cgnslib.h>

void operator +=(std::array<double, 3>& a, const std::array<double, 3>& b) {
    a[0] += b[0];
    a[1] += b[1];
    a[2] += b[2];
}

void operator /=(std::array<double, 3>& a, double scalar) {
    a[0] /= scalar;
    a[1] /= scalar;
    a[2] /= scalar;
}

RegionGenerator::RegionGenerator(boost::shared_ptr<GridData> gridData, std::string regionGeneratorScript) : gridData(gridData) {
    boost::property_tree::read_json(regionGeneratorScript, this->propertyTree);
    this->readScript();
    this->checkGridData();
    this->generateRegions();
}

void RegionGenerator::checkGridData() {
    if (std::count_if(this->gridData->sections.cbegin(), this->gridData->sections.cend(), [=](const auto& s){return s.dimension == this->gridData->dimension;}) != 1)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Only 1 region is currently supported");
}

void RegionGenerator::readScript() {
    for (const auto& wellRegion : this->propertyTree.get_child("regions")) {
        this->regionGeneratorDatum.emplace_back();

        this->regionGeneratorDatum.back().name = wellRegion.second.get<std::string>("name");

        int index = 0;
        std::array<double, 3> begin;
        for (const auto& coordinate : wellRegion.second.get_child("begin")) {
            begin[index++] = coordinate.second.get_value<double>();
        }

        index = 0;
        std::array<double, 3> end;
        for (const auto& coordinate : wellRegion.second.get_child("end")) {
            end[index++] = coordinate.second.get_value<double>();
        }

        this->regionGeneratorDatum.back().box = Box(Point(begin[0], begin[1], begin[2]), Point(end[0], end[1], end[2]));
    }
}

void RegionGenerator::generateRegions() {
    std::vector<std::vector<SectionData>::iterator> regions;
    for (auto section = this->gridData->sections.begin(); section != this->gridData->sections.end(); ++section)
        if (section->dimension == this->gridData->dimension)
            regions.emplace_back(section);

    for (auto rGD : this->regionGeneratorDatum) {
        std::cout << boost::str(boost::format("\n\t%s") % rGD.name);
        std::cout << boost::str(boost::format("\n\t\t%.3f %.3f %.3f") % rGD.box.min_corner().get<0>() % rGD.box.min_corner().get<1>() % rGD.box.min_corner().get<2>());
        std::cout << boost::str(boost::format("\n\t\t%.3f %.3f %.3f") % rGD.box.max_corner().get<0>() % rGD.box.max_corner().get<1>() % rGD.box.max_corner().get<2>());
        std::cout << std::endl;

        auto& cs = this->gridData->connectivities;

        for (auto region = regions.begin(); region != regions.end(); ++region) {
            for (auto connectivity = cs.begin() + (*region)->begin; connectivity != cs.begin() + (*region)->end; ++connectivity) {
                if (connectivity->at(0) == TETRA_4 || connectivity->at(0) == HEXA_8 || connectivity->at(0) == PENTA_6 || connectivity->at(0) == PYRA_5) {
                    if (this->isElementInRange(*connectivity, rGD.box)) {
                        connectivity->back() = this->marker;
                        (*region)->begin++;
                        for (auto former = regions.begin(); former != region; ++former) {
                            (*former)->end++;
                        }
                    }
                }
            }
        }

        std::cout << std::endl;
        for (auto i = cs.begin(); i != cs.end(); ++i) {
            for (auto j = i->cbegin(); j != i->cend(); ++j) {
                std::cout << " " << std::setw(5) << std::right << *j;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;

        std::cout << boost::str(boost::format("\n\nconnecti: %i\n") % std::count_if(cs.cbegin(), cs.cend(), [](const auto& c){return c.front() == HEXA_8;}));
        std::cout << boost::str(boost::format("\n\nelements: %i\n") % std::count_if(cs.cbegin(), cs.cend(), [](const auto& c){return c.back() == -1;}));

        std::stable_sort(cs.begin(), cs.end(), [](const auto& a, const auto& b){return a.back() < b.back();});
        auto end = std::find_if_not(cs.begin(), cs.end(), [=](const auto& c){return c.back() == this->marker;});
        if (end == cs.end()) {
            throw std::runtime_error(error(__PRETTY_FUNCTION__, "There are no elements inside box"));
        }
        this->gridData->sections.emplace_back(SectionData(rGD.name, this->gridData->dimension, 0, std::distance(cs.begin(), end), std::vector<int>{}));

        int index = 0;
        for (auto& connectivity : cs) {
            connectivity.back() = index++;
        }
    }
    std::sort(this->gridData->sections.begin(), this->gridData->sections.end(), [](const auto& a, const auto& b){return a.begin < b.begin;});
}

bool RegionGenerator::isElementInRange(const std::vector<int>& connectivity, const Box& box) {
    std::array<double, 3> centroid{0.0, 0.0, 0.0};
    for (auto vertex = connectivity.cbegin() + 1; vertex != connectivity.cend() + 1; ++vertex) {
        centroid += this->gridData->coordinates[*vertex];
    }
    centroid /= static_cast<double>(connectivity.size());

    Point point(centroid[0], centroid[1], centroid[2]);

    return boost::geometry::within(point, box);
}
