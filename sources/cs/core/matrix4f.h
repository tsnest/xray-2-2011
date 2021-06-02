////////////////////////////////////////////////////////////////////////////
//	Module 		: matrix4f.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : float matrix with 4x4 components
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_MATRIX4F_H_INCLUDED
#define CS_CORE_MATRIX4F_H_INCLUDED

#ifdef CS_CORE_BUILDING
#	include <cs/gtl/script_export_macroses.h>
#endif // #ifdef CS_CORE_BUILDING

struct matrix4f {
public:
#pragma warning(push)
#pragma warning(disable:4201)
	union {
		struct {
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};
		struct {
			vec4f i;
			vec4f j;
			vec4f k;
			vec4f c;
		};
#pragma warning(pop)
	};

public:
	inline matrix4f& identity				();

	inline matrix4f& set_angles_xyz			(float const x, float const y, float const z);
	inline matrix4f& set_angles_xyz			(vec3f const& angles);

	inline matrix4f& get_angles_xyz			(float& x, float& y, float& z);

	inline matrix4f& make_scale				(float const x, float const y, float const z);
	inline matrix4f& make_scale				(vec3f const& scale);

	inline matrix4f& make_translate			(float const x, float const y, float const z);
	inline matrix4f& make_translate			(vec3f const& translation);

	inline vec3f&	transform_vector		(vec3f& result, vec3f const& origin) const;
	inline vec3f&	transform_direction	(vec3f& result, vec3f const& origin) const;
	
	inline matrix4f& mul_43					(matrix4f const& A, matrix4f const& B);
	inline matrix4f& mul_44					(matrix4f const& A, matrix4f const& B);
	
	inline float	determinant_33			() const;
	inline float	invert_determinant_33	() const;
	inline matrix4f& invert_43				(matrix4f const& matrix);
#ifdef CS_CORE_BUILDING
	CS_DECLARE_SCRIPT_EXPORT_FUNCTION
#endif // #ifdef CS_CORE_BUILDING
};

#ifdef CS_CORE_BUILDING
	CS_TYPE_LIST_PUSH_FRONT(matrix4f)
#	undef CS_SCRIPT_EXPORT_TYPE_LIST
#	define CS_SCRIPT_EXPORT_TYPE_LIST CS_SAVE_TYPE_LIST(matrix4f)
#endif // #ifdef CS_CORE_BUILDING

#include <cs/core/matrix4f_inline.h>

#endif // #ifndef CS_CORE_MATRIX4F_H_INCLUDED