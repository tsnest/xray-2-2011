////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PATH_FIND_FUNNEL_H_INCLUDED
#define PATH_FIND_FUNNEL_H_INCLUDED

#include "graph_generator_types.h"

namespace xray {
namespace ai {
namespace navigation {

class path_find_funnel : boost::noncopyable {
public:
	typedef debug::vector< u32 >		channel_type;
	typedef debug::vector< float3 >		path_type;

public:
					path_find_funnel ( 
						triangles_mesh_type	const&	triangles_mesh, 
						channel_type		const&	channel,
						float3				const&	start,
						float3				const&	goal,
						path_type&					path
					);

private:
	enum enum_vertex_type {
		vertex_type_point,
		vertex_type_left,
		vertex_type_right
	};

private:
	void			add( float3 vertex, enum_vertex_type type);
	float3			extract_non_edge_vertex( u32 triangle_id0, u32 triangle_id1 );
	u32				get_similar_edge ( u32 triangle_id0, u32 triangle_id1 );
	void			add_path_vertex( float3 const& vertex );

private:
	typedef deque< float3 >		funnel_type;

	funnel_type					m_funnel;
	float3						m_funnel_apex;
	path_type&					m_path;
	channel_type		const&	m_channel;	
	triangles_mesh_type	const&	m_triangles_mesh;

}; // class path_find_funnel

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef PATH_FIND_FUNNEL_H_INCLUDED