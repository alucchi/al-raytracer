/**
* File : rayTracer.h
* Description : This is the main class for the raytracer.
* It implements the raytracing algorithm that fires rays through the pixels
* in the screen and recursively spawns new rays upon reflection or refraction
* Different options can be enabled using pre-processor flags :
* - SPATIAL_DIVISION : The spatial division algorithm implemented is described
* 	in the following paper : "A faster voxel traversal algorithm for ray tracing"
* 	by John Amanatides and Andrew Woo (can be downloaded from
* 	http://www.devmaster.net/articles/raytracing_series/part4.php)
* - ANTI_ALIASING : The anti-aliasing algorithm is based on the super-sampling
* 	technique : the image is rendered at higher resolutions and an average
* 	color value is calculated.
* 
* Comments : The algorithm used in this class is inspired from :
* http://www.devmaster.net/articles/raytracing_series/part1.php
*				  
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef RAYTRACER_H
#define RAYTRACER_H

//-------------------------------------------------------------------- INCLUDES
#include "rtObjects.h"
#include "scene.h"
#include "Maths/math3D.h"

//---------------------------------------------------------------------- CONSTS

#define MAX_RAYTRACE_DEPTH 3

static Vector3 eye(0,2,-10);

//----------------------------------------------------------------------- CLASS

// ----------------------------------------------------------------------------
// RayTracer class
// ----------------------------------------------------------------------------

class RayTracer
{
private:	

	int m_rayID; // counter to keep track of the current ray ID

	Scene m_Scene;

	// renderer data
	float m_WX1, m_WY1, m_WX2, m_WY2;
	// deltas for interpolation
	float m_DX, m_DY;
	float m_SX, m_SY;
	
	// data for regular grid stepping
	Vector3 m_CS; // cell size
	Vector3 m_RCS; // reverse cell size = 1 / m_CS

	float GetDistance(const Ray& r, RTObject*& nearestO, RTObject* origin=0);
	float FindNearest(const Ray& a_Ray, RTObject*& nearestObj, RTObject* origin);	

public:	

	void AddObject(RTObject* o);
	void ImportASE(char *strFileName);
	void Init();	
	void Render();

	RTObject* RayTrace(const Ray& ray, Color& color, int depth, float rIndex, RTObject* origin=0);	
};

#endif // RAYTRACER_H
