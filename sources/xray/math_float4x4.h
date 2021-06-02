////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_FLOAT4X4_H_INCLUDED
#define XRAY_MATH_FLOAT4X4_H_INCLUDED

namespace xray {
namespace math {

class quaternion;
enum axis_rotation_order{
	rotation_zxy
};

class XRAY_CORE_API float4x4 {
public:
	typedef float		type;

public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
	union {
		struct {
			float4_pod	i,j,k,c;
		};
		float			elements[4][4];
		struct {
			float		e00, e01, e02, e03;
			float		e10, e11, e12, e13;
			float		e20, e21, e22, e23;
			float		e30, e31, e32, e33;
		};
		float4_pod		lines[4];
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)

public:
	inline				float4x4						( );
	inline				float4x4						( float4 const& other_i, float4 const& other_j, float4 const& other_k, float4 const& other_c );
	inline	float4x4&	identity						( );

	inline	float4x4&	operator *=						( float4x4::type value );
	inline	float4x4&	operator /=						( float4x4::type value );

	inline	float3		transform_position				( float3 const& vector ) const;						/// this is 4x3 ransform, for full one call transform4x4
	inline	float3		transform_direction				( float3 const& direction ) const;					/// this is 3x3 ransform, for full one call transform4x4
	inline	float3		transform						( float3 const& vector ) const;

	inline	float4		transform						( float4 const& vector ) const;

	inline	float		determinant4x3					( ) const;
			bool		try_invert						( float4x4 const& other );							/// this is 4x3 invert

	inline	float3		get_scale						( ) const;
	inline	void		set_scale						( float3 const& scale );

			float3		get_angles						( axis_rotation_order const order ) const;
			float3		get_angles_xyz					( ) const;

	#ifndef MASTER_GOLD
	inline	bool		valid							( ) const;
	#endif
}; // struct float4

inline float4x4			operator *						( float4x4 const& left, float4x4::type right );
inline float4x4			operator *						( float4x4::type left, float4x4 const& right );

inline float4x4			operator /						( float4x4 const& left, float4x4::type right );

inline float3			operator *						( float3 const& left, float4x4 const& right );		/// this is 4x3 multiplication, for full one call transform4x4
inline float4			operator *						( float4 const& left, float4x4 const& right );

// 4x3 operations
inline	float4x4		operator *						( float4x4 const& left, float4x4 const& right );	/// this is 4x3 multiplication, for full one call mul4x4
XRAY_CORE_API float4x4	mul4x3							( float4x4 const& left, float4x4 const& right );	/// this is 4x3 multiplication
XRAY_CORE_API float4x4	invert4x3						( float4x4 const& other );							/// this is 4x3 invert, for full one call invert4x4

// 4x4 operations
XRAY_CORE_API float4x4	mul4x4							( float4x4 const& left, float4x4 const& right );	/// this is 4x4 multiplication
XRAY_CORE_API float4x4	invert4x4						( float4x4 const& matrix_to_invert );
XRAY_CORE_API bool		try_invert4x4					( float4x4 const& matrix_to_invert, float4x4& out_result );

inline float4x4			transpose						( float4x4 const& other );

// matrix construction

inline float4x4			create_translation				( float3 const& position );

inline float4x4			create_rotation					( float3 const& angles );
inline float4x4			create_rotation					( float3 const& angles, axis_rotation_order const );
inline float4x4			create_rotation_x				( float4x4::type angle );
inline float4x4			create_rotation_y				( float4x4::type angle );
inline float4x4			create_rotation_z				( float4x4::type angle );
inline float4x4			create_rotation					( float3 const& direction, float3 const& normal );
//inline float4x4			create_rotation					( quaternion const& rotation );
XRAY_CORE_API float4x4	create_rotation					( float3 const& axis, float angle );
inline float4x4			create_matrix					( quaternion const& rotation, float3 const& translation );

inline float4x4			create_scale					( float3 const& scale );

inline float4x4			create_rotation					( quaternion const& q );
inline float4x4			create_matrix					( quaternion const& q, float3 const& position );

XRAY_CORE_API float4x4	create_perspective_projection	( float4x4::type vertical_field_of_view, float4x4::type aspect_ratio, float4x4::type near_plane, float4x4::type far_plane );
XRAY_CORE_API float4x4	create_orthographic_projection	( float4x4::type width, float4x4::type height, float4x4::type near_plane, float4x4::type far_plane );

XRAY_CORE_API float4x4	create_camera_at				( float3 const& from, float3 const& at, float3 const& up );
XRAY_CORE_API float4x4	create_camera_direction			( float3 const& from, float3 const& direction, float3 const& up );


XRAY_CORE_API bool		try_solve_linear_equations_system( float3 const& first, float3 const& second, float3 const& third, float3 const& b, float3& result );
XRAY_CORE_API bool		try_solve_linear_equations_system( float4 const& first,	float4 const& second, float4 const& third, float4 const& fourth, float4 const& b, float4& result );


} // namespace math
} // namespace xray

#include <xray/math_float4x4_inline.h>

#endif // #ifndef XRAY_MATH_FLOAT4X4_H_INCLUDED