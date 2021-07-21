#pragma once
#include <windows.h>
#include <string>
#include <cctype>
#include <vector>
#include "GL/glew.h"
#include "Error.h"
#include "Utils.h"
#include "VecMat.h"
#include "Material.h"

// Maximum number of meshes to render
#define MAX_NUM_MESHES 10000

// Maximum number of materials to use
#define MAX_NUM_MATERIALS 100

// The structure for our material to use on this object
typedef struct MaterialEntry
{
    // Material to use on this object
    Material *pMaterial;

    // Starting face to use this material on
    unsigned int start;

    // Constructor
    MaterialEntry()
    {
        pMaterial = NULL;
        start = 0;
    }
} MaterialEntry;

class Mesh
{
    private:

    public:

        // Name of object
        std::string name;

        // Position of object
        GLfloat pos_x, pos_y, pos_z;

        // Center of object
        GLfloat center_x, center_y, center_z;

        // Angle of object
        GLfloat angle_x, angle_y, angle_z;

        // (Maximum) size of object
        GLfloat size_x, size_y, size_z;

        // Speed of object
        GLfloat speed_angle, speed_pos;

        // Number of triangulation errors in this object
        unsigned int numTriangulationErrors;

        // Number of materials in this object
        GLuint numMaterials;

        // Material list to use on this object
        MaterialEntry *pMaterialEntryList[MAX_NUM_MATERIALS];

        // Number of vertices in this object
        GLuint numVertices;

        // Pointer to vertices to define the object
        VecMat::Vertex *pVertices;

        // Number of face-normals in this object
        GLuint numFaceNormals;

        // Pointer to normals of the faces
        VecMat::Vertex *pFaceNormals;

        // Number of vertex-normals in this object
        GLuint numVertexNormals;

        // Pointer to normals of the vertices
        VecMat::Vertex *pVertexNormals;

        // Number of tangents in this object
        GLuint numTangents;

        // Pointer to tangents of the vertices
        VecMat::Vertex *pTangents;

        // Number of bitangents in this object
        GLuint numBiTangents;

        // Pointer to bi-tangents of the vertices
        VecMat::Vertex *pBiTangents;

        // Number of (bump) texture vertices in this object
        GLuint numTextureVertices;

        // Pointer to texture vertices to define the object
        VecMat::Vertex *pTextureVertices;

        // Number of faces in this object
        GLuint numFaces;

        // Pointer to faces to define the object
        VecMat::Face *pFaces;

        // Default Constructor
        Mesh (void);

        // Constructor
        Mesh (std::string name);

        // Destructor
        ~Mesh (void);
};

