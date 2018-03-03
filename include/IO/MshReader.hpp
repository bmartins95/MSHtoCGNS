#ifndef MSH_READER_HPP
#define MSH_READER_HPP

#include <BoostInterface/Filesystem.hpp>
#include <Grid/GridData.hpp>
#include <Utilities/Vector.hpp>
#include <Utilities/Set.hpp>

#include <string>
#include <fstream>
#include <numeric>

class MshReader {
	public:
		MshReader() = default;
		MshReader(const std::string&);

		GridDataShared getGridData() const;

		~MshReader();

	protected:
		void checkFile();
		void readNodes();
		void readConnectivities();
		virtual void processConnectivities() = 0;
		virtual void readPhysicalEntities() = 0;
		virtual void addElements() = 0;
		virtual void addFacets() = 0;
		virtual void defineBoundaryVertices() = 0;

		std::string filePath;
		std::ifstream file;
		char* buffer;
		int numberOfPhysicalEntities, numberOfBoundaries, numberOfRegions;
		std::vector<std::vector<int>> connectivities, elements, facets, elementsOnRegion, facetsOnBoundary;
		GridDataShared gridData;
};

#endif