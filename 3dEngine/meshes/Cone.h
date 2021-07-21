#pragma once
#include <cmath>
#include "../Mesh.h"

class Cone: public Mesh
{
    private:

    public:

        // Constructor
        Cone(GLfloat radius, GLint resolution, GLuint numSegments);

        // Destructor
        ~Cone(void);
};

