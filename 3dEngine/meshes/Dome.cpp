#include "Dome.h"

/*********************************************************************
* Constructor
*********************************************************************/
Dome::Dome(GLfloat radius, GLint resolution, GLuint numSegments)
{
    // Pointers to the (texture) vertices and faces (copies to work with)
    VecMat::Vertex *pV;
    VecMat::Vertex *pTV;
    VecMat::Face   *pF;

    // Coordinates for vertices
    GLfloat x=0.0f, y=0.0f, z=0.0f;

    // Radius to work with
    GLfloat curRadius;

    // Temp value (argument)
    GLfloat arg;

    // Number of vertices in the Dome
    numVertices = resolution * 4 * numSegments;

    // Allocate memory for the Dome vertices
    pVertices = new VecMat::Vertex[numVertices];

    // Fill this memory with 0's
    ZeroMemory(pVertices, numVertices * sizeof(VecMat::Vertex));

    // Copy vertices pointer to work with
    pV = pVertices;

    // Number of texture vertices in the Sphere
    numTextureVertices = resolution * 4 * numSegments;

    // Allocate memory for the Sphere vertices
    pTextureVertices = new VecMat::Vertex[numTextureVertices];

    // Fill this memory with 0's
    ZeroMemory(pTextureVertices, numTextureVertices * sizeof(VecMat::Vertex));

    // Copy vertices pointer to work with
    pTV = pTextureVertices;

    // Number of faces in the Dome (triangles, not squares)
    numFaces = resolution * 8 * numSegments;

    // Allocate memory for the Dome faces
    pFaces = new VecMat::Face[numFaces];

    // Fill this memory with 0's
    ZeroMemory(pFaces, numFaces * sizeof(VecMat::Face));

    // Copy faces pointer to work with
    pF = pFaces;

    // Fill all data for vertices, edges and faces for all segments
    for (GLuint curSegment = 0; curSegment < numSegments; curSegment++)
    {
        // Set y-coordinate for this segment (vertical position)
        y = (GLfloat)(curSegment+1) / (GLfloat)numSegments * radius;

        // Set radius for this segment
        curRadius = sqrt((radius*radius) - (y*y));

        // Set vertices for the segment (one side of the segment, positive z)
        for (int curVertex = -resolution; curVertex < (int) resolution; curVertex++)
        {
            // x-coordinate
            x = ((GLfloat)curVertex / (GLfloat)resolution * curRadius);

            // z-coordinate
            arg = (curRadius * curRadius) - (x * x);
            if (arg >= 0) z = sqrt(arg); else z = 0.0f;

            // Set positions
            pV->x = x;
            pV->y = y;
            pV->z = z;
            pV++;

            // Set texture positions
            pTV->x = x / radius;
            pTV->y = y / radius;
            pTV->z = 0;
            pTV++;
        };

        // Set vertices for the segment (other side of the segment, negative z)
        for (GLint curVertex = resolution; curVertex > -resolution; curVertex--)
        {
            // x-coordinate
            x = ((GLfloat)curVertex / (GLfloat)resolution * curRadius);

            // z-coordinate
            arg = (curRadius * curRadius) - (x * x);
            if (arg > 0) z = - sqrt(arg); else z = 0.0f;

            // Set positions
            pV->x = x;
            pV->y = y;
            pV->z = z;
            pV++;

            // Set texture positions
            pTV->x = x / radius;
            pTV->y = y / radius;
            pTV->z = 0;
            pTV++;
        };

        // Set faces for the Dome (from current segment to previous segment)
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
            pF->second = curSegment * (resolution * 4) - 1;
            pF->third = (curSegment - 1) * (resolution * 4);
            pF++;
        };
    };

};

/*********************************************************************
* Destructor
*********************************************************************/
Dome::~Dome(void)
{
    if (        pVertices != nullptr) delete[] (pVertices);
    if ( pTextureVertices != nullptr) delete[] (pTextureVertices);
    if (        pFaces    != nullptr) delete[] (pFaces);
};

