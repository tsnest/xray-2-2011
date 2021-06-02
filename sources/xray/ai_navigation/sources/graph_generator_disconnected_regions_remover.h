////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_DISCONNECTED_REGIONS_REMOVER_H_INCLUDED
#define GRAPH_GENERATOR_DISCONNECTED_REGIONS_REMOVER_H_INCLUDED

#include "navigation_mesh_types.h"

namespace xray {
namespace ai {
namespace navigation {

class graph_generator_disconnected_regions_remover: boost::noncopyable  {
public:
	graph_generator_disconnected_regions_remover (
		triangles_mesh_type& triangles_mesh,
		float min_agent_radius,
		float min_agent_height
	);

private:
	typedef buffer_vector< bool > markers_type;

private:
				float3		calculate_triangle_center( u32 const triangle_id );
				bool		validate_region ( u32 const triangle_id, markers_type& visited_markers );
				void		remove_region ( u32 const triangle_id );

private:
	triangles_mesh_type&			m_navigation_mesh;
	float					const	m_min_agent_radius;
	float					const	m_min_agent_height;

}; // class graph_generator_disconnected_regions_remover

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_DISCONNECTED_REGIONS_REMOVER_H_INCLUDED