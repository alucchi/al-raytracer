/**
* File : ase.cpp
* Description : Import of 3ds models.
*				  
* Author(s) : This file has been downloaded from www.GameTutorials.com
* Date of creation : 21/02/2008
* Modification(s) :
*/

#ifndef _ASE_H
#define _ASE_H

#include "Maths/Vector2.h"
#include "Maths/Vector3.h"

#include <GL/gl.h>                                      // Header File For The OpenGL32 Library
#include <GL/glu.h>                                     // Header File For The GLu32 Library

#include <string>
#include <vector>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

using namespace std;

typedef unsigned char BYTE;

// Below are a all the important tag defines for reading in a .Ase file.
// I added the normal ones just in case you want to you read them in instead
// of calculate them yourself.

#define OBJECT              "*GEOMOBJECT"           // An object tag for new objects
#define NUM_VERTEX          "*MESH_NUMVERTEX"       // The number of vertices tag
#define NUM_FACES           "*MESH_NUMFACES"        // The number of faces tag
#define NUM_TVERTEX         "*MESH_NUMTVERTEX"      // The number of texture coordinates
#define VERTEX              "*MESH_VERTEX"          // The list of vertices tag
#define FACE                "*MESH_FACE"            // The list of faces tag
#define NORMALS             "*MESH_NORMALS"         // The list of normals tag (If you want)
#define FACE_NORMAL         "*MESH_FACENORMAL"      // The face normal for the current index
#define NVERTEX             "*MESH_VERTEXNORMAL"    // The list of vertex normals
#define TVERTEX             "*MESH_TVERT"           // The texture coordinate index tag
#define TFACE               "*MESH_TFACE"           // The vertex index tag
#define TEXTURE             "*BITMAP"               // The file name for the object's texture map
#define UTILE               "*UVW_U_TILING"         // The U tiling ratio tag
#define VTILE               "*UVW_V_TILING"         // The V tiling ratio tag
#define UOFFSET             "*UVW_U_OFFSET"         // The U tile offset tag
#define VOFFSET             "*UVW_V_OFFSET"         // The V tile offset tag
#define MATERIAL_ID         "*MATERIAL_REF"         // The material ID tag
#define MATERIAL_COUNT      "*MATERIAL_COUNT"       // The material count tag
#define MATERIAL            "*MATERIAL"             // The material tag
#define MATERIAL_NAME       "*MATERIAL_NAME"        // The material name tag
#define MATERIAL_COLOR      "*MATERIAL_DIFFUSE"     // The material color tag

// This file includes all of the model structures that are needed to load
// in a .ASE file.  If you intend to do animation you will need to add on
// to this.  These structures only support the information that is needed
// to load the objects in the scene and their associative materials.

#define MAX_TEXTURES 100                                // The maximum amount of textures to load

void CreateTexture(unsigned int textureArray[],char *strFileName,int textureID);

// This is our face structure.  This is is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.
struct tFace
{
    int vertIndex[3];           // indicies for the verts that make up this triangle
    int coordIndex[3];          // indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
// Some of these are not used, but I left them because you will want to eventually
// read in the UV tile ratio and the UV tile offset for some models.
struct tMaterialInfo
{
    char  strName[255];         // The texture name
    char  strFile[255];         // The texture file name (If this is set it's a texture map)
    BYTE  color[3];             // The color of the object (R, G, B) from 0 to 255
    float fColor[3];            // The color of the object (R, G, B) from 0 to 1
    int   texureId;             // the texture ID
    float uTile;                // u tiling of texture  (Currently not used)
    float vTile;                // v tiling of texture  (Currently not used)
    float uOffset;              // u offset of texture  (Currently not used)
    float vOffset;              // v offset of texture  (Currently not used)
} ;

// This holds all the information for our model/scene. 
// You should eventually turn into a robust class that 
// has loading/drawing/querying functions like:
// LoadModel(...); DrawObject(...); DrawModel(...); DestroyModel(...);
struct t3DObject 
{
    int  numOfVerts;            // The number of verts in the model
    int  numOfFaces;            // The number of faces in the model
    int  numTexVertex;          // The number of texture coordinates
    int  materialID;            // The texture ID to use, which is the index into our texture array
    bool bHasTexture;           // This is TRUE if there is a texture map for this object
    char strName[255];          // The name of the object
    Vector3  *pVerts;          // The object's vertices
    Vector3  *pNormals;        // The object's normals
    Vector2  *pTexVerts;       // The texture's UV coordinates
    tFace *pFaces;              // The faces information of the object
};

// This holds our model information.  This should also turn into a robust class.
// We use STL's (Standard Template Library) vector class to ease our link list burdens. :)
class t3DModel 
{
public:
    int numOfObjects;                   // The number of objects in the model
    int numOfMaterials;                 // The number of materials for the model

    vector<tMaterialInfo> pMaterials;   // The list of material information (Textures and colors)
    vector<t3DObject> pObject;          // The object list for our model

	t3DModel();
	~t3DModel();
};

// This class holds all the data and function for loading in a .Ase file.
class CLoadASE 
{

public:

    // This is the only function the client needs to call to load the .ase file
    bool ImportASE(t3DModel *pModel, char *strFileName);

    // This is the main loop that parses the .ase file
    void ReadAseFile(t3DModel *pModel);

    // This returns the number of objects in the .ase file
    int GetObjectCount();

    // This returns the number of materials in the .ase file
    int GetMaterialCount();

    // This fills in the texture information for a desired texture
    void GetTextureInfo (tMaterialInfo *pTexture, int desiredMaterial);

    // This moves our file pointer to the desired object
    void MoveToObject (int desiredObject);

    // This reads in a float from the file
    float ReadFloat();

    // This reads a desired object's information (face, vertex and texture coord counts)
    void ReadObjectInfo(t3DObject *pObject, int desiredObject);

    // This gets the name of the texture
    void GetTextureName (tMaterialInfo  *pTexture);

    // This gets the name of the material
    void GetMaterialName(tMaterialInfo *pTexture);

    // This loads all the data for the desired object
    void ReadObjectData(t3DModel *pModel, t3DObject *pObject, int desiredObject);

    // This is the main load loop inside of ReadObjectData() that calls smaller load functions
    void GetData(t3DModel *pModel, t3DObject *pObject, char *strDesiredData, int desiredObject);

    // This reads in a vertice from the file
    void ReadVertex(t3DObject *pObject);

    // This reads in a texture coordinate from the file
    void ReadTextureVertex(t3DObject *pObject, tMaterialInfo texture);

    // This reads in the vertex indices for a face
    void ReadFace(t3DObject *pObject);

    // This reads in texture coordinate indices for a face
    void ReadTextureFace(t3DObject *pObject);

    // This computes the vertex normals for our objects
    void ComputeNormals(t3DModel *pModel);

private:
    // This is our file pointer to load the .ase file
    FILE *m_FilePointer;
};

#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file holds the class and defines to load a .Ase file.  The .Ase file is
// created by 3D Studio Max, but isn't as nice as the .3DS file format.
//
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//
 
