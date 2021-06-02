////////////////////////////////////////////////////////////////////////////
//	Module 		: plane3f_inline.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : plane class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_PLANE3F_INLINE_H_INCLUDED
#define CS_CORE_PLANE3F_INLINE_H_INCLUDED

inline vec3f& plane3f::n				()
{
	return				vec3();
}

inline vec3f const& plane3f::n			() const
{
	return				vec3();
}

inline plane3f& plane3f::build			(vec3f const& _0, vec3f const& _1, vec3f const& _2)
{
	vec3f				_10 = vec3f().sub(_0,_1);
	vec3f				_20 = vec3f().sub(_0,_2);
	vec3f&				normal = this->n();
	normal.cross_product(_10,_20);
	normal.normalize	();
	w					= -normal.dot_product(_0);
	return				*this;
}

inline plane3f& plane3f::build			(vec3f const& position, vec3f const& normal)
{
	ASSERT				(fp_similar(normal.magnitude_sqr(),1.f));
	n()					= normal;
	w					= -n().dot_product(position);

	return				*this;
}

inline float plane3f::signed_distance	(vec3f const& position) const
{
	return				n().dot_product(position) + w;
}

inline u16 plane3f::compress_normal		() const
{
	return				::compress_normal(n());
}

inline void plane3f::decompress_normal	(u16 const normal)
{
	::decompress_normal	(normal,n());
}

inline bool plane3f::intersect			(vec3f const& position, vec3f const& direction, vec3f& result) const
{
	float				leg = signed_distance(position);
	float				cos_alpha = n().dot_product(direction);
	if (_abs(cos_alpha) < EPS_7)
		return			false;

	float				hypotenuse = -leg/cos_alpha;
	if (fp_zero(hypotenuse)) {
		result			= position;
		return			true;
	}

	if (hypotenuse < 0.f)
		return			false;

	result.mad			(position, direction, hypotenuse);
	return				true;
}

#endif // #ifndef CS_CORE_PLANE3F_INLINE_H_INCLUDED