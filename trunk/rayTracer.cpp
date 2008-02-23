/**
* File : rayTracer.cpp
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

//-------------------------------------------------------------------- INCLUDES

#include "rayTracer.h"
#include "defs.h"
#include "display.h"
#include "Maths/Vector3.h"
#include "scene.h"

//--------------------------------------------------------------------- GLOBALS

extern Display	*display;

#define ANTI_ALIASING
#define SPATIAL_DIVISION

//--------------------------------------------------------------------- METHODS

/**
 * Initialize method
 */
void RayTracer::Init()
{
	m_rayID = 1;
	
	// screen plane in world space coordinates
	m_WX1 = -4, m_WX2 = 4, m_WY1 = m_SY = 3, m_WY2 = -3;
	// calculate deltas for interpolation
	m_DX = (m_WX2 - m_WX1) / SCR_WIDTH;
	m_DY = (m_WY2 - m_WY1) / SCR_HEIGHT;
	m_SY += m_DY;
	
#ifdef SPATIAL_DIVISION	
	m_Scene.BuildGrid();

	float gridSize = GRIDSIZE;	
	// precalculate size of a cell
	m_CS = m_Scene.GetBox().GetSize()/gridSize;
	// precalculate 1 / size of a cell
	m_RCS = 1.0f / m_CS;
#endif	
}

void RayTracer::AddObject(RTObject* o)
{
	m_Scene.AddObject(o);
}

/**
 * Finds the nearest intersection between the specified ray r and any object
 * in the scene.
 * @param r the ray that will be fired into the scene. 
 * @param origin The object specified by the origin pointer will not be tested.
 * @param nearestObj If an intersection is detected, nearestObj will point to
 * the intersected object. Otherwise, it will be 0.
 * @return std::numeric_limits<float>::infinity() if no intersection detected.
 * Otherwise the distance between the intersected object and the origin of the
 * ray is returned.
 */
