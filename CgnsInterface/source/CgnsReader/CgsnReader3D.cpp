#include <CgnsInterface/CgnsReader/CgnsReader3D.hpp>
#include <cgnslib.h>

CgnsReader3D::CgnsReader3D(const std::string& filePath) : CgnsReader(filePath) {
	this->readCoordinates();
	this->readSections();
	this->readBoundaries();
}

void CgnsReader3D::readCoordinates() {
	int one = 1;
	double coordinatesX[this->sizes[0]];
	if (cg_coord_read(this->fileIndex, this->baseIndex, this->zoneIndex, "CoordinateX", RealDouble, &one, &this->sizes[0], coordinatesX))
		throw std::runtime_error("CgnsReader3D: Could not read CoordinateX");

	double coordinatesY[this->sizes[0]];
	if (cg_coord_read(this->fileIndex, this->baseIndex, this->zoneIndex, "CoordinateY", RealDouble, &one, &this->sizes[0], coordinatesY))
		throw std::runtime_error("CgnsReader3D: Could not read CoordinateY");

	double coordinatesZ[this->sizes[0]];
	if (cg_coord_read(this->fileIndex, this->baseIndex, this->zoneIndex, "CoordinateZ", RealDouble, &one, &this->sizes[0], coordinatesZ))
		throw std::runtime_error("CgnsReader3D: Could not read CoordinateZ");

	this->gridData->coordinates.resize(this->sizes[0], std::array<double, 3>());
	for (int i = 0; i < this->sizes[0]; i++) {
		this->gridData->coordinates[i][0] = coordinatesX[i];
		this->gridData->coordinates[i][1] = coordinatesY[i];
		this->gridData->coordinates[i][2] = coordinatesZ[i];
	}
}

