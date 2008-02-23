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
#include <assert.h>

//----------------------------------------------------------------------- TYPES
typedef Vector3 Color;

//---------------------------------------------------------------------- CONSTS
static Color GREEN(0.0f,1.0f,0.0f);
static Color WHITE(1.0f,1.0f,1.0f);

static Vector3 NULLVECTOR3(0,0,0);

//--------------------------------------------------------------------- CLASSES

class Ray
{
private:
	int m_Id;
	Vector3 m_dir; // direction
	Vector3 m_pos; // center of the object

public:
	Vector3 GetDirection() const {return m_dir;}	
	Vector3 GetOrigin() const {return m_pos;}
	int GetID() const {return m_Id;}
	void SetOrigin(Vector3& pos) {m_pos=pos;}
	Ray(Vector3& p, Vector3& d, int rID):m_pos(p),m_dir(d),m_Id(rID){}
};

// -----------------------------------------------------------
// Bounding box class
// -----------------------------------------------------------
class Box {
public:
	Box(const Vector3 &min, const Vector3 &max) {
		assert(min < max);
		m_bounds[0] = min;
		m_bounds[1] = max;
	}
	bool Contains(Vector3& v);
	float Intersect(const Ray &r) const;
	
	Vector3 GetMin() const {return m_bounds[0];}
	Vector3 GetMax() const {return m_bounds[1];}
	Vector3 GetSize() const {return m_bounds[1]-m_bounds[0];}
private:	
	Vector3 m_bounds[2];
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
	void SetColor(Color& a_Color) { m_Color = a_Color; }	
	void SetDiffuse(float a_Diff) { m_Diff = a_Diff; }
	void SetReflection(float a_Refl) { m_Refl = a_Refl; }
	void SetRefraction(float a_Refr) { m_Refr = a_Refr; }	
	void SetRefrIndex(float a_Refr) { m_RIndex = a_Refr; }	
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
	int m_type;
	Vector3 m_pos; // center of the object
	RTMaterial m_material;
	int m_rayID; // ID of the last ray that was tested for intersection

public:
	enum TYPE
	{
		SPHERE = 1,
		LIGHT,
		PLANE,
		TRIANGLE
	};
	
	RTObject(Vector3& p, int t=0):m_pos(p),m_type(t),m_rayID(-1){}

	virtual RTMaterial* GetMaterial() { return &m_material;}
	virtual Vector3 GetNormal( Vector3& pos ) {return Vector3(0,0,0);}
	virtual Vector3 GetPosition() { return m_pos; }
	virtual int GetType() { return m_type; }
	virtual int GetRayID() { return m_rayID; }
	virtual float Intersect(const Ray &ray) { return std::numeric_limits<float>::infinity();}
	virtual bool IntersectBoundingBox(const Vector3& v1, const Vector3& v2) {return false;}
	bool IntersectBoundingBox(const Box& box) {return IntersectBoundingBox(box.GetMin(),box.GetMax());}	
};

class Plane : public RTObject
{
public:
	Vector3 m_N;
	float m_d;

	Vector3 GetNormal(Vector3& pos) {return m_N;}
	float Intersect(const Ray &ray);
	bool IntersectBoundingBox(const Vector3& v1, const Vector3& v2);	
	Plane(Vector3 N, float d):RTObject(NULLVECTOR3,PLANE),m_N(N),m_d(d){}
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

	Vector3 GetNormal(Vector3& pos) { return (pos - m_pos) * m_radius; }
	float Intersect(const Ray &a_Ray);
	bool IntersectBoundingBox(const Vector3& v1, const Vector3& v2);
};

class Light : public Sphere
{
public:
	Light(Vector3 p, float r):Sphere(p, r, LIGHT){m_material.SetColor(WHITE);}
}; 

class Triangle : public RTObject
{
private:
	Vector3 m_A, m_B, m_C; // coordinates
	Vector3 m_N; // triangle normal

#ifdef VERTEX_NORMAL
	Vector3 m_NA, m_NB, m_NC; // vertex normals
#endif

public:
	Vector3 GetNormal(Vector3& pos);
	float Intersect(const Ray &ray);
	bool IntersectBoundingBox(const Vector3& v1, const Vector3& v2);
#ifdef VERTEX_NORMAL
	void SetVertexNormals(Vector3 N[3]);
#endif	
	
	Triangle(Vector3 A, Vector3 B, Vector3 C):
		RTObject(NULLVECTOR3,TRIANGLE),m_A(A),m_B(B),m_C(C)
		{
			m_N = Cross((m_B-m_A),(m_C-m_A)); // AB.AC => clockwize
		}
};

#endif // RTOBJECTS_H
