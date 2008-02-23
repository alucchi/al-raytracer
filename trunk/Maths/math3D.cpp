/**
* File : math3D.cpp
* Description : 3D Math library
*
* Author(s) : ALucchi
* Date of creation : 18/12/2005
* Modification(s) : ALucchi => 11/06/2007 : Refactoring
*/

//-------------------------------------------------------------------- INCLUDES
#include "math3D.h"

//------------------------------------------------------------------- VARIABLES
MathLuts *mathLuts = NULL;

//------------------------------------------------------------------- FUNCTIONS

/**
 * Initialize method
 */
void mathInit(void)
{
	double val = 0;
	mathLuts = new MathLuts;
	// Create cos and sin tables
	for(int i = 0; i < 360; i++, val += RAD)
	{
		mathLuts->sin[i] = (float)sin(val);
		mathLuts->cos[i] = (float)cos(val);
	}
}

/**
 * This function should be called when the use of the math library is no
 * necessary.
 */
void mathDeinit(void)
{
	if(mathLuts) {
		delete mathLuts;
		mathLuts = NULL;
	}
}
