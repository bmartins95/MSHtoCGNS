#include "MSHtoCGNS/BoostInterface/Test.hpp"
#include "MSHtoCGNS/Manipulation/CgnsReader/SpecialCgnsReader.hpp"
#include "MSHtoCGNS/Manipulation/Generator/CartesianWellGenerator.hpp"
#include <cgnslib.h>

struct CartesianWellGeneratorFixture {
    CartesianWellGeneratorFixture() {
        SpecialCgnsReader specialCgnsReader(this->inputPath);
        this->gridData = specialCgnsReader.gridData;
    }

    std::string inputPath = std::string(TEST_INPUT_DIRECTORY) + "Manipulation/CartesianWellGenerator/275v_160e.cgns";
    boost::shared_ptr<GridData> gridData;
    std::string cartesianWellGeneratorScript = std::string(TEST_INPUT_DIRECTORY) + "Manipulation/CartesianWellGenerator/CartesianWellGenerator.json";
};

FixtureTestSuite(CartesianWellGeneratorSuite, CartesianWellGeneratorFixture)

TestCase(CartesianWellGeneratorTest) {
    checkEqual(std::count_if(this->gridData->sections.cbegin(), this->gridData->sections.cend(), [](const auto& e){return e.dimension == 1;}), 0);
    checkEqual(std::count_if(this->gridData->connectivities.cbegin(), this->gridData->connectivities.cend(), [](const auto& c){return c.front() == BAR_2;}), 0);

    CartesianWellGenerator cartesianWellGenerator(this->gridData, this->cartesianWellGeneratorScript);

    checkEqual(std::count_if(this->gridData->sections.cbegin(), this->gridData->sections.cend(), [](const auto& e){return e.dimension == 1;}), 1);
    checkEqual(std::count_if(this->gridData->connectivities.cbegin(), this->gridData->connectivities.cend(), [](const auto& c){return c.front() == BAR_2;}), 5);

}

TestSuiteEnd()
