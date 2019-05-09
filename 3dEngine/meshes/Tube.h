#pragma once
#include <cmath>
#include "../Mesh.h"

class Tube: public Mesh
{
    private:

        // Pointers to the vertices and faces (copies to work with)
        VecMat::Vertex *pV;
        VecMat::Face   *pF;

    public:

        // Constructor
        Tube(GLfloat radius, GLint resolution, GLuint numSegments);

        // Destructor
        ~Tube(void);
};

