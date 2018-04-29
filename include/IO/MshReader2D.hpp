#ifndef MSH_READER_2D_HPP
#define MSH_READER_2D_HPP

#include <IO/MshReader.hpp>

class MshReader2D : public MshReader {
	public:
		MshReader2D() = default;
		MshReader2D(const std::string&);

		~MshReader2D() = default;

	private:
		void readPhysicalEntities() override;
		void divideConnectivities() override;
		void addBoundaries() override;
		void addRegions() override;
		void defineBoundaryVertices() override;
};

#endif