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
#include "defs.h"
#include "rayBoxInt.inl"
#include <algorithm>

//--------------------------------------------------------------------- METHODS

/**
 * Finds the nearest intersection between a plane and the specified ray.
 * @param ray the ray that will be fired into the scene.
 * @return std::numeric_limits<float>::infinity() if no intersection detected.
 * Otherwise the distance between the intersected object and the origin of the
 * ray is returned.
 */
float Plane::Intersect(const Ray& a_Ray)
{
	m_rayID = a_Ray.GetID();
	
	float d = Dot( m_N, a_Ray.GetDirection() );
	if (d != 0)
	{
		// Non orthogonal
		float dist = -(Dot( m_N, a_Ray.GetOrigin() ) + m_d) / d;
		if(dist > 0)
			return dist;
		//return fabs((Dot( m_N, a_Ray.GetOrigin() ) + m_d) / d);
	}
	return std::numeric_limits<float>::infinity();
}

/**
 * Finds the nearest intersection between a plane and the specified
 * bounding box.
 * @param v1 lower left corner.
 * @param v2 upper right corner.
 * @return true on success, false otherwise.
 */
bool Plane::IntersectBoundingBox(const Vector3& v1, const Vector3& v2)
{
	int side1 = 0, side2 = 0;	
	const Vector3* v[2];
	v[0] = &v1;
	v[1] = &v2;
	for (int i = 0; i < 8; i++)
	{
		Vector3 p( v[i & 1]->x, v[(i >> 1) & 1]->y, v[(i >> 2) & 1]->z );
		if ((Dot( p, m_N ) + m_d) < 0)
			side1++;
		else
			side2++;
	}
	if ((side1 == 0) || (side2 == 0))
		return false;
	else
		return true;
}

/**
 * Finds the nearest intersection between a sphere and the specified ray.
 * @param ray the ray that will be fired into the scene.
 * @return std::numeric_limits<float>::infinity() if no intersection detected.
 * Otherwise the distance between the intersected object and the origin of the
 * ray is returned.
 */
