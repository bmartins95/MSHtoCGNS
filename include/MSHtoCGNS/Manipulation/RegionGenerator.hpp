#ifndef __MANIPULATION_REGION_GENERATOR_HPP_HPP__
#define __MANIPULATION_REGION_GENERATOR_HPP_HPP__

#include <algorithm>

#include <boost/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/unordered_map.hpp>

#include "MSHtoCGNS/Utilities/Print.hpp"
#include "MSHtoCGNS/BoostInterface/String.hpp"

#include "MSHtoCGNS/BoostInterface/Filesystem.hpp"
#include "MSHtoCGNS/BoostInterface/PropertyTree.hpp"
#include "MSHtoCGNS/Utilities/Vector.hpp"
#include "MSHtoCGNS/Utilities/Algorithm.hpp"
#include "MSHtoCGNS/Utilities/Error.hpp"
#include "MSHtoCGNS/GridData/GridData.hpp"

using Point = boost::geometry::model::point<double, 3, boost::geometry::cs::cartesian>;
using Box = boost::geometry::model::box<Point>;
using MultiPoint = boost::geometry::model::multi_point<Point>;

struct RegionGeneratorData {
    std::string name;
    Box box;
};

class RegionGenerator {
    public:
        RegionGenerator(boost::shared_ptr<GridData> gridData, std::string regionGeneratorScript);

        ~RegionGenerator() = default;

    private:
        void readScript();
        void generateCoordinates();
        void generateRegions();
        bool isElementInRange(std::vector<std::vector<int>>::iterator connectivity, const Box& box);

        boost::shared_ptr<GridData> gridData;
        boost::property_tree::ptree propertyTree;

        std::vector<Point> coordinates;
        std::vector<RegionGeneratorData> regionGeneratorDatum;

        int marker = -1;
};

#endif
