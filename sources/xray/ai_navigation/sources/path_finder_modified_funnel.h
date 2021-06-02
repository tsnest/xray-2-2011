////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PATH_FINDER_MODIFIED_FUNNEL_H_INCLUDED
#define PATH_FINDER_MODIFIED_FUNNEL_H_INCLUDED

#include "navigation_mesh_types.h"

namespace xray {
namespace ai {
namespace navigation {

class path_finder_modified_funnel : boost::noncopyable {
public:
	typedef debug::vector< u32 >		channel_type;
	typedef debug::vector< float3 >		path_type;

public:
					path_finder_modified_funnel ( 
						triangles_mesh_type const&	triangles_mesh, 
						channel_type	const&	channel,
						float3			const&	start,
						float3			const&	goal,
						float			const	radius,
						path_type&				path
					);

private:

	enum enum_vertex_type {
		vertex_type_left,
		vertex_type_right,
		vertex_type_point
	}; // enum enum_vertex_type

	struct funnel {
		funnel( float3 const& v, enum_vertex_type t ) :
			vertex( v ),
			type( t )
		{
		}

		float3 vertex;
		enum_vertex_type type;
	}; // struct funnel
	typedef deque< funnel >	funnel_type;

private:
	float3			extract_non_edge_vertex( u32 triangle_id0, u32 triangle_id1 );
	u32				get_similar_edge ( u32 triangle_id0, u32 triangle_id1 );
	void			add( float3 const& vertex, enum_vertex_type type );
	void			add_path_vertex( float3 const& vertex );
	void			build_path( float height );

	std::pair< float3, float3 > compute_wedge ( 
		float3 const center0, 
		enum_vertex_type type0, 
		float3 const center1, 
		enum_vertex_type type1 
	);

private:
	triangles_mesh_type	const&	m_triangles_mesh;
	float						m_radius;
	funnel						m_funnel_apex;
	funnel_type					m_funnel;
	funnel_type					m_intermediate_path;
	path_type&					m_path;
	channel_type		const&	m_channel;

}; // class path_find_modified_funnel

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef PATH_FINDER_MODIFIED_FUNNEL_H_INCLUDED