#pragma once
#include <cmath>
#include "../Mesh.h"

class Sphere: public Mesh
{
    private:

    public:

        // Constructor
        Sphere(GLfloat radius, GLint resolution, GLuint numSegments);

        // Destructor
        ~Sphere(void);
};

