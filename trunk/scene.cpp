/**
* File : scene.cpp
* Description : The scene stores the objects to be rendered on the screen.
*				  
* Author(s) : ALucchi
* Date of creation : 21/01/2008
* Modification(s) :
*/

#include "ase.h"
#include "defs.h"
#include "scene.h"

Scene::Scene()
{	
}

Scene::~Scene()
{
	if(m_box)
		delete m_box;

	// TODO : Delete all the elements...
	if(m_Grid)
		delete[] m_Grid;
}

/**
 * Build the 3d grid which contains cells referencing the objects that
 * intersect with each cell.
 * @TODO : see optimization on http://www.devmaster.net/articles/raytracing_series/part4.php
 */
void Scene::BuildGrid()
{
	// C can be changed to define the horizon of a plan added in the list of objects
	#define C 100
	const Vector3 start(-C,-C,-C);
	const Vector3 end(C,C,C);
	m_box=new Box(start,end);
	// TODO : change / operator for Vector3
	Vector3 size = (end - start) * (1.0f / GRIDSIZE);	
	
	int nbObjects = GRIDSIZE*GRIDSIZE*GRIDSIZE;
	m_Grid=new ObjectList*[nbObjects];
	
	// delta values
	float dx = (end.x - start.x) / GRIDSIZE;
	float dy = (end.y - start.y) / GRIDSIZE;
	float dz = (end.z - start.z) / GRIDSIZE;	
		
	for (int gZ = 0; gZ < GRIDSIZE; gZ++)	
		for (int gY = 0; gY < GRIDSIZE; gY++)
			for (int gX = 0; gX < GRIDSIZE; gX++)
			{
				int indexGrid = gX + gY * GRIDSIZE + gZ * GRIDSIZE *GRIDSIZE;
				m_Grid[indexGrid] = 0;
				//const Vector3 pos(gX,gY,gZ);
				Vector3 pos(start.x + gX * dx, start.y + gY * dy, start.z + gZ * dz);				
				
				list<RTObject*>::iterator iObjects;
				for( iObjects = lObjects.begin(); iObjects != lObjects.end(); iObjects++ )
				{
					if((*iObjects)->IntersectBoundingBox(pos, pos+size))
					{
						#ifdef DEBUG
							cout << "Bounding box intersected : ";
							cout << (*iObjects)->GetType() << " ";
							cout << (*iObjects)->GetPosition();
							cout << " " << gX << " " << gY << " " << gZ << "\n";
						#endif
						// Store a reference to the object in the list
						ObjectList* l = new ObjectList();
						l->SetRTObject(*iObjects);
						l->SetNext(m_Grid[indexGrid]);
						m_Grid[indexGrid] = l;
					}			
				}
			}
}

/**
 *  Add an object to the scene
 */
void Scene::AddObject(RTObject* o)
{
	lObjects.push_front(o);
}

/**
 * Imports the ASE model contained in the file whose name is specified in
 * strFileName
 * @param strFileName file name that contains the ASE model to be imported
 */
void Scene::ImportASE(char *strFileName)
{
	CLoadASE loadASE;
	t3DModel model;
	loadASE.ImportASE(&model, strFileName);
	
	// check validity
	if(model.pObject.size() <= 0) return;
	
	// TODO : replace by iterator
	// For each object
	int i=0;
	std::vector<t3DObject>::iterator itObject;
	//for(int i = 0; i < model.numOfObjects; i++)
	for(itObject = model.pObject.begin(); itObject != model.pObject.end(); itObject++) 	    
    {
        //t3DObject *pObj = &pObject[i];
        t3DObject *pObj = &(*itObject);  
            
        // TODO : textures are not handled at the moment...
		// Don't need to load pTexVerts
        //if(pObj->bHasTexture)

		float* fColor = model.pMaterials[pObj->materialID].fColor;
		Vector3 objColor(fColor[0],fColor[1],fColor[2]);

		#ifdef DEBUG
			cout << "Object color : " << objColor << "\n";
		#endif

        // Go through all of the faces (polygons) of the object and draw them
        for(int j = 0; j < pObj->numOfFaces; j++)
        {
#ifdef VERTEX_NORMAL        	
        	Vector3 vNormals[3];
#endif
        	Vector3 vectors[3];
            // Go through each corner of the triangle and draw it.
            for(int whichVertex = 0; whichVertex < 3; whichVertex++)
            {
                // Get the vertex index for each point of the face
                int vertIndex = pObj->pFaces[j].vertIndex[whichVertex];
        
                // Pass in the current vertex of the object (Corner of current face)
				vectors[whichVertex] = Vector3(pObj->pVerts[vertIndex].x,
					pObj->pVerts[vertIndex].y,
					pObj->pVerts[vertIndex].z + 2.0f);
				
#ifdef VERTEX_NORMAL        	
				vNormals[whichVertex] = Vector3(pObj->pNormals[vertIndex].x,
					pObj->pNormals[vertIndex].y,
					pObj->pNormals[vertIndex].z);
#endif				
            }
			Triangle* t=new Triangle(vectors[0], vectors[1], vectors[2]);
			
#ifdef VERTEX_NORMAL
			t->SetVertexNormals(vNormals);
#endif			
			
			// TODO : should use the material from the ASE model
			t->GetMaterial()->SetColor(objColor);
			t->GetMaterial()->SetRefraction(0.0f);
			t->GetMaterial()->SetDiffuse(0.8f);
			AddObject(t);     
        }
        i++;
	}
}
