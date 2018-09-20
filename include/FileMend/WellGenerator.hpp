#ifndef WELL_GENERATOR_HPP
#define WELL_GENERATOR_HPP

#include <set>
#include <algorithm>
#include <numeric>

#include <BoostInterface/Filesystem.hpp>
#include <BoostInterface/PropertyTree.hpp>
#include <Utilities/Vector.hpp>
#include <Grid/GridData.hpp>

#define TOLERANCE 1e-3

struct WellGeneratorData {
	std::string regionName;
	std::array<double, 3> wellStart;
	int wellDirection;
	std::string wellName;
};

class WellGenerator {
	public:
		WellGenerator(GridDataShared gridData, std::string wellGeneratorScript);

		~WellGenerator() = default;

	private:
		void checkGridData();
		void readScript();
		void buildElementConnectivities();
		void generateWells();
		void defineQuantities(std::vector<std::vector<int>> prisms);
		bool isClose(const std::array<double, 3>& coordinate, const std::array<double, 3>& referencePoint);

		GridDataShared gridData;
		std::string wellGeneratorScript;
		std::vector<WellGeneratorData> wellGeneratorDatum;
		std::vector<std::vector<int>> elementConnectivities;
		int lineConnectivityShift;

		int currentIndex = -1;
		int numberOfElementsPerSection;
		int numberOfSegments;
		int numberOfPrisms;
};

#endif