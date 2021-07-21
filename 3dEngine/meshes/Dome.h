#pragma once
#include <cmath>
#include "../Mesh.h"

class Dome: public Mesh
{
    private:

    public:

        // Constructor
        Dome(GLfloat radius, GLint resolution, GLuint numSegments);

        // Destructor
        ~Dome(void);
};

