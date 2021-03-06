#include "MSHtoCGNS/BoostInterface/Test.hpp"
#include "MSHtoCGNS/BoostInterface/PropertyTree.hpp"
#include "MSHtoCGNS/CgnsInterface/CgnsReader.hpp"
#include "MSHtoCGNS/Manipulation/GridDataExtractor.hpp"
#include "MSHtoCGNS/Manipulation/MultipleBasesCgnsCreator.hpp"
#include <cgnslib.h>

#define TOLERANCE 1e-12

struct MultipleBasesCgnsCreatorFixture {
    MultipleBasesCgnsCreatorFixture() {
        boost::property_tree::ptree script;
        boost::property_tree::read_json(this->multipleBasesScript, script);

        CgnsReader cgnsReader(std::string(TEST_INPUT_DIRECTORY) + script.get<std::string>("path.input"));
        GridDataExtractor gridDataExtractor(cgnsReader.gridData, script.get_child("ScriptGridDataExtractor"));

        this->gridDatas.emplace_back(cgnsReader.gridData);
        this->gridDatas.emplace_back(gridDataExtractor.extract);

        MultipleBasesCgnsCreator multipleBasesCgnsCreator(this->gridDatas, this->baseNames, this->zoneNames, "./");

        this->filePath = multipleBasesCgnsCreator.getFileName();
        cg_open(this->filePath.c_str(), CG_MODE_READ, &this->fileIndex);
    }

    ~MultipleBasesCgnsCreatorFixture() {
        cg_close(this->fileIndex);
        boost::filesystem::remove_all("./125v_64e");
    };

    std::string inputPath =  + "";
    std::string multipleBasesScript = std::string(TEST_INPUT_DIRECTORY) + "Manipulation/MultipleBasesCgnsCreator/MultipleBases.json";
    std::vector<boost::shared_ptr<GridData>> gridDatas;
    std::vector<std::string> baseNames{"ROCK_BASE", "RESERVOIR_BASE"};
    std::vector<std::string> zoneNames{"ROCK_ZONE", "RESERVOIR_ZONE"};

    std::string filePath;
    int fileIndex;
    char buffer[1024];
    int cellDimension, physicalDimension;
    int sizes[3];
    ElementType_t type;
    int elementStart;
    int elementEnd;
    int nbndry;
    int parent_flag;
};

FixtureTestSuite(MultipleBasesCgnsCreatorSuite, MultipleBasesCgnsCreatorFixture)

TestCase(Base1Case) {
    int numberOfBases;
    cg_nbases(this->fileIndex, &numberOfBases);
    checkEqual(numberOfBases, 2);

    int baseIndex = 1;
    cg_base_read(this->fileIndex, baseIndex, this->buffer, &this->cellDimension, &this->physicalDimension);
    checkEqual(std::string(this->buffer), "ROCK_BASE");

    int numberOfZones;
    cg_nzones(this->fileIndex, baseIndex, &numberOfZones);
    checkEqual(numberOfZones, 1);

    int zoneIndex = 1;
    cg_zone_read(this->fileIndex, baseIndex, zoneIndex, this->buffer, this->sizes);
    checkEqual(std::string(this->buffer), "ROCK_ZONE");
    checkEqual(this->sizes[0], 125);
    checkEqual(this->sizes[1], 64);
    checkEqual(this->sizes[2], 0);

    int numberOfSections;
    cg_nsections(this->fileIndex, baseIndex, zoneIndex, &numberOfSections);
    checkEqual(numberOfSections, 9);
}

TestCase(Base2Case) {
    int baseIndex = 2;
    cg_base_read(this->fileIndex, baseIndex, this->buffer, &this->cellDimension, &this->physicalDimension);
    checkEqual(std::string(this->buffer), "RESERVOIR_BASE");

    int numberOfZones;
    cg_nzones(this->fileIndex, baseIndex, &numberOfZones);
    checkEqual(numberOfZones, 1);

    int zoneIndex = 1;
    cg_zone_read(this->fileIndex, baseIndex, zoneIndex, this->buffer, this->sizes);
    checkEqual(std::string(this->buffer), "RESERVOIR_ZONE");
    checkEqual(this->sizes[0], 27);
    checkEqual(this->sizes[1], 8);
    checkEqual(this->sizes[2], 0);

    int numberOfSections;
    cg_nsections(this->fileIndex, baseIndex, zoneIndex, &numberOfSections);
    checkEqual(numberOfSections, 2);
}

TestSuiteEnd()
