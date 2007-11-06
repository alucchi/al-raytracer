/**
* File : display.h
* Description : Object to manage the display area on the screen and a few other
*  things associated with display.
* Author(s) : ALucchi
* Date of creation : 28/10/2007
* Modification(s) :
*/

#ifndef DISPLAY_H
#define DISPLAY_H

//--------------------------------------------------------------------- INCLUDE

//----------------------------------------------------------------------- TYPES

//--------------------------------------------------------------------- CLASSES

class Display
{

private:

	Screen		screen;		// Screen's surface

	SDL_Surface* surface; // we must keep the surface to use SDL functions

public:

	Display();
	~Display();
	
	bool	Init(dword bpp);
	void	Deinit();

	int	SetPalette(const L3DC_Color* pal);
	void	GetPalette(byte *pal);
	
	void	Clear();
	// Clear screen

	bool	Flip();
	// Flip buffers and wait for synchronisation with screen

	int		LoadBmp(char* name);

	Screen	GetScreen();
	// Get a pointer on the screen

};

#endif // DISPLAY_H
