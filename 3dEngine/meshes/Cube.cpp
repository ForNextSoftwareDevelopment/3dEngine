#include "Cube.h"

/*********************************************************************
* Constructor
*********************************************************************/
Cube::Cube(GLfloat size)
{
    float halfSize = size / 2.0f;

    // Number of vertices in a Cube is 24 (all sides have seperate vertices)
    numVertices = 24;
       
    // Number of texture vertices
    numTextureVertices = 24;

    // Number of faces in a Cube is 12 (triangles, not squares)
    numFaces = 12;

    // Fill vertices for the Cube
    // FrontFace 
    vertices[ 0].x = -halfSize;
    vertices[ 0].y = -halfSize;
    vertices[ 0].z = halfSize;

    vertices[ 1].x = -halfSize;
    vertices[ 1].y = halfSize;
    vertices[ 1].z = halfSize;

    vertices[ 2].x = halfSize;
    vertices[ 2].y = halfSize;
    vertices[ 2].z = halfSize;

    vertices[ 3].x = halfSize;
    vertices[ 3].y = -halfSize;
    vertices[ 3].z = halfSize;

    // LeftFace
    vertices[ 4].x = -halfSize;
    vertices[ 4].y = -halfSize;
    vertices[ 4].z = halfSize;

    vertices[ 5].x = -halfSize;
    vertices[ 5].y = -halfSize;
    vertices[ 5].z = -halfSize;

    vertices[ 6].x = -halfSize;
    vertices[ 6].y = halfSize;
    vertices[ 6].z = -halfSize;

    vertices[ 7].x = -halfSize;
    vertices[ 7].y = halfSize;
    vertices[ 7].z = halfSize;

    // BackFace
    vertices[ 8].x = -halfSize;
    vertices[ 8].y = -halfSize;
    vertices[ 8].z = -halfSize;

    vertices[ 9].x = halfSize;
    vertices[ 9].y = -halfSize;
    vertices[ 9].z = -halfSize;

    vertices[10].x = halfSize;
    vertices[10].y = halfSize;
    vertices[10].z = -halfSize;

    vertices[11].x = -halfSize;
    vertices[11].y = halfSize;
    vertices[11].z = -halfSize;

    // RightFace
    vertices[12].x = halfSize;
    vertices[12].y = -halfSize;
    vertices[12].z = -halfSize;

    vertices[13].x = halfSize;
    vertices[13].y = -halfSize;
    vertices[13].z = halfSize;

    vertices[14].x = halfSize;
    vertices[14].y = halfSize;
    vertices[14].z = halfSize;

    vertices[15].x = halfSize;
    vertices[15].y = halfSize;
    vertices[15].z = -halfSize;

    // TopFace
    vertices[16].x = -halfSize;
    vertices[16].y = halfSize;
    vertices[16].z = halfSize;

    vertices[17].x = -halfSize;
    vertices[17].y = halfSize;
    vertices[17].z = -halfSize;

    vertices[18].x = halfSize;
    vertices[18].y = halfSize;
    vertices[18].z = -halfSize;

    vertices[19].x = halfSize;
    vertices[19].y = halfSize;
    vertices[19].z = halfSize;

    // BottomFace
    vertices[20].x = -halfSize;
    vertices[20].y = -halfSize;
    vertices[20].z = halfSize;

    vertices[21].x = halfSize;
    vertices[21].y = -halfSize;
    vertices[21].z = halfSize;

    vertices[22].x = halfSize;
    vertices[22].y = -halfSize;
    vertices[22].z = -halfSize;

    vertices[23].x = -halfSize;
    vertices[23].y = -halfSize;
    vertices[23].z = -halfSize;

    // Set vertices pointer to the array of vertices just filled
    pVertices = vertices;

    // Fill texture vertices for the Cube
    // FrontFace 
    textureVertices[ 0].x = 0.0f;
    textureVertices[ 0].y = 0.0f;
    textureVertices[ 0].z = 0.0f;

    textureVertices[ 1].x = 0.0f;
    textureVertices[ 1].y = 1.0f;
    textureVertices[ 1].z = 0.0f;

    textureVertices[ 2].x = 1.0f;
    textureVertices[ 2].y = 1.0f;
    textureVertices[ 2].z = 0.0f;

    textureVertices[ 3].x = 1.0f;
    textureVertices[ 3].y = 0.0f;
    textureVertices[ 3].z = 0.0f;

    // LeftFace
    textureVertices[ 4].x = 0.0f;
    textureVertices[ 4].y = 0.0f;
    textureVertices[ 4].z = 0.0f;

    textureVertices[ 5].x = 1.0f;
    textureVertices[ 5].y = 0.0f;
    textureVertices[ 5].z = 0.0f;

    textureVertices[ 6].x = 1.0f;
    textureVertices[ 6].y = 1.0f;
    textureVertices[ 6].z = 0.0f;

    textureVertices[ 7].x = 0.0f;
    textureVertices[ 7].y = 1.0f;
    textureVertices[ 7].z = 0.0f;

    // BackFace
    textureVertices[ 8].x = 0.0f;
    textureVertices[ 8].y = 1.0f;
    textureVertices[ 8].z = 0.0f;

    textureVertices[ 9].x = 1.0f;
    textureVertices[ 9].y = 1.0f;
    textureVertices[ 9].z = 0.0f;

    textureVertices[10].x = 1.0f;
    textureVertices[10].y = 0.0f;
    textureVertices[10].z = 0.0f;

    textureVertices[11].x = 0.0f;
    textureVertices[11].y = 0.0f;
    textureVertices[11].z = 0.0f;

    // RightFace
    textureVertices[12].x = 0.0f;
    textureVertices[12].y = 1.0f;
    textureVertices[12].z = 0.0f;

    textureVertices[13].x = 1.0f;
    textureVertices[13].y = 1.0f;
    textureVertices[13].z = 0.0f;

    textureVertices[14].x = 1.0f;
    textureVertices[14].y = 0.0f;
    textureVertices[14].z = 0.0f;

    textureVertices[15].x = 0.0f;
    textureVertices[15].y = 0.0f;
    textureVertices[15].z = 0.0f;

    // TopFace
    textureVertices[16].x = 1.0f;
    textureVertices[16].y = 1.0f;
    textureVertices[16].z = 0.0f;

    textureVertices[17].x = 1.0f;
    textureVertices[17].y = 0.0f;
    textureVertices[17].z = 0.0f;

    textureVertices[18].x = 0.0f;
    textureVertices[18].y = 0.0f;
    textureVertices[18].z = 0.0f;

    textureVertices[19].x = 0.0f;
    textureVertices[19].y = 1.0f;
    textureVertices[19].z = 0.0f;

    // BottomFace
    textureVertices[20].x = 0.0f;
    textureVertices[20].y = 0.0f;
    textureVertices[20].z = 0.0f;

    textureVertices[21].x = 1.0f;
    textureVertices[21].y = 0.0f;
    textureVertices[21].z = 0.0f;

    textureVertices[22].x = 1.0f;
    textureVertices[22].y = 1.0f;
    textureVertices[22].z = 0.0f;

    textureVertices[23].x = 0.0f;
    textureVertices[23].y = 1.0f;
    textureVertices[23].z = 0.0f;

    // Set texture vertices pointer to the array of vertices just filled
    pTextureVertices = textureVertices;

    // Fill faces for the Cube (6 squares containing each 2 triangle vertices, making 12 faces)
    // FrontFace 
    faces[ 0].first  =  0;
    faces[ 0].second =  2;
    faces[ 0].third  =  1;

    faces[ 1].first  =  0;
    faces[ 1].second =  3;
    faces[ 1].third  =  2;

    // LeftFace
    faces[ 2].first  =  4;
    faces[ 2].second =  6;
    faces[ 2].third  =  5;

    faces[ 3].first  =  4;
    faces[ 3].second =  7;
    faces[ 3].third  =  6;

    // BackFace
    faces[ 4].first  =  8;
    faces[ 4].second = 10;
    faces[ 4].third  =  9;

    faces[ 5].first  =  8;
    faces[ 5].second = 11;
    faces[ 5].third  = 10;

    // RightFace
    faces[ 6].first  = 12;
    faces[ 6].second = 14;
    faces[ 6].third  = 13;

    faces[ 7].first  = 12;
    faces[ 7].second = 15;
    faces[ 7].third  = 14;

    // TopFace
    faces[ 8].first  = 16;
    faces[ 8].second = 18;
    faces[ 8].third  = 17;

    faces[ 9].first  = 16;
    faces[ 9].second = 19;
    faces[ 9].third  = 18;

    // BottomFace
    faces[10].first  = 20;
    faces[10].second = 22;
    faces[10].third  = 21;

    faces[11].first  = 20;
    faces[11].second = 23;
    faces[11].third  = 22;

    // Set faces pointer to the array of faces just filled
    pFaces = faces;
};

/*********************************************************************
* Destructor
*********************************************************************/
Cube::~Cube(void)
{
};

