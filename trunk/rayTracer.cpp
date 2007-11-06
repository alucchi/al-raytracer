/**
* File : rayTracer.cpp
* Description : Simple raytracer that fires rays through the pixels in the
* screen and recursively spawns new rays upon reflection or refraction
* Links : http://www.devmaster.net/articles/raytracing_series/part1.php
*				  
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#include "rayTracer.h"
#include "defs.h"
#include "display.h"
#include "Maths/Vector3.h"

extern Display	*display;
//--------------------------------------------------------------------- METHODS

void RayTracer::Init()
{
	// screen plane in world space coordinates
	m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	// calculate deltas for interpolation
	m_DX = (m_WX2 - m_WX1) / SCR_WIDTH;
	m_DY = (m_WY2 - m_WY1) / SCR_HEIGHT;
	m_SY += m_DY;
}

void RayTracer::AddObject(RTObject* o)
{
	lObjects.push_front(o);
}

float RayTracer::GetDistance(const Ray& r, RTObject*& nearestObj, RTObject* origin)
{
	float nearestT = std::numeric_limits<float>::infinity();	
	list<RTObject*>::iterator iObjects;
	for( iObjects = lObjects.begin(); iObjects != lObjects.end(); iObjects++ )
	{
		if((*iObjects) != origin)
		{
			float distObj = (*iObjects)->Intersect(r);
			if(distObj < nearestT)
			{
				nearestObj = (*iObjects);
				nearestT = distObj;
			}
		}
	}
	return nearestT;
}

Color RayTracer::RayTrace(const Ray& ray, int depth, float rIndex, RTObject* origin)
{
	Color color(0,0,0);

	if(depth > MAX_RAYTRACE_DEPTH) return color;

	// Find nearest object
	RTObject* nearestObj = 0;
	float distObj = GetDistance(ray,nearestObj, origin);

	if(nearestObj)
	{
		//if(nearestObj->GetType() == RTObject::LIGHT)
		if(false) // debug to display the lights
		{
			color = nearestObj->GetMaterial()->GetColor();
		}
		else
		{
			Vector3 posObj = ray.GetOrigin() + ray.GetDirection() * distObj;
			// Normal
			Vector3 N = nearestObj->GetNormal(posObj);
			N.Normalize();

			// Shoot a ray to each light source to check if in shadow
			list<RTObject*>::iterator iObjectLight;
			for( iObjectLight = lObjects.begin(); iObjectLight != lObjects.end(); iObjectLight++ )
			{
				if((*iObjectLight)->GetType() == RTObject::LIGHT)
				{					
					Vector3 L = (*iObjectLight)->GetPosition()-posObj;
					// Don't use L.Normalize() to speed things up as we need to get the lenght of L
					float distL = L.Length();
					if(distL > std::numeric_limits<float>::epsilon())
						L *= 1/distL;
					Ray rLight(posObj, L);
					
					bool shadow = false;
					list<RTObject*>::iterator iObject;
					for( iObject = lObjects.begin(); iObject != lObjects.end(); iObject++ )
					{
						if(((*iObject) != nearestObj) && ((*iObject) != *iObjectLight))
						{
							float distObj = (*iObject)->Intersect(rLight);
							if(distObj < distL)
							{
								//std::cout << (*iObjectLight)->GetType() << " ; " << (*iObjectLight)->GetPosition() << std::endl;
								//std::cout << (*iObject)->GetType() << " ; " << (*iObject)->GetPosition() << std::endl;
								shadow=true;
								break;
							}
						}
					}

					if(!shadow)
					{
						// no object on the way
						float angleNL = Dot(N,L);
						if(angleNL > 0)
						{
							color += nearestObj->GetMaterial()->GetColor()*angleNL*nearestObj->GetMaterial()->GetDiffuse()*(*iObjectLight)->GetMaterial()->GetColor();
						}

						if (nearestObj->GetMaterial()->GetSpecular() > 0)
						{
							// point light source: sample once for specular highlight
							Vector3 V = ray.GetDirection();
							Vector3 R = L - 2.0f * Dot( L, N ) * N;
							float dotVR = Dot( V, R );
							if (dotVR > 0)
							{
								float spec = powf( dotVR, 20 ) * nearestObj->GetMaterial()->GetSpecular();
								// add specular component to ray color
								color += ((*iObjectLight)->GetMaterial()->GetColor() * spec);
							}
						}
					}
				}
			}
			// calculate reflection
			float reflection = nearestObj->GetMaterial()->GetReflection();
			if (reflection > 0.0f)
			{
				Vector3 R = ray.GetDirection() - 2.0f * Dot( ray.GetDirection(), N ) * N;
				Ray reflRay(posObj, R);

				Color rcol = RayTrace(reflRay, depth + 1, rIndex, nearestObj);
				color += rcol * reflection * nearestObj->GetMaterial()->GetColor();
			}
			// calculate refraction
			float refraction = nearestObj->GetMaterial()->GetRefraction();
			if (refraction > 0.0f)
			{
				float new_rIndex = nearestObj->GetMaterial()->GetRefrIndex();
				float n = rIndex / new_rIndex;
				if(distObj<0)
					N = -N;
				float cosI = -Dot( N, ray.GetDirection() );
				float cosT2 = 1.0f - n * n * (1.0f - cosI * cosI);
				if (cosT2 > 0.0f)
				{
					Vector3 T = (n * ray.GetDirection()) + (n * cosI - sqrtf( cosT2 )) * N;
					Ray refrRay(posObj, T);
					Color rcol = RayTrace(refrRay, depth + 1, new_rIndex, nearestObj);
					color += rcol * refraction;
				}
			}
		}
	}

	return color;
}

void RayTracer::Render()
{
	Screen screen=display->GetScreen();
	Vector3 eye(0,0,-10);

	// For each pixel in image
	for(int h=0;h<SCR_HEIGHT;h++)	
	{
		m_SX = m_WX1;
		for(int w=0;w<SCR_WIDTH;w++)
		{
			// Create ray from eyepoint passing through this pixel
			Vector3 dirEye = Vector3(m_SX,m_SY,0)-eye;
			dirEye.Normalize();
			Ray rEye(eye,dirEye);

			Color color = RayTrace(rEye,0,1,0);

			int red = (int)(color.x * 256.0f);
			int green = (int)(color.y * 256.0f);
			int blue = (int)(color.z * 256.0f);
			if(red > 255)	red = 255;
			if(green > 255)	green = 255;
			if(blue > 255)	blue = 255;

			*screen = (red << 16) + (green << 8) + blue;
			m_SX += m_DX;
			screen++;
		}
		m_SY += m_DY;
	}	
}
