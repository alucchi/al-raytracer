/**
* File : display.h
* Description : Object to manage the display area on the screen and a few other
* things associated with the display.
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
	// Screen's surface
	Screen		screen;
	// Pointer used to call the SDL functions
	SDL_Surface* surface;

public:
	Display();
	~Display();
	
	void Deinit();
	bool Init(dword bpp);	

	int	SetPalette(const L3DC_Color* pal);
	void GetPalette(byte *pal);
	
	// Clear the display
	void Clear();	

	// Flip back and front buffers and wait for synchronisation with screen
	bool Flip();	

	int	LoadBmp(char* name);

	// Get a pointer on the screen
	Screen	GetScreen();
};

#endif // DISPLAY_H
