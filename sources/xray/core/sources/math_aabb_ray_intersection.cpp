////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

namespace xray {
namespace math {

//http://tog.acm.org/resources/GraphicsGems/gems/RayBox.c
/* 
Fast Ray-Box Intersection
by Andrew Woo
from "Graphics Gems", Academic Press, 1990
*/

intersection	aabb::intersect	( float3 const& origin, float3 const& direction, float3& result ) const
{
	enum{right, left, middle};
	enum {num_dim = 3};
	bool inside = true;
	char quadrant[num_dim];
	register int i;
	int whichPlane;
	float maxT[num_dim];
	float candidatePlane[num_dim];

	/* Find candidate planes; this loop can be avoided if
   	rays cast all from the eye(assume perpsective view) */
	for (i=0; i<num_dim; i++)
		if( origin[i] < min[i] ) {
			quadrant[i] = left;
			candidatePlane[i] = min[i];
			inside = false;
		}else if (origin[i] > max[i]) {
			quadrant[i] = right;
			candidatePlane[i] = max[i];
			inside = false;
		}else	{
			quadrant[i] = middle;
		}

	/* Ray origin inside bounding box */
	if(inside)	{
		result = origin;
		return math::intersection_inside;
	}


	/* Calculate T distances to candidate planes */
	for (i = 0; i < num_dim; i++)
		if (quadrant[i] != middle && direction[i] !=0.)
			maxT[i] = (candidatePlane[i]-origin[i]) / direction[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (i = 1; i < num_dim; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.) return math::intersection_none;
	for (i = 0; i < num_dim; i++)
		if (whichPlane != i) {
			result[i] = origin[i] + maxT[whichPlane] *direction[i];
			if (result[i] < min[i] || result[i] > max[i])
				return math::intersection_none;
		} else {
			result[i] = candidatePlane[i];
		}
	return math::intersection_outside;				/* ray hits box */
}

}
}