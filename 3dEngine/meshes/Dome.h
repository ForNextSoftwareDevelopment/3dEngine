#pragma once
#include <cmath>
#include "../Mesh.h"

class Dome: public Mesh
{
	private:

        // Pointers to the vertices and faces (copies to work with)
        VecMat::Vertex *pV;
        VecMat::Face   *pF;

	public:

		// Constructor
		Dome(GLfloat radius, GLint resolution, GLuint numSegments);

		// Destructor
		~Dome(void);
};

