/**
* File : rayTracer.h
* Description : Simple raytracer that fires rays through the pixels in the
* screen and recursively spawns new rays upon reflection or refraction
* Links : http://www.devmaster.net/articles/raytracing_series/part1.php
*				  
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef RAYTRACER_H
#define RAYTRACER_H

//-------------------------------------------------------------------- INCLUDES
#include "rtObjects.h"
#include "Maths/math3D.h"

#include <iostream>
#include <list>
using namespace std;

//---------------------------------------------------------------------- CONSTS

#define MAX_RAYTRACE_DEPTH 4

//----------------------------------------------------------------------- CLASS

class RayTracer
{
private:
	list<RTObject*> lObjects;

	// renderer data
	float m_WX1, m_WY1, m_WX2, m_WY2, m_DX, m_DY, m_SX, m_SY;

public:
	void Init();
	void Render();

	float GetDistance(const Ray& r, RTObject*& nearestO, RTObject* origin=0);
	Color RayTrace(const Ray& ray, int depth, float rIndex, RTObject* origin=0);
	void AddObject(RTObject* o);
};

#endif // RAYTRACER_H
