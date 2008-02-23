/**
* File : Main.cpp
* Description : Main file
*
* Author(s) : ALucchi
* Date of creation : 27/10/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include "defs.h"
#include "display.h"
#include "rayTracer.h"

#include <string.h>
#include <SDL/SDL.h>

//----------------------------------------------------------------------- TYPES

//------------------------------------------------------------------- VARIABLES

Display		*display = NULL;	// Application's display object

//------------------------------------------------------------------- FUNCTIONS

#ifndef WINDOWS
#include <sys/times.h>
long GetTickCount()
{
    tms tm;
    return times(&tm);
}
#endif

bool init();
void deinit();
bool msgLoop();

int main(int argc, char *argv[])
{	
	init();

	RayTracer rayTracer;
	Color ground(1.0f,0.4f,0.4f);
	Color red(1.0f,0.1f,0.1f);
	Color green(0.5f,1.0f,0.2f);
	Color blue(0.05f,0.05f,0.7f);
	Color orange(0.9f,0.5f,0.1f);
	Color white(1.0f,1.0f,1.0f);
	Color lColor(0.9f,0.8f,0.8f);
	
	Plane p(Vector3(0,1,0),6);
	p.GetMaterial()->SetColor(ground);
	p.GetMaterial()->SetRefraction(0.0f);
	p.GetMaterial()->SetDiffuse(1.0f);	
	Plane p2(Vector3(0,0,1),-20);
	p2.GetMaterial()->SetColor(white);
	
	Sphere s(Vector3(0, -2, 2),0.6f);
	s.GetMaterial()->SetColor(blue);
	s.GetMaterial()->SetRefraction(0.5f);
	Sphere s2(Vector3(-3, -2, 5),1.0f);
	s2.GetMaterial()->SetColor(blue);
	s2.GetMaterial()->SetRefraction(0.3f);
	//s2.GetMaterial()->SetRefrIndex(0.2f);
	Sphere s3(Vector3(-4, 2, 7),0.5f);
	s3.GetMaterial()->SetColor(green);
	s3.GetMaterial()->SetRefraction(0.0f);
	s3.GetMaterial()->SetDiffuse(0.8f);		
	
	cout << "Adding objects\n";
	
	for(int i=-3;i<3;i++)
	for(int j=6;j<12;j++)
	{
		Vector3 v(i, -3, j);
		Sphere* s = new Sphere(v,0.1f);
		s->GetMaterial()->SetColor(green);
		s->GetMaterial()->SetRefraction(0.0f);
		s->GetMaterial()->SetDiffuse(0.8f);		
		rayTracer.AddObject(s);
	}	
	
	for(int i=-4;i<2;i++)
	for(int j=8;j<10;j++)
	{
		Vector3 v(i, -1, j);
		Sphere* s = new Sphere(v,0.1f);
		s->GetMaterial()->SetColor(red);
		s->GetMaterial()->SetRefraction(0.0f);
		s->GetMaterial()->SetDiffuse(0.8f);		
		rayTracer.AddObject(s);
	}	
	
	for(int i=-3;i<3;i++)
	for(int j=12;j<14;j++)
	{
		Vector3 v(i, 1, j);
		Sphere* s = new Sphere(v,0.1f);
		s->GetMaterial()->SetColor(orange);
		s->GetMaterial()->SetRefraction(0.0f);
		s->GetMaterial()->SetDiffuse(0.8f);		
		rayTracer.AddObject(s);
	}
	
//	// OK
//	Triangle t(Vector3(0,0,3),Vector3(1,0,3),Vector3(1,1,3));
//	t.GetMaterial()->SetColor(green);
//	t.GetMaterial()->SetRefraction(0.0f);	
//	
//	Triangle t2(Vector3(0,0,3),Vector3(0,-2,3),Vector3(1,0,3));
//	t2.GetMaterial()->SetColor(green);
//	t2.GetMaterial()->SetRefraction(0.0f);	
//	
//	Triangle t3(Vector3(1,-2,3),Vector3(1,0,3),Vector3(0,-2,3));
//	t3.GetMaterial()->SetColor(green);
//	t3.GetMaterial()->SetRefraction(0.0f);		
//	
//	Triangle t4(Vector3(0,-1,3),Vector3(1,-1,3),Vector3(1,0,3));
//	t4.GetMaterial()->SetColor(green);
//	t4.GetMaterial()->SetRefraction(0.0f);	
//	
//	Triangle t5(Vector3(0,-2,4),Vector3(1,-2,4),Vector3(0.5,-2,3.98));
//	t5.GetMaterial()->SetColor(green);
//	t5.GetMaterial()->SetRefraction(0.0f);	
	
	Light l(Vector3(0,3,3),1.0f);
	l.GetMaterial()->SetColor(lColor);
	l.GetMaterial()->SetRefraction(0);
	Light l2(Vector3(4,-3,2),0.7f);
	l2.GetMaterial()->SetColor(lColor);
	l2.GetMaterial()->SetRefraction(0);
	//l2.GetMaterial()->SetColor(lColor);
	Light l3(Vector3(-3,0,20),0.2f);
	l3.GetMaterial()->SetColor(lColor);
	l3.GetMaterial()->SetRefraction(0);
	Light l4(Vector3(3.5,-1,-2),0.8f);
	l4.GetMaterial()->SetColor(lColor);
	l4.GetMaterial()->SetRefraction(0);	
	
	rayTracer.AddObject(&p);
	//rayTracer.AddObject(&t4);
	//rayTracer.AddObject(&p2);
	rayTracer.AddObject(&s);
	//rayTracer.AddObject(&s2);
	rayTracer.AddObject(&s3);
	rayTracer.AddObject(&l);
	//rayTracer.AddObject(&l2);
	rayTracer.AddObject(&l3);
	rayTracer.AddObject(&l4);

	rayTracer.ImportASE("mesh/cyl2.ase");

	cout << "Objects added\n";

	long start = GetTickCount();

	display->Clear();
	rayTracer.Init();
	rayTracer.Render();
	display->Flip();

	long end = GetTickCount();

	printf("Time: %ld ms\n", (end-start)*10);

	// Enter the message loop
	while(msgLoop())
	{
	}

	printf("Application over\n");

	return 0;
}

bool init()
{
	display = new Display;
	
	// Init display
	if(!display->Init(SCR_BPP))
	{
		display->Deinit();
		return false;
	}
	
	return true;
}

void deinit()
{
	// Shutdown display
	if(display)
	{
		display->Deinit();
		delete display;
	}
}

bool msgLoop()
{
	SDL_Event event;
	
	// Parse the SDL events and evetually take some actions

	while(SDL_PollEvent(&event)) {
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					return false;
					break;
			}
			break;			
		case SDL_QUIT:
			return false;
			break;
		}
	}

	return true;
}
