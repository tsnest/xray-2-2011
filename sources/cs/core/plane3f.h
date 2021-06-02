////////////////////////////////////////////////////////////////////////////
//	Module 		: plane3f.h
//	Created 	: 28.08.2006
//  Modified 	: 28.08.2006
//	Author		: Dmitriy Iassenev
//	Description : plane class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_PLANE3F_H_INCLUDED
#define CS_CORE_PLANE3F_H_INCLUDED

struct CS_CORE_API plane3f : public vec4f {
public:
	inline	vec3f&		n					();
	inline	vec3f const&n					() const;
	inline	plane3f&	build				(vec3f const& _0, vec3f const& _1, vec3f const& _2);
	inline	plane3f&	build				(vec3f const& position, vec3f const& normal);
	inline	float		signed_distance		(vec3f const& position) const;
	inline	u16			compress_normal		() const;
	inline	void		decompress_normal	(u16 const normal);
	inline	bool		intersect			(vec3f const& position, vec3f const& direction, vec3f& result) const;
};

CS_CORE_API u16			compress_normal		(vec3f const& normal);
CS_CORE_API void		decompress_normal	(u16 const normal, vec3f& result);

#include <cs/core/plane3f_inline.h>

#endif // #ifndef CS_CORE_PLANE3F_H_INCLUDED