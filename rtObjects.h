/**
* File : rtObjects.h
* Description : Objects displayed by the raytracer
*
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef RTOBJECTS_H
#define RTOBJECTS_H

//-------------------------------------------------------------------- INCLUDES
#include "Maths/math3D.h"
#include <limits>

//----------------------------------------------------------------------- TYPES
typedef Vector3 Color;

//---------------------------------------------------------------------- CONSTS
static Color WHITE(1.0f,1.0f,1.0f);

//--------------------------------------------------------------------- CLASSES

class Ray
{
private:
	Vector3 m_dir; // direction
	Vector3 m_pos; // center of the object

public:
	Vector3 GetDirection() const {return m_dir;}
	Vector3 GetOrigin() const {return m_pos;}
	Ray(Vector3 p, Vector3 d):m_pos(p),m_dir(d){}
};

// -----------------------------------------------------------
// Material class
// -----------------------------------------------------------
class RTMaterial
{
public:
	RTMaterial() { m_Diff = 1.0f; m_Refl = 0.2f; m_Refr = 0.1f; m_RIndex = 1.0f; }
	
	Color GetColor() { return m_Color; }
	float GetDiffuse() { return m_Diff; }	
	float GetSpecular() { return 1.0f - m_Diff; }	
	float GetReflection() { return m_Refl; }
	float GetRefraction() { return m_Refr; }
	float GetRefrIndex() { return m_RIndex; }
	void SetColor( Color& a_Color ) { m_Color = a_Color; }	
	void SetDiffuse( float a_Diff ) { m_Diff = a_Diff; }
	void SetReflection( float a_Refl ) { m_Refl = a_Refl; }
	void SetRefraction( float a_Refr ) { m_Refr = a_Refr; }	
	void SetRefrIndex( float a_Refr ) { m_RIndex = a_Refr; }	
private:
	Color m_Color;
	float m_Diff;	
	float m_Refl;
	float m_Refr;
	float m_RIndex;	
};

// -----------------------------------------------------------
// Raytracing object class
// -----------------------------------------------------------
class RTObject
{
protected:
	int type;
	Vector3 m_pos; // center of the object
	RTMaterial m_material;

public:
	enum TYPE
	{
		SPHERE = 1,
		PLANE,
		LIGHT
	};
	
	RTObject(Vector3 p, int t=0):m_pos(p),type(t){}

	virtual RTMaterial* GetMaterial() { return &m_material;}
	virtual Vector3 GetNormal( Vector3& pos ) {return Vector3(0,0,0);}
	virtual Vector3 GetPosition() { return m_pos; }
	virtual int GetType() { return type; }	
	virtual float Intersect(const Ray &ray) { return std::numeric_limits<float>::infinity();}
};

class Plane : public RTObject
{
public:
	Vector3 m_N;
	float m_d;

	Vector3 GetNormal( Vector3& pos ) {return m_N;}
	float Intersect(const Ray &ray);
	Plane(Vector3 N, float d):RTObject(Vector3(0,0,0),PLANE),m_N(N),m_d(d){}
};

class Sphere : public RTObject
{
public:
	float m_radius;

	Sphere(Vector3 p, float r):RTObject(p, SPHERE)
	{
		this->m_radius = r;
	}

	Sphere(Vector3 p, float r, int type):RTObject(p, type)
	{
		this->m_radius = r;
	}

	float Intersect(const Ray &ray);

	Vector3 GetNormal( Vector3& pos ) { return (pos - m_pos) * m_radius; }
};

class Light : public Sphere
{
public:
	Light(Vector3 p, float r):Sphere(p, r, LIGHT){m_material.SetColor(WHITE);}
}; 

#endif // RTOBJECTS_H
