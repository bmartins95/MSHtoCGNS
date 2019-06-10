#include "MSHtoCGNS/BoostInterface/Test.hpp"
#include "MSHtoCGNS/MshInterface/MshReaderFixture.hpp"

#define TOLERANCE 1e-12

struct Read_Region1_Hexahedrons_Well1_3D_Fixture : public MshReaderFixture {
    Read_Region1_Hexahedrons_Well1_3D_Fixture() : MshReaderFixture("MshInterface/3D-Region3-Hexahedrons-Well1/88v_30e.msh") {}
};

FixtureTestSuite(Read_Region3_Hexahedrons_Well1_3D_Suite, Read_Region1_Hexahedrons_Well1_3D_Fixture)

TestCase(Lines) {
    checkEqual(this->gridData->lines.size(), 1u);

    checkEqual(this->gridData->lines[0][0], 4); checkEqual(this->gridData->lines[0][1], 3); checkEqual(this->gridData->lines[0][2], 116);
}

TestCase(Wells) {
    checkEqual(this->gridData->wells.size(), 1u);
}

TestCase(Line) {
    auto well = this->gridData->wells[0];

    checkEqual(well.name, std::string("LINE"));

    checkEqual(well.begin, 116);
    checkEqual(well.end, 117);

    auto vertices = well.vertices;
    checkEqual(vertices.size(), 2u);
    checkEqual(vertices[0],  3);
    checkEqual(vertices[1],  4);
}

TestSuiteEnd()