float RayTracer::GetDistance(const Ray& r, RTObject*& nearestObj, RTObject* origin)
{
	float nearestT = std::numeric_limits<float>::infinity();	
	list<RTObject*>::iterator iObjects;
	for( iObjects = m_Scene.GetObjects().begin(); iObjects != m_Scene.GetObjects().end(); iObjects++ )
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

/**
  * This algorithm should be used when the spatial division is activated !
 * Finds the nearest intersection between the specified ray r and any object
 * in the scene.
 * @param r the ray that will be fired into the scene. 
 * @param origin The object specified by the origin pointer will not be tested.
 * @param nearestObj If an intersection is detected, nearestObj will point to
 * the intersected object. Otherwise, it will be 0.
 * @return std::numeric_limits<float>::infinity() if no intersection detected.
 * Otherwise the distance between the intersected object and the origin of the
 * ray is returned.
 */
float RayTracer::FindNearest(const Ray& a_Ray, RTObject*& nearestObj, RTObject* origin)
{
	float a_Dist=std::numeric_limits<float>::infinity();
	Vector3 raydir, curpos;
	Box boundingBox = m_Scene.GetBox();
	curpos = a_Ray.GetOrigin();
	raydir = a_Ray.GetDirection();
	
	// setup 3DDDA (double check reusability of primary ray data)
	
	// cell boundary
	Vector3 cb;
	// value of t at which the ray crosses the first vertical voxel boundary
	Vector3 tmax;
	// how far we must move along the ray to cross one cell
	Vector3 tdelta;
	Vector3 cell = (curpos - boundingBox.GetMin()) * m_RCS;
	
	// stepX and stepY are initialized to either 1 or -1 indicating whether X
	// and Y are incremented or decremented as the ray crosses voxel boundaries	
	int stepX, stepY, stepZ;
	// end of the grid
	int outX, outY, outZ;

	// X, Y, Z are initialized to the starting voxel coordinates
	int X = (int)cell.x;
	int Y = (int)cell.y;
	int Z = (int)cell.z;
	if ((X < 0) || (X >= GRIDSIZE) || (Y < 0) || (Y >= GRIDSIZE) || (Z < 0) || (Z >= GRIDSIZE))
		return a_Dist; // infinity
		
	if (raydir.x > 0)
	{
		stepX = 1;
		outX = GRIDSIZE;
		cb.x = boundingBox.GetMin().x + (X + 1) * m_CS.x;
	}
	else 
	{
		stepX = -1;
		outX = -1;
		cb.x = boundingBox.GetMin().x + X * m_CS.x;
	}
	if (raydir.y > 0.0f)
	{
		stepY = 1;
		outY = GRIDSIZE;
		cb.y = boundingBox.GetMin().y + (Y + 1) * m_CS.y; 
	}
	else 
	{
		stepY = -1;
		outY = -1;
		cb.y = boundingBox.GetMin().y + Y * m_CS.y;
	}
	if (raydir.z > 0.0f)
	{
		stepZ = 1;
		outZ = GRIDSIZE;
		cb.z = boundingBox.GetMin().z + (Z + 1) * m_CS.z;
	}
	else 
	{
		stepZ = -1;
		outZ = -1;
		cb.z = boundingBox.GetMin().z + Z * m_CS.z;
	}
	
	float rxr, ryr, rzr;
	if (raydir.x != 0)
	{
		rxr = 1.0f / raydir.x;
		tmax.x = (cb.x - curpos.x) * rxr; 
		tdelta.x = m_CS.x * stepX * rxr;
	}
	else
		tmax.x = 1000000;
	if (raydir.y != 0)
	{
		ryr = 1.0f / raydir.y;
		tmax.y = (cb.y - curpos.y) * ryr; 
		tdelta.y = m_CS.y * stepY * ryr;
	}
	else
		tmax.y = 1000000;
	if (raydir.z != 0)
	{
		rzr = 1.0f / raydir.z;
		tmax.z = (cb.z - curpos.z) * rzr; 
		tdelta.z = m_CS.z * stepZ * rzr;
	}
	else
		tmax.z = 1000000;
		
	// start stepping
	ObjectList* list = 0;
	ObjectList** grid = m_Scene.GetGrid();
	nearestObj = 0;
	// trace primary ray :
	// loop until either we find a voxel with a non-empty object list or
	// we fall out of the end of the grid.
	while (1)
	{
		//list = grid[X + Y * GRIDSIZE + Z * GRIDSIZE * GRIDSIZE];		
		list = grid[X + (Y << GRIDSHIFT) + (Z << (GRIDSHIFT * 2))];
		while (list)
		{
			RTObject* object = list->GetRTObject();
			if(object!=origin)
			{			
				//if (object->GetRayID() != a_Ray.GetID())
				//{			
					float distObj = object->Intersect(a_Ray);
					if(distObj < a_Dist)
					{
						a_Dist = distObj;
						nearestObj = object;
						goto testloop;
					}
				//}			
			}
			list = list->GetNext();
		}
		if (tmax.x < tmax.y)
		{
			if (tmax.x < tmax.z)
			{
				X = X + stepX;
				if (X == outX) return -1;
				tmax.x += tdelta.x;
			}
			else
			{
				Z = Z + stepZ;
				if (Z == outZ) return -1;
				tmax.z += tdelta.z;
			}
		}
		else
		{
			if (tmax.y < tmax.z)
			{
				Y = Y + stepY;
				if (Y == outY) return -1;
				tmax.y += tdelta.y;
			}
			else
			{
				Z = Z + stepZ;
				if (Z == outZ) return -1;
				tmax.z += tdelta.z;
			}
		}
	}
testloop:
	// the following loop is the same as the previous one with an extra comparison
	while (1)
	{
		//list = grid[X + Y * GRIDSIZE + Z * GRIDSIZE * GRIDSIZE];		
		list = grid[X + (Y << GRIDSHIFT) + (Z << (GRIDSHIFT * 2))];
		while (list)
		{
			RTObject* object = list->GetRTObject();
			if(object!=origin)
			{		
				if (object->GetRayID() != a_Ray.GetID())
				{
					float distObj = object->Intersect(a_Ray);
					if(distObj < a_Dist)
					{
						a_Dist = distObj;
						nearestObj = object;
					}
				}
			}			
			list = list->GetNext();
		}
		if (tmax.x < tmax.y)
		{
			if (tmax.x < tmax.z)
			{
				if (a_Dist < tmax.x) break; // extra test
				X = X + stepX;
				if (X == outX) break;
				tmax.x += tdelta.x;
			}
			else
			{
				if (a_Dist < tmax.z) break;
				Z = Z + stepZ;
				if (Z == outZ) break;
				tmax.z += tdelta.z;
			}
		}
		else
		{
			if (tmax.y < tmax.z)
			{
				if (a_Dist < tmax.y) break;
				Y = Y + stepY;
				if (Y == outY) break;
				tmax.y += tdelta.y;
			}
			else
			{
				if (a_Dist < tmax.z) break;
				Z = Z + stepZ;
				if (Z == outZ) break;
				tmax.z += tdelta.z;
			}
		}
	}
	return a_Dist;
}

/**
 * Raytrace a specified ray into the scene.
 * @param ray is the ray that will be fired into the scene. 
 * @param color is the color of the pixel on the screen. A black color is
 * returned if no objects intersected.
 * @param depth specify the number of recursive calls until now
 * @param rIndex is the refraction index of the previous encountered material
 * @param origin is the object from which the ray has been traced (0 if the ray comes
 * from the eye). 
 * @return This method returns a null pointer if no object is intersected by the ray.
 */
RTObject* RayTracer::RayTrace(const Ray& ray, Color& color, int depth, float rIndex, RTObject* origin)
{
	if(depth > MAX_RAYTRACE_DEPTH) return 0;

	// Find nearest object
	RTObject* nearestObj = 0;

#ifdef SPATIAL_DIVISION
	float distObj = FindNearest(ray,nearestObj, origin);
#else
	float distObj = GetDistance(ray,nearestObj, origin);
#endif

	if(nearestObj)
	//if(distObj != std::numeric_limits<float>::infinity() && distObj>=0)
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
			for( iObjectLight = m_Scene.GetObjects().begin(); iObjectLight != m_Scene.GetObjects().end(); iObjectLight++ )
			{
				if((*iObjectLight)->GetType() == RTObject::LIGHT)
				{					
					Vector3 L = (*iObjectLight)->GetPosition()-posObj;
					// Don't use L.Normalize() to speed things up as we need to get the lenght of L
					float distL = L.Length();
					if(distL > std::numeric_limits<float>::epsilon())
						L *= 1/distL;
					Ray rLight(posObj, L, m_rayID++);
																						
					RTObject* nearestObjL;			
					#ifdef SPATIAL_DIVISION
						float distObj = FindNearest(rLight, nearestObjL, nearestObj);
					#else
						float distObj = GetDistance(rLight, nearestObjL, nearestObj);
					#endif

					if(nearestObjL == *iObjectLight)
					{
						// No shadow as there is no object between the
						// intersected object and the light
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
				Ray reflRay(posObj, R, m_rayID++);

				Color rcol;
				// Instead of passing a pointer to the origin object, we could move
				// the ray a little bit doing : posObj+(R*EPSILON)
				RayTrace(reflRay, rcol, depth + 1, rIndex, nearestObj);
				color += rcol * reflection * nearestObj->GetMaterial()->GetColor();
			}
			// calculate refraction using Snell's law
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
					Ray refrRay(posObj, T, m_rayID++);
					Color rcol;
					RayTrace(refrRay, rcol, depth + 1, new_rIndex, nearestObj);
					color += rcol * refraction;
				}
			}		
		}
	}

	return nearestObj;
}

