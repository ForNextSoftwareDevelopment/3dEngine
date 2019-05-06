#pragma once
#include "../Mesh.h"

class Cube: public Mesh
{
	private:

		// Vertices to form a Cube
		VecMat::Vertex vertices[8];

		// Faces to form a Cube (triangles, not squares)
		VecMat::Face faces[12];

	public:

		// Constructor
		Cube(GLfloat size);

		// Destructor
		~Cube(void);
};

