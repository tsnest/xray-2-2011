////////////////////////////////////////////////////////////////////////////
//	Module 		: vec3f_inline.h
//	Created 	: 24.08.2006
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : float vector with 3 components inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_VEC3F_INLINE_H_INCLUDED
#define CS_CORE_VEC3F_INLINE_H_INCLUDED

#define VEC	cs::core::math::vec3f

inline float& VEC::operator[]				(unsigned int const i) const
{
	ASSERT				(i < 3);
	return				((float*)&x)[i];
}

inline VEC& VEC::set						(float const _x, float const _y, float const _z)
{
	x					= _x;
	y					= _y;
	z					= _z;

	return				*this;
}

inline VEC& VEC::add						(float const value)
{
	x					+= value;
	y					+= value;
	z					+= value;

	return				*this;
}

inline VEC& VEC::add						(VEC const& vector)
{
	x					+= vector.x;
	y					+= vector.y;
	z					+= vector.z;

	return				*this;
}

inline VEC& VEC::add						(VEC const& _0, VEC const& _1)
{
	x					= _0.x + _1.x;
	y					= _0.y + _1.y;
	z					= _0.z + _1.z;

	return				*this;
}

inline VEC& VEC::add						(VEC const& _0, float const _1)
{
	x					= _0.x + _1;
	y					= _0.y + _1;
	z					= _0.z + _1;

	return				*this;
}

inline VEC& VEC::sub						(float const value)
{
	x					-= value;
	y					-= value;
	z					-= value;

	return				*this;
}

inline VEC& VEC::sub						(VEC const& vector)
{
	x					-= vector.x;
	y					-= vector.y;
	z					-= vector.z;

	return				*this;
}

inline VEC& VEC::sub						(VEC const& _0, VEC const& _1)
{
	x					= _0.x - _1.x;
	y					= _0.y - _1.y;
	z					= _0.z - _1.z;

	return				*this;
}

inline VEC& VEC::sub						(VEC const& _0, float const _1)
{
	x					= _0.x - _1;
	y					= _0.y - _1;
	z					= _0.z - _1;

	return				*this;
}

inline VEC& VEC::mul						(float const value)
{
	x					*= value;
	y					*= value;
	z					*= value;

	return				*this;
}

inline VEC& VEC::mul						(VEC const& vector)
{
	x					*= vector.x;
	y					*= vector.y;
	z					*= vector.z;

	return				*this;
}

inline VEC& VEC::mul						(VEC const& _0, VEC const& _1)
{
	x					= _0.x*  _1.x;
	y					= _0.y*  _1.y;
	z					= _0.z*  _1.z;

	return				*this;
}

inline VEC& VEC::mul						(VEC const& _0, float const _1)
{
	x					= _0.x*  _1;
	y					= _0.y*  _1;
	z					= _0.z*  _1;

	return				*this;
}

inline VEC& VEC::div						(float const value)
{
	ASSERT				(!fp_zero(value));
	float				multiplier = 1.f/value;
	return				mul(multiplier);
}

inline VEC& VEC::div						(VEC const& vector)
{
	ASSERT				(!fp_zero(vector.x));
	x					/= vector.x;

	ASSERT				(!fp_zero(vector.y));
	y					/= vector.y;

	ASSERT				(!fp_zero(vector.z));
	z					/= vector.z;

	return				*this;
}

inline VEC& VEC::div						(VEC const& _0, VEC const& _1)
{
	ASSERT				(!fp_zero(_1.x));
	x					= _0.x / _1.x;

	ASSERT				(!fp_zero(_1.y));
	y					= _0.y / _1.y;

	ASSERT				(!fp_zero(_1.z));
	z					= _0.z / _1.z;

	return				*this;
}

inline VEC& VEC::div						(VEC const& _0, float const _1)
{
	ASSERT				(!fp_zero(_1));
	float				multiplier = 1.f/_1;
	return				mul(_0,multiplier);
}

inline VEC& VEC::mad						(VEC const& point, VEC const& direction, float const multiplier)
{
	x					= point.x + direction.x*multiplier;
	y					= point.y + direction.y*multiplier;
	z					= point.z + direction.z*multiplier;

	return				*this;
}

inline VEC& VEC::abs						(VEC const& vector)
{
	x					= _abs(vector.x);
	y					= _abs(vector.y);
	z					= _abs(vector.z);

	return				*this;
}

inline VEC& VEC::min						(VEC const& vector)
{
	x					= _min(x,vector.x);
	y					= _min(y,vector.y);
	z					= _min(z,vector.z);

	return				*this;
}

