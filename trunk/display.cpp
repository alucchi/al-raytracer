/**
* File : display.cpp
* Description : Object to manage the display area on the screen and a few other
* things associated with the display.
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

//-------------------------------------------------------------------- INCLUDES
#include <SDL/SDL.h>
#include <stdio.h>

#include "defs.h"
#include "display.h"

//--------------------------------------------------------------------- CLASSES

Display::Display()
{
	screen	= NULL;
}

Display::~Display()
{
	Deinit();
}

/**
 * Initialize the display (use the SDL functions)
 * @param bpp bits-per-pixel value.
 * @return true if the initialization has been done successfully.
 */
bool Display::Init(dword bpp)
{
	SDL_VideoInfo	*info;
	dword	flags = 0;
	bool fullscr = true;		// Should we run in fullscreen mode ?
	bool doubleBuf = false;    // Should we try to create a double buffer ?

	assert(screen == NULL);
	
	// Initialize inner SDL video system
	// FIXME: Should be already done since app must call SDL_Init(..)
	if(!SDL_WasInit(SDL_INIT_VIDEO))
	{
		if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL video initialization failed : %s\n", SDL_GetError());
			return false;
		}
	}

	// Check manualy for valid bit-depth, because of a weird bug in SDL
	switch(bpp)
	{
		case 8:
		case 15:
		case 16:
		case 24:
		case 32:
			break;
		default:
			printf("Invalid bit depth!");
			Deinit();
			return false;
	}

	// Set display flags
	if(fullscr)
		flags |= SDL_FULLSCREEN;

	info = (SDL_VideoInfo*)SDL_GetVideoInfo();	
	if(info->hw_available)
	{
		flags |= SDL_HWSURFACE;
		if(doubleBuf)
			flags |= SDL_DOUBLEBUF;
	}

	// Check if the required display mode is supported 
	if(fullscr)
	{
		if(SDL_VideoModeOK(SCR_WIDTH, SCR_HEIGHT, bpp, flags) != bpp)
		{
			printf("Unsupported display mode: %dx%d %d bpp", SCR_WIDTH, SCR_HEIGHT, bpp);
			flags &= ~SDL_FULLSCREEN;
		}
	}	

	// Init the display
	flags=SDL_SWSURFACE;
	surface = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, bpp, flags);	
	if(!surface && info->hw_available)
	{
		// Try again with software surface
		flags &= ~SDL_HWSURFACE;
		if(doubleBuf)
			flags &= ~SDL_DOUBLEBUF;
		surface = SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, bpp, flags);
	}	

	if(!surface) {
		printf("Cannot create display surface!\n");
		Deinit();
		return false;
	}

	// Everything is OK. We can use the surface
	screen=(Screen)surface->pixels;

	if(info->wm_available)
		SDL_WM_SetCaption(TITLE, NULL);

	SDL_ShowCursor(SDL_DISABLE);

	return true;
}

/**
 * This function allows to shut down the system that has been previously
 * initialized using the Init method
 */
void Display::Deinit()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	screen = NULL;
}

/**
 * Clear the screen
 */
void Display::Clear()
{
	SDL_FillRect(surface, NULL, SCREEN_COLOR);
}

int Display::SetPalette(const L3DC_Color* pal)
{
	assert(screen != NULL);
	assert(pal != NULL);
	
	if(surface->format->BitsPerPixel > 8)
		return 0;
	
	return SDL_SetPalette(surface, SDL_PHYSPAL | SDL_LOGPAL, (SDL_Color*)pal, 0, 256);
}

void Display::GetPalette(byte *pal)
{
	assert(screen != NULL);
	assert(pal != NULL);

	if(surface->format->BitsPerPixel > 8)
		return;

	SDL_Palette *surfPal = surface->format->palette;
	byte *dst = pal;

	memset(dst, 0, 768);
	
	for(int i = 0; i < surfPal->ncolors; i++)
	{
		*dst++ = surfPal->colors[i].r;
		*dst++ = surfPal->colors[i].g;
		*dst++ = surfPal->colors[i].b;
	}
}


/**
 * Flip buffers and wait for synchronisation with screen
 * @return true on success, false otherwise.
 */
bool Display::Flip()
{
	return (SDL_Flip(surface) == 0);
}

Screen Display::GetScreen()
{
	return screen;
}
