////////////////////////////////////////////////////////////////////////////
//	Created 	: 23.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_QUATERNION_H_INCLUDED
#define XRAY_MATH_QUATERNION_H_INCLUDED

/***************************************************************************
	The quaternion module contains basic support for a quaternion object.

	quaternions are an extension of complex numbers that allows an
	expression for rotation that can be easily interpolated.  Quaternions are also
	more numericaly stable for repeated rotations than matrices.


	A quaternion is a 4 element 'vector3'  [w,x,y,z] where:

	q = w + xi + yj + zk
	i*i = -1
	j*j = -1
	k*k = -1
	i*j = -j*i = k
	j*k = -k*j = i
	k*i = -i*k = j
	q' (conjugate) = w - xi - yj - zk
	||q|| (length) = sqrt(q*q') = sqrt(w*w + x*x + y*y + z*z)
	unit quaternion ||q|| == 1; this implies  q' == qinverse
	quaternions are associative (q1*q2)*q3 == q1*(q2*q3)
	quaternions are not commutative  q1*q2 != q2*q1
	qinverse (inverse (1/q) ) = q'/(q*q')

	q can be expressed by w + xi + yj + zk or [w,x,y,z]
	or as in this implementation (s,v) where s=w, and v=[x,y,z]

	quaternions can represent a rotation.  The rotation is an angle t, around a
	unit vector3 u.   q=(s,v);  s= cos(t/2);   v= u*sin(t/2).

	quaternions can apply the rotation to a point.  let the point be p [px,py,pz],
	and let P be a quaternion(0,p).  Protated = q*P*qinverse
	( Protated = q*P*q' if q is a unit quaternion)

	concatenation rotations is similar to matrix concatenation.  given two rotations
	q1 and q2,  to rotate by q1, then q2:  let qc = (q2*q1), then the combined
	rotation is given by qc*P*qcinverse (= qc*P*qc' if q is a unit quaternion)

	multiplication:
	q1 = w1 + x1i + y1j + z1k
	q2 = w2 + x2i + y2j + z2k
	q1*q2 = q3 =
			(w1*w2 - x1*x2 - y1*y2 - z1*z2)     {w3}
	        (w1*x2 + x1*w2 + y1*z2 - z1*y2)i	{x3}
			(w1*y2 - x1*z2 + y1*w2 + z1*x2)j    {y3}
			(w1*z2 + x1*y2 + y1*x2 + z1*w2)k	{z3}

	also,
	q1 = (s1,v1) = [s1,(x1,y1,z1)]
	q2 = (s2,v2) = [s2,(x2,y2,z2)]
	q1*q2 = q3	=	(s1*s2 - dot_product(v1,v2),			{s3}
					(s1*v2 + s2*v1 + cross_product(v1,v2))	{v3}

	interpolation - it is possible (and sometimes reasonable) to interpolate between
	two quaternions by interpolating each component.  This does not quarantee a
	resulting unit quaternion, and will result in an animation with non-linear
	rotational velocity.

	spherical interpolation: (slerp) treat the quaternions as vectors
	find the angle between them (w = arccos(q1 dot q2) ).
	given 0<=t<=1,  q(t) = q1*(sin((1-t)*w)/sin(w) + q2 * sin(t*w)/sin(w).
	since q == -q, care must be taken to rotate the proper way.

	this implementation uses the notation quaternion q = (quatS,quatV)
	  where quatS is a scalar, and quatV is a 3 element vector3.

	***************************************************************************

	Quaternions are really strange mathematical objects, just like complex
	numbers except that instead of just a real and imaginary part, you have
	three imaginary components, so every quaternion is of the form a + bi +
	cj + dk, where i, j, and k when squared equal -1.  The odd thing about
	these numbers is that they don't obey the commutative law of
	multiplication pq != qp if p and q are quaternions.  They're multiplied
	by the distributive law, and by the rules: i^2 = j^2 = k^2 = -1, i*j = k
	= -j*i, j*k = i = -k*j, and k*i = j = -i*k.  For rotations in graphics,
	you're going to be interested in the unit quaternions, quaternions for
	which sqrt(a^2 + b^2 + c^2 + d^2) = 1, as in this form:

	cos(phi/2) + b*sin(phi/2)*i + c*sin(phi/2)*j + d*sin(phi/2)*k

	This corresponds to a rotation of an angle phi about the axis [ b c d ]
	(which is a unit vector3, of course).  A unit quaternion can also be
	thought of as a point on the surface of a four-dimensional hypersphere,
	so if you try to interpolate between two unit quaternions, you can get
	an intermediate rotation.  Gamasutra describes Shoemake's spherical
	linear interpolation method, but I think getting the logarithm of the
	quaternions and performing linear interpolation is easier.  The
	logarithm of a quaternion is given by

	ln(a + bi + cj + dk) = log(sqrt(a^2 + b^2 + c^2 + d^2))
	+ i*b*arctan(r/a)/r
	+ j*c*arctan(r/a)/r
	+ k*d*arctan(r/a)/r

	where r = sqrt(b^2 + c^2 + d^2)

	Note that the first component will always be zero for unit quaternions. 
	Linearly interpolate each of the components of the logarithm of both
	quaternions then perform the quaternion exponential on the result, given
	by

	exp(a + bi + cj + dk) = exp(a)*cos(r) +
	i*exp(a)*b*sin(r)/r +
	j*exp(a)*c*sin(r)/r +
	k*exp(a)*d*sin(r)/r

	where r is the same factor as above.  This finds an intermediate
	rotation.  Now, to actually use the quaternion rotation q on a point
	p=[x y z], you compute the quaternion product s' = qsq^-1 where q is the
	unit quaternion a + bi + cj + dk for the rotation you want, q^-1 is a -
	bi - cj - dk, and s = xi +  yj + zk.  s' will be of the form x'i + y'j +
	z'k, so the rotated point is p'=[x' y' z'].  The quaternion triple
	product above is equivalent to a rotation matrix, as a lot of tedious
	algebra can show.  The proof is left as an exercise for the reader :-).

	For more information, you can check the Gamasutra article referred to
	above, and Section 21.1 and Exercise 21.7 in Foley, et. al.'s "Computer
	Graphics: Principles and Practice".  I had to implement a lot of this
	for my software renderer library...

***************************************************************************/

