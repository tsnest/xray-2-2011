////////////////////////////////////////////////////////////////////////////
//	Module 		: vec2f_inline.h
//	Created 	: 25.08.2006
//  Modified 	: 25.08.2006
//	Author		: Dmitriy Iassenev
//	Description : float vector with 2 components inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef VEC2F_INLINE_H_INCLUDED
#define VEC2F_INLINE_H_INCLUDED

inline float& vec2f::operator[] (u32 const i) const
{
	ASSERT			(i < 2, "invalid index");
	return			((float*)&x)[i];
}

inline vec2f& vec2f::set		(float const _x, float const _y)
{
	x				= _x;
	y				= _y;

	return			*this;
}

inline vec2f& vec2f::add			(float const value)
{
	x				+= value;
	y				+= value;

	return			*this;
}

inline vec2f& vec2f::add			(vec2f const& vector)
{
	x				+= vector.x;
	y				+= vector.y;

	return			*this;
}

inline vec2f& vec2f::add			(vec2f const& _0, vec2f const& _1)
{
	x				= _0.x + _1.x;
	y				= _0.y + _1.y;

	return			*this;
}

inline vec2f& vec2f::add			(vec2f const& _0, float const _1)
{
	x				= _0.x + _1;
	y				= _0.y + _1;

	return			*this;
}

inline vec2f& vec2f::sub			(float const value)
{
	x				-= value;
	y				-= value;

	return			*this;
}

inline vec2f& vec2f::sub			(vec2f const& vector)
{
	x				-= vector.x;
	y				-= vector.y;

	return			*this;
}

inline vec2f& vec2f::sub			(vec2f const& _0, vec2f const& _1)
{
	x				= _0.x - _1.x;
	y				= _0.y - _1.y;

	return			*this;
}

inline vec2f& vec2f::sub			(vec2f const& _0, float const _1)
{
	x				= _0.x - _1;
	y				= _0.y - _1;

	return			*this;
}

inline vec2f& vec2f::mul			(float const value)
{
	x				*= value;
	y				*= value;

	return			*this;
}

inline vec2f& vec2f::mul			(vec2f const& vector)
{
	x				*= vector.x;
	y				*= vector.y;

	return			*this;
}

inline vec2f& vec2f::mul			(vec2f const& _0, vec2f const& _1)
{
	x				= _0.x*  _1.x;
	y				= _0.y*  _1.y;

	return			*this;
}

inline vec2f& vec2f::mul			(vec2f const& _0, float const _1)
{
	x				= _0.x*  _1;
	y				= _0.y*  _1;

	return			*this;
}


inline vec2f& vec2f::div			(float const value)
{
	ASSERT			(!fp_zero(value), "division by zero");
	float			multiplier = 1.f/value;
	return			mul(multiplier);
}

inline vec2f& vec2f::div			(vec2f const& vector)
{
	ASSERT			(!fp_zero(vector.x), "division by zero");
	x				/= vector.x;

	ASSERT			(!fp_zero(vector.y), "division by zero");
	y				/= vector.y;

	return			*this;
}

inline vec2f& vec2f::div			(vec2f const& _0, vec2f const& _1)
{
	ASSERT			(!fp_zero(_1.x), "division by zero");
	x				= _0.x / _1.x;

	ASSERT			(!fp_zero(_1.y), "division by zero");
	y				= _0.y / _1.y;

	return			*this;
}

inline vec2f& vec2f::div			(vec2f const& _0, float const _1)
{
	ASSERT			(!fp_zero(_1), "division by zero");
	float			multiplier = 1.f/_1;
	return			mul(_0,multiplier);
}

inline vec2f& vec2f::mad			(vec2f const& point, vec2f const& direction, float const multiplier)
{
	x				= point.x + direction.x*multiplier;
	y				= point.y + direction.y*multiplier;

	return			*this;
}

inline vec2f& vec2f::normalize		()
{
	return			div(magnitude());
}

inline bool vec2f::similar			(vec2f const& vector, float const epsilon) const
{
	return			(
						fp_similar(x,vector.x,epsilon) &&
						fp_similar(y,vector.y,epsilon)
					);
}

inline float vec2f::dot_product		(vec2f const& vector) const
{
	return			x*vector.x + y*vector.y;
}

inline float vec2f::cross_product	(vec2f const& vector) const
{
	return			y*vector.x - x*vector.y;
}

inline float vec2f::distance_to_sqr	(vec2f const& vector) const
{
	return			_sqr(x - vector.x) + _sqr(y - vector.y);
}

inline float vec2f::distance_to		(vec2f const& vector) const
{
	return			_sqrt(distance_to_sqr(vector));
}

inline float vec2f::geth			() const
{
	if (fp_zero(y))  {
		if (fp_zero(x))
			return	0.f;

		if ((x > 0.f))
			return	-PI_DIV_2;

		return		PI_DIV_2;
	}

	if (y < 0.f)
		return		-(atanf(x/y) - PI);
		
	return			-atanf(x/y);
}

inline float vec2f::magnitude_sqr	() const
{
	return			_sqr(x) + _sqr(y);
}

inline float vec2f::magnitude		() const
{
	return			_sqrt(magnitude_sqr());
}

#endif // #ifndef VEC2F_INLINE_H_INCLUDED