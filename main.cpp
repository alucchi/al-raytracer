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
bool init();
void deinit();
bool msgLoop();

//------------------------------------------------------------------------ MAIN

int main(int argc, char *argv[])
{	
	init();

	RayTracer rayTracer;
	Color ground(1.0f,0.4f,0.4f);
	Color green(0.5f,1.0f,0.2f);
	Color blue(0.05f,0.05f,0.7f);
	Color white(1.0f,1.0f,1.0f);
	Color lColor(0.9f,0.8f,0.8f);
	
	Plane p(Vector3(0,1,0),6);
	p.GetMaterial()->SetColor(ground);
	Plane p2(Vector3(0,0,1),-20);
	p2.GetMaterial()->SetColor(white);
	
	Sphere s(Vector3(0, 0, 8),0.4f);
	s.GetMaterial()->SetColor(green);
	s.GetMaterial()->SetRefraction(0.0f);
	Sphere s2(Vector3(0, 0, 5),1.0f);
	s2.GetMaterial()->SetColor(blue);
	s2.GetMaterial()->SetRefraction(0.5f);
	//s2.GetMaterial()->SetRefrIndex(0.2f);
	Sphere s3(Vector3(3, -2, 7),0.5f);
	s3.GetMaterial()->SetColor(green);
	s3.GetMaterial()->SetRefraction(0.0f);
	s3.GetMaterial()->SetDiffuse(0.8f);	
	
	Light l(Vector3(-3,2,2),0.5f);
	l.GetMaterial()->SetColor(lColor);
	l.GetMaterial()->SetRefraction(0);
	Light l2(Vector3(3,-3,2),0.3f);
	l2.GetMaterial()->SetColor(lColor);
	l2.GetMaterial()->SetRefraction(0);
	//l2.GetMaterial()->SetColor(lColor);
	Light l3(Vector3(-3,-2,20),0.2f);
	l3.GetMaterial()->SetColor(lColor);
	l3.GetMaterial()->SetRefraction(0);
	
	rayTracer.AddObject(&p);
	//rayTracer.AddObject(&p2);
	rayTracer.AddObject(&s);
	rayTracer.AddObject(&s2);
	rayTracer.AddObject(&s3);
	rayTracer.AddObject(&l);
	rayTracer.AddObject(&l2);
	//rayTracer.AddObject(&l3);

	display->Clear();
	rayTracer.Init();
	rayTracer.Render();
	display->Flip();

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