namespace xray {
namespace math {

class XRAY_CORE_API quaternion {
public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
	union {
		struct {
			float		x;
			float		y;
			float		z;
			float		w;
		};
		struct {
			float4_pod	vector;
		};
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

public:
	inline				quaternion	( );
			explicit	quaternion	( float4x4 matrix );
	inline	explicit	quaternion	( float3 angles );
	inline				quaternion	( float3 const& direction, float angle );
	inline	explicit	quaternion	( float4 const& vector );

//	inline	quaternion&	normalize	( );
	inline	quaternion&	conjugate	( );
	inline	quaternion&	identity	( );

	inline	bool get_axis_and_angle	( float3& axe, float& angle ) const;
	inline	bool		similar		( quaternion const& other, float const epsilob = epsilon_5 ) const;

#ifndef MASTER_GOLD
	inline	bool		valid		( ) const;
	inline	bool		is_unit		( ) const;
#endif // #ifndef MASTER_GOLD
}; // class quaternion

inline	quaternion		operator *	( quaternion const& left, quaternion const& right );
inline	quaternion		operator *	( float left, quaternion const& right );
inline	quaternion		operator *	( quaternion const& left, float right );

inline	void			hemi_flip	( quaternion &q, quaternion const& to );

inline	quaternion		conjugate	( quaternion const& value );
inline	quaternion		logn		( quaternion const& value );
inline	quaternion		exp			( quaternion const& value );

inline	float3			rotate		( float3 const& position, quaternion const& rotation );

XRAY_CORE_API quaternion slerp		( quaternion const& left, quaternion const& right, float time_delta );

} // namespace math
} // namespace xray

#include <xray/math_quaternion_inline.h>

#endif // #ifndef XRAY_MATH_QUATERNION_H_INCLUDED