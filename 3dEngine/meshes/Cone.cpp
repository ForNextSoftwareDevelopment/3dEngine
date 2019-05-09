#include "Cone.h"

/*********************************************************************
* Constructor
*********************************************************************/
Cone::Cone(GLfloat radius, GLint resolution, GLuint numSegments)
{
    // Coordinates for vertices
    GLfloat x=0.0f, y=0.0f, z=0.0f;

    // Radius to work with
    GLfloat curRadius;

    // Temp value (argument)
    GLfloat arg;

    // Number of vertices in the Cone
    numVertices = resolution * 4 * numSegments;

    // Allocate memory for the Cone vertices
    pVertices = (VecMat::Vertex*) malloc (numVertices * sizeof(VecMat::Vertex));

    // Fill this memory with 0's
    ZeroMemory(pVertices, numVertices * sizeof(VecMat::Vertex));

    // Copy vertices pointer to work with
    pV = pVertices;

    // Number of faces in the Cone (triangles, not squares)
    numFaces = resolution * 8 * numSegments;

    // Allocate memory for the Cone faces
    pFaces = (VecMat::Face*) malloc (numFaces * sizeof(VecMat::Face));

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
        curRadius = radius - y;

        // Set vertices for the segment (one side of the segment, positive z)
        for (GLint curVertex = -resolution; curVertex < resolution; curVertex++)
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
        };

        // Set faces for the Cone (from current segment to previous segment)
        if (curSegment > 0)
        {
            for (int curVertex = 0; curVertex < (resolution * 4) - 1; curVertex++)
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
Cone::~Cone(void)
{
    if (pVertices != NULL) free (pVertices);
    if (pFaces    != NULL) free (pFaces);
};

