#include "MSHtoCGNS/Utilities/Print.hpp"

std::string red(std::string message) {
    return "\033[1;31m" + message + "\033[0m";
}

std::string yellow(std::string message) {
    return "\033[1;33m" + message + "\033[0m";
}

std::string purple(std::string message) {
    return "\033[1;35m" + message + "\033[0m";
}

std::string cyan(std::string message) {
    return "\033[1;36m" + message + "\033[0m";
}

void printGridDataInformation(boost::shared_ptr<GridData> gD, std::string message) {
    std::cout << std::endl << "\t" << message << std::endl;

    int numberOfElements = gD->tetrahedrons.size() + gD->hexahedrons.size() + gD->prisms.size() + gD->pyramids.size();
    std::cout << std::endl << "\t\tnumberOfElements: " << numberOfElements;
    std::cout << std::endl << "\t\t\ttetrahedrons: " << std::setw(6) << std::setfill(' ') << std::right << gD->tetrahedrons.size();
    std::cout << std::endl << "\t\t\thexahedrons : " << std::setw(6) << std::setfill(' ') << std::right << gD->hexahedrons.size();
    std::cout << std::endl << "\t\t\tprisms    : " << std::setw(6) << std::setfill(' ') << std::right << gD->prisms.size();
    std::cout << std::endl << "\t\t\tpyramids  : " << std::setw(6) << std::setfill(' ') << std::right << gD->pyramids.size() << std::endl;

    int numberOfFacets = gD->triangles.size() + gD->quadrangles.size();
    std::cout << std::endl << "\t\tnumberOfFacets  : " << gD->triangles.size() + gD->quadrangles.size();
    std::cout << std::endl << "\t\t\ttriangle  : " << std::setw(6) << std::setfill(' ') << std::right << gD->triangles.size();
    std::cout << std::endl << "\t\t\tquadrangle: " << std::setw(6) << std::setfill(' ') << std::right << gD->quadrangles.size() << std::endl;

    std::cout << std::endl << "\t\tnumberOfLines: " << gD->lines.size() << std::endl;

    std::cout << std::endl << "\t\ttotal: " << numberOfElements + numberOfFacets + gD->lines.size() << std::endl;

    std::cout << std::endl << "\t\tnumberOfRegions: " << gD->regions.size();
    for (auto r : gD->regions)
        std::cout << std::endl << "\t\t\t" << std::setw(20) << std::left << r.name << ": " << std::setw(6) << std::right << r.begin << " - " << r.end << "\t|" << std::setw(6) << r.end - r.begin;
    std::cout << std::endl;

    std::cout << std::endl << "\t\tnumberOfBoundaries: " << gD->boundaries.size();
    for (auto b : gD->boundaries)
        std::cout << std::endl << "\t\t\t" << std::setw(20) << std::left << b.name << ": " << std::setw(6) << std::right << b.begin << " - " << b.end << "\t|" << std::setw(6) << b.end - b.begin;
    std::cout << std::endl;

    std::cout << std::endl << "\t\tnumberOfWells: " << gD->wells.size();
    for (auto w : gD->wells)
        std::cout << std::endl << "\t\t\t" << std::setw(20) << std::left << w.name << ": " << std::setw(6) << std::right << w.begin << " - " << w.end << "\t|" << std::setw(6) << w.end - w.begin;

    std::cout << std::endl << std::endl;
}