float Sphere::Intersect(const Ray &a_Ray)
{
	m_rayID = a_Ray.GetID();
	
	Vector3 dst = a_Ray.GetOrigin() - m_pos;
	Vector3 dirRay = a_Ray.GetDirection();

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

/**
 * Finds the nearest intersection between a sphere and the specified
 * bounding box.
 * @param v1 lower left corner.
 * @param v2 upper right corner.
 * @return true on success, false otherwise.
 */
bool Sphere::IntersectBoundingBox(const Vector3& v1, const Vector3& v2)
{
	float dmin = 0;
	
	if (m_pos.x < v1.x) 
	{
		dmin = (m_pos.x - v1.x) * (m_pos.x - v1.x);
	}
	else if (m_pos.x > v2.x)
	{
		dmin = (m_pos.x - v2.x) * (m_pos.x - v2.x);
	}
	
	if (m_pos.y < v1.y)
	{
		dmin += (m_pos.y - v1.y) * (m_pos.y - v1.y);
	}
	else if (m_pos.y > v2.y)
	{
		dmin += (m_pos.y - v2.y) * (m_pos.y - v2.y);
	}
	
	if (m_pos.z < v1.z)
	{
		dmin += (m_pos.z - v1.z) * (m_pos.z - v1.z);
	}
	else if (m_pos.z > v2.z)
	{
		dmin += (m_pos.z - v2.z) * (m_pos.z - v2.z);
	}
	
	// Check if the distance between the sphere and the box
	// is less than the square radius
	return (dmin <= (m_radius*m_radius));
}

/**
 * Finds the nearest intersection between a triangle and the specified ray.
 * Implementation based on :
 * http://www.devmaster.net/wiki/Ray-triangle_intersection
 * @param ray the ray that will be fired into the scene.
 * @return std::numeric_limits<float>::infinity() if no intersection detected.
 * Otherwise the distance between the intersected object and the origin of the
 * ray is returned.
 */
float Triangle::Intersect(const Ray& a_Ray)
{
	m_rayID = a_Ray.GetID();
	
	Vector3 D = a_Ray.GetDirection();	
	float d = Dot(D, m_N);
	
	if (d != 0)
	{
		Vector3 O = a_Ray.GetOrigin();
		float dist = -(Dot(m_N, O-m_A)) / d;
		if(dist > 0)
		{
			Vector3 bt = m_B - m_A;
			Vector3 ct = m_C - m_A;	
			Vector3 P = O + dist * D;
			Vector3 pt = P - m_A;
			
			Vector2 b;
			Vector2 c;	
			Vector2 p;
			
			if(bt.x == 0 && ct.x == 0)
			{
				b.Set(bt.y, bt.z);
				c.Set(ct.y, ct.z);
				p.Set(pt.y, pt.z);
			}
			else if(bt.y == 0 && ct.y == 0)
			{
				b.Set(bt.x, bt.z);
				c.Set(ct.x, ct.z);
				p.Set(pt.x, pt.z);
			}
			else
			{
				b.Set(bt.x, bt.y);
				c.Set(ct.x, ct.y);
				p.Set(pt.x, pt.y);
			}	
			
			float dU = b.y*c.x - b.x*c.y;
			float u = p.y*c.x - p.x*c.y;
			if(dU != 0)
				u /= dU;
				
			float dV = c.y*b.x - c.x*b.y;
			float v = p.y*b.x - p.x*b.y;
			if(dV != 0)
				v /= dV;				
				
			if(u>=0 && v>=0 && (u+v)<=1)
				return dist;
		}
	}
	return std::numeric_limits<float>::infinity();
}

/**
 * Finds the nearest intersection between a triangle and the specified
 * bounding box.
 * @param v1 lower left corner.
 * @param v2 upper right corner.
 * @return true on success, false otherwise.
 */
bool Triangle::IntersectBoundingBox(const Vector3& v1, const Vector3& v2)
{
	Vector3 center = (v1+v2)/2.0f;
	float boxcenter[3]={center.x,center.y,center.z};
	Vector3 halfsize = (v2-v1)/2.0f;
	float boxhalfsize[3]={halfsize.x,halfsize.y,halfsize.z};
	float triverts[3][3]={{m_A.x,m_A.y,m_A.z},{m_B.x,m_B.y,m_B.z},{m_C.x,m_C.y,m_C.z}};
	return (triBoxOverlap(boxcenter,boxhalfsize,triverts)==1);
}

Vector3 Triangle::GetNormal(Vector3& pos)
{
#ifdef VERTEX_NORMAL
	float dA = (m_A - pos).LengthSq();
	float dB = (m_B - pos).LengthSq();
	float dC = (m_C - pos).LengthSq();
	
	if(dA < dB)
	{
		if(dA < dC)
			return dA;
		else
			return dC;
	}
	else
	{
		if(dB < dC)
			return dB;
		else
			return dC;		
	}
	
#else	
	return m_N;
#endif // VERTEX_NORMAL
}

#ifdef VERTEX_NORMAL
void Triangle::SetVertexNormals(Vector3 N[3])
{
	m_NA = N[0];
	m_NB = N[1];
	m_NC = N[2];
}
#endif // VERTEX_NORMAL

/**
 * Finds the nearest intersection between a box and the specified ray.
 * Implementation based on Smits’ method
 * @TODO : see improved code on http://cag.csail.mit.edu/~amy/papers/box-jgt.pdf
 * @param ray the ray that will be fired into the scene.
 * @return std::numeric_limits<float>::infinity() if no intersection detected.
 * Otherwise the distance between the intersected object and the origin of the
 * ray is returned.
 */
float Box::Intersect(const Ray &ray) const
{
	float a_Dist;
	float dist[6];
	Vector3 ip[6], d = ray.GetDirection(), o = ray.GetOrigin();

	int i;
	for (i = 0; i < 6; i++) dist[i] = -1;

	if (d.x) 
	{
		float rc = 1.0f / d.x;
		dist[0] = (m_bounds[0].x - o.x) * rc;
		dist[3] = (m_bounds[1].x - o.x) * rc;
	}
	if (d.y) 
	{
		float rc = 1.0f / d.y;
		dist[1] = (m_bounds[0].y - o.y) * rc;
		dist[4] = (m_bounds[1].y - o.y) * rc;
	}
	if (d.z) 
	{
		float rc = 1.0f / d.z;
		dist[2] = (m_bounds[0].z - o.z) * rc;
		dist[5] = (m_bounds[1].z - o.z) * rc;
	}
	
	for ( i = 0; i < 6; i++ )
		if (dist[i] > 0)
		{
			// Check if the intersection point is inside the box
			ip[i] = o + dist[i] * d;
			if ((ip[i].x > (m_bounds[0].x - EPSILON)) && (ip[i].x < (m_bounds[1].x + EPSILON)) && 
				(ip[i].y > (m_bounds[0].y - EPSILON)) && (ip[i].y < (m_bounds[1].y + EPSILON)) &&
				(ip[i].z > (m_bounds[0].z - EPSILON)) && (ip[i].z < (m_bounds[1].z + EPSILON)))
			{
				if (dist[i] < a_Dist) 
				{
					a_Dist = dist[i];
				}
			}
		}
		
	return a_Dist;
}

/**
 * @return true if the box contains the specified vector v, false otherwise.
 */
bool Box::Contains(Vector3& v)
{
	return ((v.x > (m_bounds[0].x - EPSILON)) && (v.x < (m_bounds[1].x + EPSILON)) &&
			(v.y > (m_bounds[0].y - EPSILON)) && (v.y < (m_bounds[1].y + EPSILON)) &&
			(v.z > (m_bounds[0].z - EPSILON)) && (v.z < (m_bounds[1].z + EPSILON)));
}
