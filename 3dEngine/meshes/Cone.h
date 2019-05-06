#pragma once
#include <cmath>
#include "../Mesh.h"

class Cone: public Mesh
{
	private:

        // Pointers to the vertices and faces (copies to work with)
        VecMat::Vertex *pV;
        VecMat::Face   *pF;

	public:

		// Constructor
		Cone(GLfloat radius, GLint resolution, GLuint numSegments);

		// Destructor
		~Cone(void);
};

