////////////////////////////////////////////////////////////////////////////
//	Module 		: plane3f.cpp
//	Created 	: 29.08.2006
//  Modified 	: 29.08.2006
//	Author		: Dmitriy Iassenev
//	Description : plane class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/plane3f.h>

static const u16 sign_mask		= 0xe000;
static const u16 xsign_mask		= 0x8000;
static const u16 ysign_mask		= 0x4000;
static const u16 zsign_mask		= 0x2000;
static const u16 top_mask		= 0x1f80;	
static const u16 bottom_mask	= 0x007f;	

static float data[8192];

u16 compress_normal	(vec3f const& normal)
{
	vec3f			temp = normal;

	u16				result = 0;

	if (temp.x < 0)
		result		|= xsign_mask;
	// do not move the next line under the if
	// since the number -0.000000.f is not less
	// than zero, but i_floor(-0.000000.f) == -1
	temp.x			= _abs(temp.x);

	if (temp.y < 0)
		result		|= ysign_mask;
	// the same as for the temp.x
	temp.y			= _abs(temp.y);

	if (temp.z < 0)
		result		|= zsign_mask;

	// the same as for the temp.x
	temp.z			= _abs(temp.z);

	float			w = 126.f/(temp.x + temp.y + temp.z);
	int				xbits = i_floor(temp.x*  w);
	int				ybits = i_floor(temp.y*  w);
	
	if (xbits >= 64) {
		xbits		= 127 - xbits; 
		ybits		= 127 - ybits;
	}

	result			|= (xbits << 7);		   
	result			|= ybits;

	return			result;
}

static void initialize_compressor	()
{
	for (int idx = 0; idx < 8192; ++idx) {
		int			xbits = idx >> 7;
		int			ybits = idx&  bottom_mask;

		if ((xbits + ybits) >= 127) {
			xbits	= 127 - xbits; 
			ybits	= 127 - ybits;
		}
		
		float		x = float(xbits);
		float		y = float(ybits);
		float		z = float(126 - xbits - ybits);
		
		data[idx]	= 1.f/_sqrt(x*x + y*y + z*z);
	}
}

void decompress_normal				(u16 const normal, vec3f& result)
{
	static	bool	initialized = false;
	if (!initialized) {
		initialize_compressor	();
		initialized	= true;
	}

	int				xbits = ((normal&  top_mask ) >> 7 );
	int				ybits = (normal&  bottom_mask);

	
	if ((xbits + ybits) >= 127) {
		xbits		= 127 - xbits; 
		ybits		= 127 - ybits;
	}
	
	float			uvadj = data[normal&  ~sign_mask];
	result.x		= uvadj*float(xbits);
	result.y		= uvadj*float(ybits);
	result.z		= uvadj*float(126 - xbits - ybits);

	
	if (normal&  xsign_mask)
		result.x	= -_abs(result.x);

	if (normal&  ysign_mask)
		result.y	= -_abs(result.y);

	if (normal&  zsign_mask)
		result.z	= -_abs(result.z);
}