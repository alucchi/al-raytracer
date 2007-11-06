/**
* File : rtObjects.cpp
* Description : Objects displayed by the raytracer
*
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/ 

//-------------------------------------------------------------------- INCLUDES
#include "rtObjects.h"

//--------------------------------------------------------------------- METHODS

float Plane::Intersect(const Ray& a_Ray)
{
	float d = Dot( m_N, a_Ray.GetDirection() );
	if (d != 0)
	{
		float dist = -(Dot( m_N, a_Ray.GetOrigin() ) + m_d) / d;
		if(dist > 0)
			return dist;
		//return fabs((Dot( m_N, a_Ray.GetOrigin() ) + m_d) / d);
	}
	return std::numeric_limits<float>::infinity();
}

float Sphere::Intersect(const Ray &ray)
{
	Vector3 dst = ray.GetOrigin() - m_pos;
	Vector3 dirRay = ray.GetDirection();

	float a_Dist = std::numeric_limits<float>::infinity();
	float b = -Dot( dst, dirRay );
	float det = (b * b) - Dot( dst, dst ) + m_radius;
	if (det > 0)
	{
		det = sqrtf( det );
		float i1 = b - det;
		float i2 = b + det;
		if (i2 > 0)
		{
			if (i1 < 0) 
			{
				if (i2 < a_Dist) 
				{
					a_Dist = i2;
				}
			}
			else
			{
				if (i1 < a_Dist)
				{
					a_Dist = i1;
				}
			}
		}
	}
	return a_Dist;
}
