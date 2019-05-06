#include "Cube.h"

/*********************************************************************
* Constructor
*********************************************************************/
Cube::Cube(GLfloat size)
{
	// Number of vertices in a Cube is 8
	numVertices = 8;

	// Number of faces in a Cube is 12 (triangles, not squares)
	numFaces = 12;

	// Fill vertices for the Cube
	vertices[0].x = -size/2;
	vertices[0].y = -size/2;
	vertices[0].z = size/2;

	vertices[1].x = -size/2;
	vertices[1].y = size/2;
	vertices[1].z = size/2;

	vertices[2].x = size/2;
	vertices[2].y = size/2;
	vertices[2].z = size/2;

	vertices[3].x = size/2;
	vertices[3].y = -size/2;
	vertices[3].z = size/2;

	vertices[4].x = -size/2;
	vertices[4].y = -size/2;
	vertices[4].z = -size/2;

	vertices[5].x = -size/2;
	vertices[5].y = size/2;
	vertices[5].z = -size/2;

	vertices[6].x = size/2;
	vertices[6].y = size/2;
	vertices[6].z = -size/2;

	vertices[7].x = size/2;
	vertices[7].y = -size/2;
	vertices[7].z = -size/2;

	// Set vertices pointer to the array of vertices just filled
	pVertices = vertices;

	// Fill faces for the Cube (6 squares containing each 2 triangle vertices, making 12 faces)
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

	// Set faces pointer to the array of faces just filled
	pFaces = faces;
};

/*********************************************************************
* Destructor
*********************************************************************/
Cube::~Cube(void)
{
};

