////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

using xray::math::cylinder;
using xray::math::float3;
using xray::math::intersection;

u32 cylinder::intersect						( float3 const& position, float3 const& direction, float ( &t )[ 2 ] ) const
{
    float const epsilon	= 1e-12f;

    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    float3 kU, kV, kW = direction;
    generate_orthonormal_basis	(kW,kU,kV);
    float3 kD( kU | direction, kV | direction, kW | direction );

#ifdef DEBUG
	if ( kD.squared_length() <= std::numeric_limits<float>::min())
	{
		LOG_ERROR("direction :%f,%f,%f",direction.x,direction.y,direction.z);
		LOG_ERROR("kU :%f,%f,%f",kU.x,kU.y,kU.z);
		LOG_ERROR("kV :%f,%f,%f",kV.x,kV.y,kV.z);
		LOG_ERROR("kW :%f,%f,%f",kW.x,kW.y,kW.z);
		FATAL("KD is zero");
	}
#endif // #ifdef DEBUG

    float fDLength = kD.normalize_r();
    float fInvDLength = 1.0f/fDLength;
    float3 kDiff = position - center;
    float3 kP( kU | kDiff, kV | kDiff, kW | kDiff );
    float fHalfHeight = 0.5f*height;
	float fRadiusSqr = xray::math::sqr(radius);

    float fInv, fA, fB, fC, fDiscr, fRoot, fT, fT0, fT1, fTmp0, fTmp1;

    if ( xray::math::abs(kD.z) >= 1.0f - epsilon ){
        // line is parallel to cylinder axis
        if ( kP.x*kP.x+kP.y*kP.y <= fRadiusSqr ){
            fTmp0 = fInvDLength/kD.z;
            t[0] = (+fHalfHeight - kP.z)*fTmp0;
            t[1] = (-fHalfHeight - kP.z)*fTmp0;
            return 2;
        }
        else{
            return 0;
        }
    }

    if ( xray::math::abs(kD.z) <= epsilon ){
        // line is perpendicular to axis of cylinder
        if ( xray::math::abs(kP.z) > fHalfHeight ){
            // line is outside the planar caps of cylinder
            return 0;
        }

        fA = kD.x*kD.x + kD.y*kD.y;
        fB = kP.x*kD.x + kP.y*kD.y;
        fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
        fDiscr = fB*fB - fA*fC;
        if ( fDiscr < 0.0f ){
            // line does not intersect cylinder wall
            return 0;
        }
        else if ( fDiscr > 0.0f ){
            fRoot = sqrt(fDiscr);
            fTmp0 = fInvDLength/fA;
            t[0] = (-fB - fRoot)*fTmp0;
            t[1] = (-fB + fRoot)*fTmp0;
            return 2;
        }else{
            t[0] = -fB*fInvDLength/fA;
            return 1;
        }
    }

    // test plane intersections first
    int iQuantity = 0;
    fInv = 1.0f/kD.z;
    fT0 = (+fHalfHeight - kP.z)*fInv;
    fTmp0 = kP.x + fT0*kD.x;
    fTmp1 = kP.y + fT0*kD.y;
    if ( fTmp0*fTmp0 + fTmp1*fTmp1 <= fRadiusSqr )
        t[iQuantity++] = fT0*fInvDLength;

    fT1 = (-fHalfHeight - kP.z)*fInv;
    fTmp0 = kP.x + fT1*kD.x;
    fTmp1 = kP.y + fT1*kD.y;
    if ( fTmp0*fTmp0 + fTmp1*fTmp1 <= fRadiusSqr )
        t[iQuantity++] = fT1*fInvDLength;

    if ( iQuantity == 2 ){
        // line intersects both top and bottom
        return 2;
    }

    // If iQuantity == 1, then line must intersect cylinder wall
    // somewhere between caps in a single point.  This case is detected
    // in the following code that tests for intersection between line and
    // cylinder wall.

    fA = kD.x*kD.x + kD.y*kD.y;
    fB = kP.x*kD.x + kP.y*kD.y;
    fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
    fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f ){
        // line does not intersect cylinder wall
        //ASSERT( iQuantity == 0 );
        return 0;
    }else if ( fDiscr > 0.0f ){
        fRoot = sqrt(fDiscr);
        fInv = 1.0f/fA;
        fT = (-fB - fRoot)*fInv;
        if ( fT0 <= fT1 ){
            if ( fT0 <= fT && fT <= fT1 )
                t[iQuantity++] = fT*fInvDLength;
        }else{
            if ( fT1 <= fT && fT <= fT0 )
                t[iQuantity++] = fT*fInvDLength;
        }

        if ( iQuantity == 2 ){
            // Line intersects one of top/bottom of cylinder and once on
            // cylinder wall.
            return 2;
        }

        fT = (-fB + fRoot)*fInv;
        if ( fT0 <= fT1 ){
            if ( fT0 <= fT && fT <= fT1 )
                t[iQuantity++] = fT*fInvDLength;
        }else{
            if ( fT1 <= fT && fT <= fT0 )
                t[iQuantity++] = fT*fInvDLength;
        }
    }else{
        fT = -fB/fA;
        if ( fT0 <= fT1 ){
            if ( fT0 <= fT && fT <= fT1 )
                t[iQuantity++] = fT*fInvDLength;
        }else{
            if ( fT1 <= fT && fT <= fT0 )
                t[iQuantity++] = fT*fInvDLength;
        }
    }

    return iQuantity;
}


intersection	cylinder::intersect	( float3 const& position, float3 const& direction, float& distance ) const
{
	float			t[2];
    u32 const count	= intersect ( position, direction, t );
	if ( !count )
		return		( intersection_none );

	R_ASSERT_CMP	( count, <=, 2 );
	intersection	inside = intersection_outside;
	bool result		= false;
	for ( u32 i = 0; i < count; ++i ) {
		if ( t[i] < 0.f ) {
			if ( count == 2 )
				inside	= intersection_inside;
			continue;
		}

		if ( t[i] >= distance )
			continue;

		distance	= t[i];
		result		= true;
	}

	if ( !result )
		return		( intersection_none );

	return			( inside );
}