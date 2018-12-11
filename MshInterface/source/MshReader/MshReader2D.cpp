#include "MSHtoCGNS/MshInterface/MshReader/MshReader2D.hpp"

MshReader2D::MshReader2D(std::string filePath) : MshReader(filePath) {
    this->gridData->dimension = 2;
    this->addPhysicalEntities();
    this->addElements();
    this->addFacets();
    this->defineBoundaryVertices();
}

void MshReader2D::addPhysicalEntities() {
    for (int physicalEntity = 0; physicalEntity < this->numberOfPhysicalEntities; ++physicalEntity) {
        switch (this->entitiesTypes[physicalEntity]) {
            case 0: {
                this->addBoundary(this->entitiesNames[physicalEntity], this->physicalEntitiesRange[physicalEntity].front(), this->physicalEntitiesRange[physicalEntity].back());
                break;
            }
            case 1: {
                this->addRegion(this->entitiesNames[physicalEntity], this->physicalEntitiesRange[physicalEntity].front(), this->physicalEntitiesRange[physicalEntity].back());
                break;
            }
            default:
                throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Physical entity " + std::to_string(this->entitiesTypes[physicalEntity]) + " not supported");
        }
    }
}

void MshReader2D::addElements() {
    for (auto& region : this->gridData->regions) {
        auto begin = this->connectivities.begin() + region.begin;
        auto end = this->connectivities.begin() + region.end;
        region.begin = this->shift;
        while (begin != end) {
            begin->push_back(this->shift++);
            switch ((*begin)[this->typeIndex]) {
                case 1: {
                    this->gridData->triangleConnectivity.emplace_back(std::array<int, 4>());
                    std::copy_n(std::begin(*begin++) + this->nodeIndex, 4, std::begin(this->gridData->triangleConnectivity.back()));
                    break;
                }
                case 2: {
                    this->gridData->quadrangleConnectivity.emplace_back(std::array<int, 5>());
                    std::copy_n(std::begin(*begin++) + this->nodeIndex, 5, std::begin(this->gridData->quadrangleConnectivity.back()));
                    break;
                }
                default:
                    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Non supported element found");
            }
        }
        region.end = this->shift;
    }
}

void MshReader2D::addFacets() {
    for (auto& boundary : this->gridData->boundaries) {
        auto begin = this->connectivities.begin() + boundary.begin;
        auto end = this->connectivities.begin() + boundary.end;
        boundary.begin = this->shift;
        while (begin != end) {
            begin->push_back(this->shift++);
            switch ((*begin)[this->typeIndex]) {
                case 0: {
                    this->gridData->lineConnectivity.emplace_back(std::array<int, 3>());
                    std::copy_n(std::begin(*begin++) + this->nodeIndex, 3, std::begin(this->gridData->lineConnectivity.back()));
                    break;
                }
                default:
                    throw std::runtime_error(std::string(__PRETTY_FUNCTION__) + " - Non supported facet found");
            }
        }
        boundary.end = this->shift;
    }
}

void MshReader2D::defineBoundaryVertices() {
    for (auto& boundary : this->gridData->boundaries) {
        std::set<int> vertices;

        for (const auto& line : this->gridData->lineConnectivity)
            if (line.back() >= boundary.begin && line.back() < boundary.end)
                vertices.insert(line.cbegin(), line.cend() - 1);

        boundary.vertices = std::vector<int>(vertices.cbegin(), vertices.cend());
    }
}