/**
 * Render the scene. The eye vector is the position from which the viewer sees
 * the scene.
 */
void RayTracer::Render()
{
	Screen screen=display->GetScreen();
	
#ifdef SPATIAL_DIVISION	
	Box bb = m_Scene.GetBox();
#endif	

#ifdef ANTI_ALIASING
	RTObject* lastObject = 0;
	RTObject* lineObject[SCR_WIDTH];
#endif

	// For each pixel in the image
	for(int h=0;h<SCR_HEIGHT;h++)	
	{
		m_SX = m_WX1;
		for(int w=0;w<SCR_WIDTH;w++)
		{
			// Create ray from eyepoint passing through this pixel
			Vector3 o(m_SX,m_SY,0);
			Vector3 dirEye = o-eye;
			dirEye.Normalize();
			Ray rEye(eye,dirEye,m_rayID);
			m_rayID++;

#ifdef SPATIAL_DIVISION
			// advance ray to scene bounding box boundary
			if (!bb.Contains(o))
			{
				float bbDist;
				if (bbDist=bb.Intersect(rEye))
				{
					Vector3 origin = o+(bbDist + EPSILON) * dirEye;
					rEye.SetOrigin(origin);
				}
			}
#endif

			Color color;
			RTObject* object = RayTrace(rEye,color,0,1,0);			

			int red, green, blue;
			
#ifdef ANTI_ALIASING
			// super-sampling only when we encounter a new primitive
			if(lastObject != object || lineObject[w] != object)
			{
				lastObject = object;
				lineObject[w] = object;
				
				for ( int tx = -1; tx < 2; tx++ )
					for ( int ty = -1; ty < 2; ty++ )
					{
						Vector3 dir = Vector3( m_SX + m_DX * tx / 2.0f, m_SY + m_DY * ty / 2.0f, 0 ) - eye;
						dir.Normalize();
						Ray r(eye, dir, m_rayID++);
						RayTrace(r,color,0,1,0);
					}				
				
				red = (int)(color.x * 256.0f/9.0f);
				green = (int)(color.y * 256.0f/9.0f);
				blue = (int)(color.z * 256.0f/9.0f);				
			}
			else
#endif
			{				
				red = (int)(color.x * 256.0f);
				green = (int)(color.y * 256.0f);
				blue = (int)(color.z * 256.0f);
			}
			
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

/**
 * Imports the ASE model contained in the file whose name is specified in
 * strFileName
 * @param strFileName file name that contains the ASE model to be imported
 */
void RayTracer::ImportASE(char *strFileName)
{
	m_Scene.ImportASE(strFileName);
}
