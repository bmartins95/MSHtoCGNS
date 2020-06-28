#include "MSHtoCGNS/Manipulation/Generator/CartesianWellGenerator.hpp"
#include <cgnslib.h>

CartesianWellGenerator::CartesianWellGenerator(boost::shared_ptr<GridData> gridData, std::string scriptPath) : Generator(gridData, scriptPath) {
    this->checkGridData();
    this->generateWells();
}

CartesianWellGenerator::CartesianWellGenerator(boost::shared_ptr<GridData> gridData, boost::property_tree::ptree script) : Generator(gridData, script) {
    this->checkGridData();
    this->generateWells();
}

void CartesianWellGenerator::checkGridData() {
    if (this->gridData->dimension != 3)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData dimension must be 3 and not " + std::to_string(this->gridData->dimension));

    if (std::count_if(this->gridData->connectivities.cbegin(), this->gridData->connectivities.cend(), [](const auto& c){return c[0] == BAR_2;}) != 0)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - gridData lines size must be 0");

    if (std::count_if(this->gridData->sections.cbegin(), this->gridData->sections.cend(), [](const auto& e){return e.dimension == 1;}) != 0)
        throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Number of wells in gridData must be 0");
}

void CartesianWellGenerator::generateWells() {
    std::vector<Point> coordinates;
    coordinates.reserve(this->gridData->coordinates.size());
    for (auto c : this->gridData->coordinates) {
        coordinates.emplace_back(Point(c[0], c[1], c[2]));
    }

    for (const auto& wellRegion : this->script.get_child("wells")) {
        auto wellName = wellRegion.second.get<std::string>("name");
        Point begin, end;
        boost::geometry::read_wkt(boost::str(boost::format("POINT(%s)") % wellRegion.second.get<std::string>("begin")), begin);
        boost::geometry::read_wkt(boost::str(boost::format("POINT(%s)") % wellRegion.second.get<std::string>("end")), end);
    }
}
