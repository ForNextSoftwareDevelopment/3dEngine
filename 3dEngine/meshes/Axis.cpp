#include "Axis.h"

/*********************************************************************
* Constructor
*********************************************************************/
Axis::Axis(GLfloat size, char direction)
{
	if ((direction != 'x') && (direction != 'y') && (direction != 'z')) return;

	name = "AxisPosX";

	GLfloat sizeX = size;
	GLfloat sizeY = size;
	GLfloat sizeZ = size;

	if (direction == 'x') sizeX = 100 * size;
	if (direction == 'y') sizeY = 100 * size;
	if (direction == 'z') sizeZ = 100 * size;

	// Number of vertices in an Axis is 16
	numVertices = 16;

	// Number of faces in an Axis is 24 (triangles, not squares)
	numFaces = 24;

	// Fill vertices for the Axis (positive side)
	vertices[0].x = 0.0f;
	vertices[0].y = 0.0f;
	vertices[0].z = sizeZ/2;

	vertices[1].x = 0.0f;
	vertices[1].y = sizeY/2;
	vertices[1].z = sizeZ/2;

	vertices[2].x = sizeX/2;
	vertices[2].y = sizeY/2;
	vertices[2].z = sizeZ/2;

	vertices[3].x = sizeX/2;
	vertices[3].y = 0.0f;
	vertices[3].z = sizeZ/2;

	vertices[4].x = 0.0f;
	vertices[4].y = 0.0f;
	vertices[4].z = 0.0f;

	vertices[5].x = 0.0f;
	vertices[5].y = sizeY/2;
	vertices[5].z = 0.0f;

	vertices[6].x = sizeX/2;
	vertices[6].y = sizeY/2;
	vertices[6].z = 0.0f;

	vertices[7].x = sizeX/2;
	vertices[7].y = 0.0f;
	vertices[7].z = 0.0f;

	// Fill vertices for the Axis (negative side) 
	vertices[8].x = direction == 'x' ? -sizeX / 2 : sizeX / 2;
	vertices[8].y = direction == 'y' ? -sizeY / 2 : sizeY / 2;
	vertices[8].z = 0.0f;

	vertices[9].x = direction == 'x' ? -sizeX / 2 : sizeX / 2;
	vertices[9].y = 0.0f;
	vertices[9].z = direction == 'z' ? -sizeZ / 2 : sizeZ / 2;

	vertices[10].x = 0.0f;
	vertices[10].y = 0.0f;
	vertices[10].z = 0.0f;

	vertices[11].x = 0.0f;
	vertices[11].y = direction == 'y' ? -sizeY / 2 : sizeY / 2;
	vertices[11].z = 0.0f;

	vertices[12].x = direction == 'x' ? -sizeX / 2 : sizeX / 2;
	vertices[12].y = direction == 'y' ? -sizeY / 2 : sizeY / 2;
	vertices[12].z = direction == 'z' ? -sizeZ / 2 : sizeZ / 2;

	vertices[13].x = direction == 'x' ? -sizeX / 2 : sizeX / 2;
	vertices[13].y = 0.0f;
	vertices[13].z = direction == 'z' ? -sizeZ / 2 : sizeZ / 2;

	vertices[14].x = 0.0f;
	vertices[14].y = 0.0f;
	vertices[14].z = direction == 'z' ? -sizeZ / 2 : sizeZ / 2;

	vertices[15].x = 0.0f;
	vertices[15].y = direction == 'y' ? -sizeY / 2 : sizeY / 2;
	vertices[15].z = direction == 'z' ? -sizeZ / 2 : sizeZ / 2;

	// Set vertices pointer to the array of vertices just filled
	pVertices = vertices;

	// Fill faces for the Axis (positive: 6 squares containing each 2 triangle vertices, making 12 faces)
	faces[0].first = 0;
	faces[0].second = 1;
	faces[0].third = 2;

	faces[1].first = 0;
	faces[1].second = 2;
	faces[1].third = 3;

	faces[2].first = 4;
	faces[2].second = 5;
	faces[2].third = 6;

	faces[3].first = 4;
	faces[3].second = 7;
	faces[3].third = 6;

	faces[4].first = 0;
	faces[4].second = 1;
	faces[4].third = 5;

	faces[5].first = 0;
	faces[5].second = 4;
	faces[5].third = 5;

	faces[6].first = 2;
	faces[6].second = 3;
	faces[6].third = 7;

	faces[7].first = 2;
	faces[7].second = 6;
	faces[7].third = 7;

	faces[8].first = 1;
	faces[8].second = 2;
	faces[8].third = 6;

	faces[9].first = 1;
	faces[9].second = 5;
	faces[9].third = 6;

	faces[10].first = 0;
	faces[10].second = 3;
	faces[10].third = 7;

	faces[11].first = 0;
	faces[11].second = 4;
	faces[11].third = 7;

	// Fill faces for the Axis (negative: 6 squares containing each 2 triangle vertices, making 12 faces)
	faces[12].first = 8;
	faces[12].second = 9;
	faces[12].third = 10;

	faces[13].first = 8;
	faces[13].second = 10;
	faces[13].third = 11;

	faces[14].first = 12;
	faces[14].second = 13;
	faces[14].third = 14;

	faces[15].first = 12;
	faces[15].second = 15;
	faces[15].third = 14;

	faces[16].first = 8;
	faces[16].second = 9;
	faces[16].third = 13;

	faces[17].first = 8;
	faces[17].second = 12;
	faces[17].third = 13;

	faces[18].first = 10;
	faces[18].second = 11;
	faces[18].third = 15;
	
	faces[19].first = 10;
	faces[19].second = 14;
	faces[19].third = 15;

	faces[20].first = 9;
	faces[20].second = 10;
	faces[20].third = 14;

	faces[21].first = 9;
	faces[21].second = 13;
	faces[21].third = 14;

	faces[22].first = 8;
	faces[22].second = 11;
	faces[22].third = 15;

	faces[23].first = 8;
	faces[23].second = 12;
	faces[23].third = 15;

	// Set faces pointer to the array of faces just filled
	pFaces = faces;

	// Assign materials to the mesh
	numMaterials = 2;
	Material *pMaterialPos = new Material((name + "-pos").c_str());
	if (direction == 'x') pMaterialPos->pKa = new VecMat::Vec3(1.0f, 0.0f, 0.0f);
	if (direction == 'y') pMaterialPos->pKa = new VecMat::Vec3(1.0f, 1.0f, 0.0f);
	if (direction == 'z') pMaterialPos->pKa = new VecMat::Vec3(1.0f, 1.0f, 1.0f);
	pMaterialEntryList[0] = new MaterialEntry();
	pMaterialEntryList[0]->pMaterial = pMaterialPos;
	pMaterialEntryList[0]->start = 0;

	Material *pMaterialNeg = new Material((name + "-neg").c_str());
	if (direction == 'x') pMaterialNeg->pKa = new VecMat::Vec3(0.0f, 0.0f, 1.0f);
	if (direction == 'y') pMaterialNeg->pKa = new VecMat::Vec3(0.0f, 1.0f, 0.0f);
	if (direction == 'z') pMaterialNeg->pKa = new VecMat::Vec3(0.0f, 0.0f, 0.0f);
	pMaterialEntryList[1] = new MaterialEntry();
	pMaterialEntryList[1]->pMaterial = pMaterialNeg;
	pMaterialEntryList[1]->start = 12;
}

/*********************************************************************
* Destructor
*********************************************************************/
Axis::~Axis(void)
{
}

