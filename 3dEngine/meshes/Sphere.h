#pragma once
#include <cmath>
#include "../Mesh.h"

class Sphere: public Mesh
{
    private:

        // Pointers to the vertices and faces (copies to work with)
        VecMat::Vertex *pV;
        VecMat::Face   *pF;

    public:

        // Constructor
        Sphere(GLfloat radius, GLint resolution, GLuint numSegments);

        // Destructor
        ~Sphere(void);
};

