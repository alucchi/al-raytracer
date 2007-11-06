/**
* File : defs.h
* Description : Constants, types,...
*				  
* Author(s) : ALucchi
* Date of creation : 27/10/2007
* Modification(s) :
*/

#ifndef DEFS_H
#define DEFS_H

//-------------------------------------------------------------------- INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "Maths/math3D.h"

#include <SDL/SDL.h>

//---------------------------------------------------------------------- CONSTS

/* Debug */

#define TITLE	"Ray tracer by Aurelien Lucchi"
#define SCREEN_COLOR	0xFFFFFF

/* Screen sizes */	
#define SCR_WIDTH 640
#define SCR_HEIGHT 480
#define SCR_BPP	32 // under review (we should use 16 bits under Windows)


//---------------------------------------------------------------------- MACROS

#define degstorads(x)	((x) * (DEGS_PER_RAD))
#define radstodegs(x)	((x) * (RADS_PER_DEG))

#define tan(x)			(sin(x) / cos(x))
#define cot(x)			(cos(x) / sin(x))

#define INVERSE(x)		((1.0f) / (x))

#define SQUARE(x)		((x) * (x))
#define CUBE(x)			((x) * (x) * (x))

//----------------------------------------------------------------------- TYPES

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned int uint;

// Screen's surface
#if SCR_BPP == 8
	typedef byte * Screen;	// 8 bits
#elif SCR_BPP == 16
	typedef word * Screen;	// 16 bits
#elif SCR_BPP == 32
	typedef dword * Screen;	// 32 bits
#endif

// Color
typedef struct L3DC_Color {
	byte r;
	byte g;
	byte b;
	byte unused;
} L3DC_Color;

#endif // DEFS_H
