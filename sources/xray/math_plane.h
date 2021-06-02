////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_MATH_PLANE_H_INCLUDED
#define XRAY_MATH_PLANE_H_INCLUDED

namespace xray {
namespace math {

class XRAY_CORE_API plane {
public:
	typedef float		type;

public:
#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:4201)
#endif // #if defined(_MSC_VER)
	union {
		struct {
			float3_pod	normal;
			type		d;
		};
		struct {
			float4_pod	vector;
		};
	};
#if defined(_MSC_VER)
#	pragma warning(pop)
#endif // #if defined(_MSC_VER)
	inline				plane					( );
	inline				plane					( float3_pod const& normal, type const distance );
	inline	bool		similar					( plane const& other ) const;
	inline	float3		project					( float3 const& vertex ) const;
	inline	type		classify				( float3 const& vertex ) const;
	inline	type		distance				( float3 const& vertex ) const;
	inline	plane&		normalize				( );
	inline	bool		intersect_ray			( float3 const& position, float3 const& direction, float& distance ) const;
	inline	bool		intersect_ray			( float3 const& position, float3 const& direction, float3& intersection_position ) const;
	inline	bool		intersect_segment		( xray::math::float3 const& first, xray::math::float3 const& second, xray::math::float3& intersection_position ) const;
	inline	plane&		transform				( float4x4 const& right );

	#ifndef MASTER_GOLD
	inline	bool		valid					( ) const;
	#endif
}; // class plane

inline	plane			transform				( plane const& left, float4x4 const& right );

inline	plane			create_plane_normalized	( float3 const& normalized_normal, float3 const& point_on_plane );
inline	float3	get_plane_normal_non_normalized	( float3 const& first, float3 const& second, float3 const& third );
inline	float3			get_plane_normal		( float3 const& first, float3 const& second, float3 const& third );
inline	plane			create_plane			( float3 const& first, float3 const& second, float3 const& third );
inline	plane			create_plane_precise	( float3 const& first, float3 const& second, float3 const& third );
inline	plane			create_plane			( float3 const ( &vertices )[ 3 ] );
inline	plane			create_plane_precise	( float3 const ( &vertices )[ 3 ] );

} // namespace math
} // namespace xray

#include <xray/math_plane_inline.h>

#endif // #ifndef XRAY_MATH_PLANE_H_INCLUDED