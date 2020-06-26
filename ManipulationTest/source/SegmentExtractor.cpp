#include "MSHtoCGNS/BoostInterface/Test.hpp"
#include "MSHtoCGNS/UnitTest/FixtureCgnsReader.hpp"
#include "MSHtoCGNS/Manipulation/RadialGridDataReordered.hpp"
#include "MSHtoCGNS/Manipulation/SegmentExtractor.hpp"
#include <cgnslib.h>
#include <cmath>

#undef TOLERANCE
#define TOLERANCE 1.0e-4

struct SegmentExtractorFixture : public FixtureCgnsReader {
    SegmentExtractorFixture() : FixtureCgnsReader("Manipulation/RadialGridDataReordered/370v_324e.cgns") {
        RadialGridDataReordered radialGridDataReordered(this->gridData);
        this->gridData = radialGridDataReordered.reordered;
    }

    double calculateDistance(std::array<double, 3> a, std::array<double, 3> b) {
        return std::sqrt(std::inner_product(a.cbegin(), a.cend(), b.cbegin(), 0.0, std::plus<>(), [](auto c, auto d){return (c-d)*(c-d);}));
    }
};

FixtureTestSuite(SegmentExtractorSuite, SegmentExtractorFixture)

TestCase(SegmentExtractorTest) {
    SegmentExtractor SegmentExtractor(this->gridData);
    auto segment = SegmentExtractor.segment;

    checkEqual(segment->coordinates.size(), 74u);

    {
        auto connectivities = segment->connectivities;
        checkEqual(std::count_if(connectivities.cbegin(), connectivities.cend(), [](const auto& c){return c.front() == HEXA_8 ;}), 24);
        checkEqual(std::count_if(connectivities.cbegin(), connectivities.cend(), [](const auto& c){return c.front() == PENTA_6;}), 12);
        checkEqual(std::count_if(connectivities.cbegin(), connectivities.cend(), [](const auto& c){return c.front() == TRI_3  ;}), 24);
        checkEqual(std::count_if(connectivities.cbegin(), connectivities.cend(), [](const auto& c){return c.front() == QUAD_4 ;}), 60);
        checkEqual(std::count_if(connectivities.cbegin(), connectivities.cend(), [](const auto& c){return c.front() == BAR_2  ;}),  1);
    }

    {
        auto sections = segment->sections;
        checkEqual(sections.size(), 5u);

        checkEqual(std::count_if(sections.cbegin(), sections.cend(), [](const auto& e){return e.dimension == 3;}), 1);
        checkEqual(std::count_if(sections.cbegin(), sections.cend(), [](const auto& e){return e.dimension == 2;}), 3);
        checkEqual(std::count_if(sections.cbegin(), sections.cend(), [](const auto& e){return e.dimension == 1;}), 1);

        {
            auto section = sections[0];
            checkEqual(section.name, std::string("BODY_WELL"));
            checkEqual(section.dimension, 3);
            checkEqual(section.begin, 0);
            checkEqual(section.end, 36);
        }

        {
            auto section = sections[1];
            checkEqual(section.name, std::string("BOUNDARY_SURFACE"));
            checkEqual(section.dimension, 2);
            checkEqual(section.begin, 36);
            checkEqual(section.end, 48);
            std::vector<int> expected{13, 14, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 50, 51, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72};
            checkEqualCollections(section.vertices.cbegin(), section.vertices.cend(), expected.cbegin(), expected.cend());
        }

        {
            auto section = sections[2];
            checkEqual(section.name, std::string("BOUNDARY_BOTTOM"));
            checkEqual(section.dimension, 2);
            checkEqual(section.begin, 48);
            checkEqual(section.end, 84);
            checkEqual(section.vertices.size(), 37u);
            checkEqual(std::all_of(section.vertices.cbegin(), section.vertices.cend(), [=](auto v){return v >= 0 && v < 37;}), true);
        }

        {
            auto section = sections[3];
            checkEqual(section.name, std::string("BOUNDARY_TOP"));
            checkEqual(section.dimension, 2);
            checkEqual(section.begin, 84);
            checkEqual(section.end, 120);
            checkEqual(section.vertices.size(), 37u);
            checkEqual(std::all_of(section.vertices.cbegin(), section.vertices.cend(), [=](auto v){return v >= 37 && v < 74;}), true);
        }

        {
            auto section = sections[4];
            checkEqual(section.name, std::string("LINE_WELL"));
            checkEqual(section.dimension, 1);
            checkEqual(section.begin, 120);
            checkEqual(section.end, 121);
            std::vector<int> expected{0, 37};
            checkEqualCollections(section.vertices.cbegin(), section.vertices.cend(), expected.cbegin(), expected.cend());
        }
    }

    // for (int j = 0; j < 12; ++j)
    //     checkEqual(segment->prisms[j].back(), j);

    // for (int j = 0; j < 24; ++j)
    //     checkEqual(segment->hexahedrons[j].back(), 12 + j);

    // for (int j = 0; j < 12; ++j)
    //     checkEqual(segment->quadrangles[j].back(), 36 + j);

    // for (int j = 0; j < 12; ++j) {
    //     checkEqual(segment->triangles[j     ].back(), 48 + j);
    //     checkEqual(segment->triangles[j + 12].back(), 84 + j);
    // }

    // for (int j = 0; j < 24; ++j) {
    //     checkEqual(segment->quadrangles[j + 12].back(), 60 + j);
    //     checkEqual(segment->quadrangles[j + 36].back(), 96 + j);
    // }

    // checkEqual(segment->lines[0].back(), 120);

    // for (int j = 0; j < 12; ++j)
    //     checkEqual(std::all_of(segment->triangles[j + 12].cbegin(), segment->triangles[j + 12].cend() - 1, [=](auto v){return v >= 37 && v < 74;}), true);

    // for (int j = 0; j < 24; ++j)
    //     checkEqual(std::all_of(segment->quadrangles[j + 36].cbegin(), segment->quadrangles[j + 36].cend() - 1, [=](auto v){return v >= 37 && v < 74;}), true);

    // for (int j = 0; j < 37; ++j) {
    //     checkSmall(segment->coordinates[37 * 0 + j][2], TOLERANCE);
    //     checkClose(segment->coordinates[37 * 1 + j][2], 5.5555555, TOLERANCE);
    // }

    // for (int j = 0; j < 37; ++j)
    //     checkClose(fabs(segment->coordinates[37 * 0 + j][2] - segment->coordinates[37 *  1 + j][2]), 5.5555555555555554e+00, TOLERANCE);

    // for (int j = 0; j < 37; ++j)
    //     checkClose(calculateDistance(segment->coordinates[37 * 0 + j], segment->coordinates[37 *  1 + j]), 5.5555555555555554e+00, TOLERANCE);
}

TestSuiteEnd()
