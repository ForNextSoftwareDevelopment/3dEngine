#include "Tube.h"

/*********************************************************************
* Constructor
*********************************************************************/
Tube::Tube(GLfloat radius, GLint resolution, GLuint numSegments)
{
    // Coordinates for vertices
    GLfloat x, y, z;

    // Temp value (argument)
    GLfloat arg;

    // Number of vertices in the Tube
    numVertices = resolution * 4 * numSegments;

    // Allocate memory for the Tube vertices
    pVertices = (VecMat::Vertex*) malloc (numVertices * sizeof(VecMat::Vertex));

    // Fill this memory with 0's
    ZeroMemory(pVertices, numVertices * sizeof(VecMat::Vertex));

    // Copy vertices pointer to work with
    pV = pVertices;

    // Number of faces in the Tube (triangles, not squares)
    numFaces = resolution * 8 * numSegments;

    // Allocate memory for the Tube faces
    pFaces = (VecMat::Face*) malloc (numFaces * sizeof(VecMat::Face));

    // Fill this memory with 0's
    ZeroMemory(pFaces, numFaces * sizeof(VecMat::Face));

    // Copy faces pointer to work with
    pF = pFaces;

    // Fill all data for vertices, edges and faces for all segments
    for (GLuint curSegment = 0; curSegment < numSegments; curSegment++)
    {
        // Set y-coordinate for this segment (vertical position)
        y = (GLfloat) curSegment * 0.1f;

        // Set vertices for the segment (one side of the segment, positive z)
        for (GLint curVertex = -resolution; curVertex < resolution; curVertex++)
        {
            // x-coordinate
            x = ((float)curVertex / (float)resolution) * radius;
            if (x < -1.0f) x = -1.0f;
            if (x > 1.0f) x = 1.0f;

            // z-coordinate
            arg = (radius*radius) - (x * x);
            if (arg < 0) arg = -arg;
            z = sqrt(arg);
            if (z > 1.0f) z = 1.0f;

            // Set positions
            pV->x = x;
            pV->y = y;
            pV->z = z;
            pV++;
        };

        // Set vertices for the segment (other side of the segment, negative z)
        for (GLint curVertex = resolution; curVertex > -resolution; curVertex--)
        {
            // x-coordinate
            x = ((float)curVertex / (float)resolution) * radius;
            if (x < -1.0f) x = -1.0f;
            if (x > 1.0f) x = 1.0f;

            // z-coordinate
            arg = (radius*radius) - (x * x);
            if (arg < 0) arg = -arg;
            z = - sqrt(arg);
            if (z < -1.0f) z = -1.0f;

            // Set positions
            pV->x = x;
            pV->y = y;
            pV->z = z;
            pV++;
        };

        // Set faces for the sphere (from current segment to previous segment)
        if (curSegment > 0)
        {
            for (GLint curVertex = 0; curVertex < (resolution * 4) - 1; curVertex++)
            {
                pF->first = curSegment * (resolution * 4) + curVertex + 1;
                pF->second = curSegment * (resolution * 4) + curVertex;
                pF->third = (curSegment - 1) * (resolution * 4) + curVertex;
                pF++;
                pF->first = (curSegment - 1) * (resolution * 4) + curVertex;
                pF->second = (curSegment - 1) * (resolution * 4) + curVertex + 1;
                pF->third = curSegment * (resolution * 4) + curVertex + 1;
                pF++;
            };

            // Set the last face from the end- to the starting vertices
            pF->first = curSegment * (resolution * 4);
            pF->second = (curSegment + 1) * (resolution * 4) - 1;
            pF->third = curSegment * (resolution * 4) -1;
            pF++;
            pF->first = curSegment * (resolution * 4);
            pF->second = (curSegment - 1) * (resolution * 4);
            pF->third = curSegment * (resolution * 4) -1;
            pF++;
        };
    };

};

/*********************************************************************
* Destructor
*********************************************************************/
Tube::~Tube(void)
{
    if (pVertices != NULL) free (pVertices);
    if (pFaces    != NULL) free (pFaces);
};

