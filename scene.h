/**
* File : scene.h
* Description : The scene stores the objects to be rendered on the screen.
*				  
* Author(s) : ALucchi
* Date of creation : 21/01/2008
* Modification(s) :
*/

#ifndef SCENE_H
#define SCENE_H

//-------------------------------------------------------------------- INCLUDES

#include "rtObjects.h"

#include <iostream>
#include <list>
using namespace std;

//----------------------------------------------------------------------- CLASS

// ----------------------------------------------------------------------------
// Object list helper class (similar to the std:multimap class).
// The order of insertion is maintained
// ----------------------------------------------------------------------------

class ObjectList
{
public:
	ObjectList() : m_RTObject( 0 ), m_Next( 0 ) {}
	~ObjectList() { delete m_Next; }
	void SetRTObject( RTObject* a_obj ) { m_RTObject = a_obj; }
	RTObject* GetRTObject() { return m_RTObject; }
	void SetNext( ObjectList* a_Next ) { m_Next = a_Next; }
	ObjectList* GetNext() { return m_Next; }
private:
	RTObject* m_RTObject;
	ObjectList* m_Next;
};

// ----------------------------------------------------------------------------
// Scene class
// ----------------------------------------------------------------------------

class Scene
{
public:
	Scene();
	virtual ~Scene();
	void BuildGrid();
	
	void AddObject(RTObject* o);	
	Box& GetBox() {return *m_box;}
	ObjectList** GetGrid() {return m_Grid;}
	list<RTObject*>& GetObjects() {return lObjects;}
	void ImportASE(char *strFileName);
	
private:
	// list of the objects that belong to the scene
	list<RTObject*> lObjects;
	// Structure used for the spatial division
	ObjectList** m_Grid;
	// bounding box surrounding the scene
	Box* m_box;	
};

#endif // SCENE_H
