////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_QUATERNION_INLINE_H_INCLUDED
#define XRAY_MATH_QUATERNION_INLINE_H_INCLUDED

namespace xray {
namespace math {

inline quaternion::quaternion				( )
#ifdef DEBUG
:	vector			( float4() )
#endif // #ifdef DEBUG
{
}

inline quaternion::quaternion				( float4 const& vector ) :
	vector			( vector )
{
//	R_ASSERT		( is_unit() );
}

inline quaternion::quaternion				( float3 angles )
{
	angles			*= .5f;

	sine_cosine x(angles.x), y(angles.y), z(angles.z);

	float const	xsXyc	= x.sine	* y.cosine;
	float const	xcXys	= x.cosine	* y.sine;
	float const	xcXyc	= x.cosine	* y.cosine;
	float const	xsXys	= x.sine	* y.sine;

	this->x			= -	xsXyc * z.cosine	-	xcXys * z.sine;
	this->y			= -	xcXys * z.cosine	+	xsXyc * z.sine;
	this->z			= -	xcXyc * z.sine  	-	xsXys * z.cosine;
	this->w			= -	xcXyc * z.cosine	+	xsXys * z.sine;

	R_ASSERT		( is_unit() );
}

inline quaternion::quaternion				( float3 const& direction, float angle )
{
	w				= cos( angle * .5f );
	vector.xyz()	= direction * sin( angle * .5f );

	R_ASSERT		( is_unit() );
}

inline quaternion& quaternion::conjugate	( )
{
	vector.xyz()	= -vector.xyz();
	R_ASSERT		( is_unit() );
	return			( *this );
}

inline quaternion& quaternion::identity		( )
{
	vector			= float4( 0.f, 0.f, 0.f, 1.f );
	return			( *this );
}

inline bool quaternion::get_axis_and_angle	( float3& axis, float& angle ) const
{
	R_ASSERT		( is_unit() );
	float const s	= vector.xyz().length();
	if ( s > epsilon_7 ) {
		axis		= vector.xyz()*( 1.f / s );
		angle		= 2.f * atan2( s, w );
		return		( true );
	}

	axis			= float3( 0.f, 0.f, 1.f );
	angle			= 0.0f;
	return			( false );
}

inline bool quaternion::similar				( quaternion const& other, float const epsilon ) const
{
	R_ASSERT		( is_unit() );
	R_ASSERT		( other.is_unit() );
	return			( vector.is_similar( other.vector, epsilon ) || vector.is_similar( -float4(other.vector), epsilon ) );
}

#ifndef MASTER_GOLD
inline bool quaternion::is_unit				( ) const
{
	return			( math::is_similar( vector.length( ), 1.f ) );
}

inline bool quaternion::valid				( ) const
{
	R_ASSERT		( is_unit() );
	return			( vector.valid( ) );
}

#endif // #ifndef MASTER_GOLD

inline quaternion operator *				( quaternion const& left, quaternion const& right )
{
//	R_ASSERT		( left.is_unit() );
//?	R_ASSERT		( right.is_unit() );

	quaternion		result;

	result.w		= ( left.w*right.w - left.x*right.x - left.y*right.y - left.z*right.z );
	result.x		= ( left.w*right.x + left.x*right.w + left.y*right.z - left.z*right.y );
	result.y		= ( left.w*right.y - left.x*right.z + left.y*right.w + left.z*right.x );
	result.z		= ( left.w*right.z + left.x*right.y - left.y*right.x + left.z*right.w );

//?	R_ASSERT		( result.is_unit() );
	return			( result );
}

inline quaternion operator *				( float const left, quaternion const& right )
{
	float3			axis;
	float			angle;
	right.get_axis_and_angle( axis, angle );
	return			quaternion( axis, angle*left );
}

inline quaternion operator *				( quaternion const& left, float const right )
{
	return			right * left;
}

inline	void hemi_flip						( quaternion& q, quaternion const& to )
{
	R_ASSERT		( q.is_unit() );
	R_ASSERT		( to.is_unit() );

	if( dot_product ( q.vector, to.vector ) < 0.f )
		q.vector	= -q.vector; 
}

inline quaternion conjugate					( quaternion const& value )
{
	return			quaternion( value ).conjugate( );
}

inline quaternion logn						( quaternion const& value )
{
	R_ASSERT		( value.is_unit() );
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    quaternion result;
    result.w = (float)0.0;

   	if ( abs( value.w ) < (float)1.0 )
    {
		float const		n = value.vector.xyz().squared_length( );
		float const		r = sqrt( n );
		
		float angle	= atan2( r, value.w );
		float sine	= sin(angle);

        if ( abs( sine ) >= epsilon_6 ) //  Math<Real>::ZERO_TOLERANCE = 1.f e-06
        {
            float coeff = angle/sine;

  			result.vector.xyz() = coeff * value.vector.xyz();

            return result;
        }
    }

 	result.vector.xyz() = value.vector.xyz();
	R_ASSERT		( result.is_unit() );
    return result;
}

inline quaternion exp					( quaternion const& value )
{
//?	R_ASSERT		( value.is_unit() );
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    quaternion result;

	float angle = sqrt( value.vector.xyz().length() );

	sine_cosine	sin_cos( angle );

	result.w = sin_cos.cosine;

	if ( abs(sin_cos.sine) >= epsilon_6 )//Math<Real>::ZERO_TOLERANCE
    {
        float coeff = sin_cos.sine/angle;
		result.vector.xyz() = coeff * value.vector.xyz();
    }
    else
       result.vector.xyz() =  value.vector.xyz();

//?	R_ASSERT		( result.is_unit() );
    return result;
}

inline float3 rotate					( float3 const& position, quaternion const& rotation )
{
	return			(rotation * quaternion( float4(position, 0.f) ) * conjugate( rotation )).vector.xyz();
}

} // namespace math
} // namespace xray

#endif // #ifndef XRAY_MATH_QUATERNION_INLINE_H_INCLUDED