inline VEC& VEC::max						(VEC const& vector)
{
	x					= _max(x,vector.x);
	y					= _max(y,vector.y);
	z					= _max(z,vector.z);

	return				*this;
}

inline VEC& VEC::clamp						(VEC const& low, VEC const& high)
{
	::clamp				(x,low.x,high.x);
	::clamp				(y,low.y,high.y);
	::clamp				(z,low.z,high.z);

	return				*this;
}

inline VEC& VEC::negate						()
{
	x					= -x;
	y					= -y;
	z					= -z;
	return				*this;
}

inline VEC& VEC::set_angles_hp				(float const heading, float const pitch)
{
	sine_cosine			h(heading), p(pitch);

    x					= -p.cosine*h.sine;
    y					= p.sine;
    z					= p.cosine*h.cosine;

	return				*this;
}

inline void VEC::get_angles_hp				(float& heading, float& pitch) const
{
	ASSERT				(!fp_zero(magnitude_sqr()));

	if (fp_zero(x) && fp_zero(z)) {
		heading			= 0.f;
		if (y > 0.f) {
			pitch		= PI_DIV_2;
			return;
		}

		pitch			= -PI_DIV_2;
		return;
	}

	pitch				= atanf(y/_sqrt(_sqr(x) + _sqr(z)));

	if (fp_zero(z)) {
		if (x > 0.f) {
			heading		= -PI_DIV_2;
			return;
		}
		
		heading			= PI_DIV_2;
		return;
	}

	if (z < 0.f) {
		heading			= -(atanf(x/z) - PI);
		return;
	}

	heading				= -atanf(x/z);
}

inline float VEC::get_angles_h				() const
{
	ASSERT				(!fp_zero(magnitude_sqr()));

	if (fp_zero(x) && fp_zero(z))
		return			0.f;

	if (fp_zero(z)) {
		if (x > 0.f)
			return		-PI_DIV_2;

		return			PI_DIV_2;
	}

	if (z < 0.f)
		return			-(atanf(x/z) - PI);

	return				-atanf(x/z);
}

inline float VEC::get_angles_p				() const
{
	ASSERT				(!fp_zero(magnitude_sqr()));

	if (fp_zero(x) && fp_zero(z)) {
		if (y > 0.f)
			return		PI_DIV_2;

		return			-PI_DIV_2;
	}

	return				atanf(y/_sqrt(_sqr(x) + _sqr(z)));
}

inline VEC& VEC::normalize					()
{
	return				div(magnitude());
}

inline VEC& VEC::normalize_safe				()
{
	float				magnitude = this->magnitude();
	if (!fp_zero(magnitude))
		return			div(magnitude);

	set					(0.f,0.f,1.f);
	return				*this;
}

inline float VEC::magnitude_sqr				() const
{
	return				_sqr(x) + _sqr(y) + _sqr(z);
}

inline float VEC::magnitude					() const
{
	return				_sqrt(magnitude_sqr());
}

inline float VEC::distance_to_sqr			(VEC const& vector) const
{
	return				_sqr(x - vector.x) + _sqr(y - vector.y) + _sqr(z - vector.z);
}

inline float VEC::distance_to				(VEC const& vector) const
{
	return				_sqrt(distance_to_sqr(vector));
}

inline VEC& VEC::cross_product				(VEC const& _0, VEC const& _1)
{
	x 					= _0.y*_1.z - _0.z*_1.y;
	y 					= _0.z*_1.x - _0.x*_1.z;
	z 					= _0.x*_1.y - _0.y*_1.x;

	return				*this;
}

inline float VEC::dot_product				(VEC const& vector) const
{
	return				x*vector.x + y*vector.y + z*vector.z;
}

inline VEC& VEC::create_normal				(VEC const& _0, VEC const& _1, VEC const& _2)
{
	vec3f				v01;
	v01.sub				(_1, _0);

	vec3f				v12;
	v12.sub				(_2, _1);

	cross_product		(v01, v12);
	normalize_safe		();

	return				*this;
}

inline bool VEC::similar					(VEC const& vector, float const epsilon) const
{
	return				(
		fp_similar(x, vector.x, epsilon) &&
		fp_similar(y, vector.y, epsilon) &&
		fp_similar(z, vector.z, epsilon)
	);
}

inline bool VEC::similar					(VEC const& vector, vec3f const& epsilon) const
{
	return				(
		fp_similar(x, vector.x, epsilon.x) &&
		fp_similar(y, vector.y, epsilon.y) &&
		fp_similar(z, vector.z, epsilon.z)
	);
}

#undef VEC

#endif // #ifndef CS_CORE_VEC3F_INLINE_H_INCLUDED