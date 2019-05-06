#pragma once
#include "../Mesh.h"

class Axis: public Mesh
{
	private:

		// Vertices to form an Axis
		VecMat::Vertex vertices[16];

		// Faces to form an Axis (triangles, not squares)
		VecMat::Face faces[24];

	public:

		// Constructor
		Axis(GLfloat size, char direction);

		// Destructor
		~Axis(void);
};

