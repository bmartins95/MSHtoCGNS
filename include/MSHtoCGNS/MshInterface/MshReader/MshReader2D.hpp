#ifndef MSH_READER_2D_HPP
#define MSH_READER_2D_HPP

#include "MSHtoCGNS/MshInterface/MshReader.hpp"

class MshReader2D : public MshReader {
    public:
        MshReader2D(std::string filePath);

        ~MshReader2D() = default;

    private:
        void addPhysicalEntities() override;
        void addElements() override;
        void addFacets() override;
        void findBoundaryVertices() override;
        void findRegionVertices() override;
};

#endif