void CgnsReader3D::readSections() {
	for (int sectionIndex = 1; sectionIndex <= this->numberOfSections; sectionIndex++) {
		ElementType_t elementType;
		int elementStart, elementEnd;
		int lastBoundaryElement, parentFlag;
		if (cg_section_read(this->fileIndex, this->baseIndex, this->zoneIndex, sectionIndex, this->buffer, &elementType, &elementStart, &elementEnd,
								&lastBoundaryElement, &parentFlag))
			throw std::runtime_error("CgnsReader3D: Could not read section");
		int numberOfElements = elementEnd - elementStart + 1;

		int size;
		if (cg_ElementDataSize(this->fileIndex, this->baseIndex, this->zoneIndex, sectionIndex, &size))
			throw std::runtime_error("CgnsReader3D: Could not read element data size");

		int connectivities[size];
		if (cg_elements_read(this->fileIndex, this->baseIndex, this->zoneIndex, sectionIndex, connectivities, nullptr))
			throw std::runtime_error("CgnsReader3D: Could not read section elements");

		if (elementType == MIXED)
			if (ElementType_t(connectivities[0]) == TETRA_4 || ElementType_t(connectivities[0]) == HEXA_8 || ElementType_t(connectivities[0]) == PENTA_6 || ElementType_t(connectivities[0]) == PYRA_5)
				this->addRegion(std::string(this->buffer), elementStart, numberOfElements);
			else
				this->addBoundary(std::string(this->buffer), elementStart, numberOfElements);
		else if (elementType == TETRA_4 || elementType == HEXA_8)
				this->addRegion(std::string(this->buffer), elementStart, numberOfElements);
		else if (elementType == TRI_3 || elementType == QUAD_4)
			this->addBoundary(std::string(this->buffer), elementStart, numberOfElements);
		else if (elementType == BAR_2)
			this->addWell(std::string(this->buffer), elementStart, numberOfElements);
		else
			throw std::runtime_error("CgnsReader3D: Section element type not supported");

		int numberOfVertices;
		if (cg_npe(elementType, &numberOfVertices))
			throw std::runtime_error("CgnsReader3D: Could not read element number of vertices");

		switch (elementType) {
			case MIXED : {
				int position = 0;
				for (int e = 0; e < numberOfElements; e++) {
					cg_npe(ElementType_t(connectivities[position]), &numberOfVertices);
					std::vector<int> element(numberOfVertices);
					for (int k = 0; k < numberOfVertices; ++k)
						element[k] = connectivities[position+1+k] - 1;
					element.emplace_back(elementStart - 1 + e);
					switch(connectivities[position]) {
						case TETRA_4: {
							std::array<int, 5> tetrahedron;
							std::copy_n(std::begin(element), 5, std::begin(tetrahedron));
							this->gridData->tetrahedronConnectivity.emplace_back(std::move(tetrahedron));
							break;
						}
						case HEXA_8: {
							std::array<int, 9> hexahedron;
							std::copy_n(std::begin(element), 9, std::begin(hexahedron));
							this->gridData->hexahedronConnectivity.emplace_back(std::move(hexahedron));
							break;
						}
						case PENTA_6: {
							std::array<int, 7> prism;
							std::copy_n(std::begin(element), 7, std::begin(prism));
							this->gridData->prismConnectivity.emplace_back(std::move(prism));
							break;
						}
						case PYRA_5: {
							std::array<int, 6> pyramid;
							std::copy_n(std::begin(element), 6, std::begin(pyramid));
							this->gridData->pyramidConnectivity.emplace_back(std::move(pyramid));
							break;
						}
						case TRI_3: {
							std::array<int, 4> triangle;
							std::copy_n(std::begin(element), 4, std::begin(triangle));
							this->gridData->triangleConnectivity.emplace_back(std::move(triangle));
							break;
						}
						case QUAD_4: {
							std::array<int, 5> quadrangle;
							std::copy_n(std::begin(element), 5, std::begin(quadrangle));
							this->gridData->quadrangleConnectivity.emplace_back(std::move(quadrangle));
							break;
						}
						// default:
							// throw std::runtime_error("CgnsReader3D: Element type " + std::to_string(elementType) + " in MIXED section not supported");
					}
					position += numberOfVertices + 1;
				}
				break;
			}
			case TETRA_4: {
				for (int e = 0; e < numberOfElements; e++) {
					std::array<int, 5> tetrahedron;
					for (int k = 0; k < numberOfVertices; k++)
						tetrahedron[k] = connectivities[e*numberOfVertices+k] - 1;
					tetrahedron.back() = (elementStart - 1 + e);
					this->gridData->tetrahedronConnectivity.emplace_back(std::move(tetrahedron));
				}
				break;
			}
			case HEXA_8: {
				for (int e = 0; e < numberOfElements; e++) {
					std::array<int, 9> hexahedron;
					for (int k = 0; k < numberOfVertices; k++)
						hexahedron[k] = connectivities[e*numberOfVertices+k] - 1;
					hexahedron.back() = (elementStart - 1 + e);
					this->gridData->hexahedronConnectivity.emplace_back(std::move(hexahedron));
				}
				break;
			}
			case TRI_3: {
				for (int e = 0; e < numberOfElements; e++) {
					std::array<int, 4> triangle;
					for (int k = 0; k < numberOfVertices; k++)
						triangle[k] = connectivities[e*numberOfVertices+k] - 1;
					triangle.back() = (elementStart - 1 + e);
					this->gridData->triangleConnectivity.emplace_back(std::move(triangle));
				}
				break;
			}
			case QUAD_4: {
				for (int e = 0; e < numberOfElements; e++) {
					std::array<int, 5> quadrangle;
					for (int k = 0; k < numberOfVertices; k++)
						quadrangle[k] = connectivities[e*numberOfVertices+k] - 1;
					quadrangle.back() = (elementStart - 1 + e);
					this->gridData->quadrangleConnectivity.emplace_back(std::move(quadrangle));
				}
				break;
			}
			case BAR_2: {
				for (int e = 0; e < numberOfElements; e++) {
					std::array<int, 3> line;
					for (int k = 0; k < numberOfVertices; k++)
						line[k] = connectivities[e*numberOfVertices+k] - 1;
					line.back() = (elementStart - 1 + e);
					this->gridData->lineConnectivity.emplace_back(std::move(line));
				}
				break;
			}
			default:
				throw std::runtime_error("CgnsReader3D: Section element type not supported");
		}
	}
}

void CgnsReader3D::addWell(std::string&& name, int elementStart, int numberOfElements) {
	WellData well;
	well.name = name;
	well.elementsOnWell.resize(numberOfElements);
	std::iota(well.elementsOnWell.begin(), well.elementsOnWell.end(), elementStart - 1);
	this->gridData->wells.emplace_back(std::move(well));
	// printf("\n\nHullo\n\n");
}
