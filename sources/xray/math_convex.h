////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MATH_CONVEX_H_INCLUDED
#define MATH_CONVEX_H_INCLUDED

#include <xray/math_aabb_plane.h>

namespace xray {
namespace math {


class XRAY_CORE_API convex {
public:
	struct cache
	{
		typedef std::vector<math::float3>	vertices_type;
		typedef std::pair<u32, u32>			edge;
		typedef std::vector< edge >			edges_type;
	#pragma warning ( push )
	#pragma warning ( disable : 4251 )
		vertices_type	vertices;
		edges_type		edges;
	#pragma warning ( pop )
		float bounding_radius;
	};
	
	struct plane_similar : public std::binary_function< xray::math::aabb_plane const&, xray::math::plane const&, bool >
	{
		plane_similar( float normal_epsilon = xray::math::epsilon_3, float distance_epsilon = xray::math::epsilon_3 ):
		normal_e( normal_epsilon ),
		distance_e( distance_epsilon )
		{
		}
		bool operator() ( xray::math::aabb_plane const& left, xray::math::plane const& right ) const
		{
			return xray::math::is_similar( left.plane.normal | right.normal, 1.0f, normal_e ) && xray::math::is_similar( left.plane.d, right.d, distance_e );
		}
		float normal_e;
		float distance_e;
	};
	enum {
		msc_max_plane_count = 32,
	};
	typedef fixed_vector<aabb_plane, msc_max_plane_count>	planes_type;
	typedef fixed_vector<u32, msc_max_plane_count * 2>		adjacencies_type;
public:
								convex			( );
	explicit					convex			( aabb const& bbox );
								convex			( planes_type const& planes, adjacencies_type const& adjacency );
	void						split			( plane const& p, cache const& c, convex& positive, convex& negative ) const;
	intersection				test_inexact	( aabb const& aabb) const;
	bool						inside			( math::float3 const& point ) const;
	void						fill_cache		( cache& c ) const;
	bool						empty			( ) const { return m_planes.empty(); }
	planes_type	 const&			get_planes		( ) const { return m_planes; }
	adjacencies_type const&		get_adjacencies	( ) const { return m_adjacency; }
private:
	typedef u64 vertex_flags;
	typedef u8 (original_to_new_plane_id)[msc_max_plane_count];
private:
	static	u32		get_plane_flags							( adjacencies_type const& adjacency_vector );
	static	void	build_original_to_new_adjacency_dict	( u32 adjacency_flags, original_to_new_plane_id& dict );
	static	u32		original_to_new_adjacency				( original_to_new_plane_id const& dict, u32 adjacency );
private:
#pragma warning ( push )
#pragma warning ( disable : 4251 )
	planes_type			m_planes;
	adjacencies_type	m_adjacency;
#pragma warning ( pop )
}; // class convex

} // namespace math
} // namespace xray

#endif // #ifndef MATH_CONVEX_H_INCLUDED