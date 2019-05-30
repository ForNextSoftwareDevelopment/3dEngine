#include "Mesh.h"

/*********************************************************************
* Default Constructor
*********************************************************************/
Mesh::Mesh (void)
{
    // Set default name of object
    this->name = "Mesh";

    // Set number of triangulation errors (while importing this object) to 0
    numTriangulationErrors = 0;

    // Set 'no mapping provided' as default
    mapped = false;

    // Set number of vertices, texture vertices, faces and materials
    numVertices        = 0;
    numVertexNormals   = 0;
    numFaceNormals     = 0;
    numTangents        = 0;
    numTextureVertices = 0;
    numFaces           = 0;
    numMaterials       = 0;

    // Set pointers to the vertices and faces
    pVertices         = NULL;
    pVertexNormals    = NULL;
    pFaceNormals      = NULL;
    pTangents         = NULL;
    pBiTangents       = NULL;
    pTextureVertices  = NULL;
    pFaces            = NULL;

    // Set pointers to the array of material entries for this object
    for (int i=0; i<MAX_NUM_MATERIALS; i++) pMaterialEntryList[i] = NULL;
}

/*********************************************************************
* Constructor
*********************************************************************/
Mesh::Mesh(std::string name)
{
    // Set default name of object
    this->name = name;

    // Set number of triangulation errors (while importing this object) to 0
    numTriangulationErrors = 0;

    // Set 'no mapping provided' as default
    mapped = false;

    // Set number of vertices, texture vertices, faces and materials
    numVertices        = 0;
    numVertexNormals   = 0;
    numFaceNormals     = 0;
    numTangents        = 0;
    numTextureVertices = 0;
    numFaces           = 0;
    numMaterials       = 0;

    // Set pointers to the vertices and faces
    pVertices         = NULL;
    pVertexNormals    = NULL;
    pFaceNormals      = NULL;
    pTangents         = NULL;
    pBiTangents       = NULL;
    pTextureVertices  = NULL;
    pFaces            = NULL;

    // Set pointers to the array of material entries for this object
    for (int i=0; i<MAX_NUM_MATERIALS; i++) pMaterialEntryList[i] = NULL;
}

/*********************************************************************
* Destructor
*********************************************************************/
Mesh::~Mesh (void)
{
    if (pVertices        != NULL) free (pVertices);
    if (pVertexNormals   != NULL) free (pVertexNormals);
    if (pFaceNormals     != NULL) free (pFaceNormals);
    if (pTangents        != NULL) free (pTangents);
    if (pBiTangents      != NULL) free (pBiTangents);
    if (pTextureVertices != NULL) free (pTextureVertices);
    if (pFaces           != NULL) free (pFaces);

    for (int i=0; i<MAX_NUM_MATERIALS; i++)
    {
        if (pMaterialEntryList[i] != NULL) delete pMaterialEntryList[i];
    }
}

