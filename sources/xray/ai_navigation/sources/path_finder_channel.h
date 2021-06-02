////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PATH_FINDER_CHANNEL_H_INCLUDED
#define PATH_FINDER_CHANNEL_H_INCLUDED

#include "navigation_mesh_types.h"
#include "path_finder_funnel.h"

namespace xray {
namespace ai {
namespace navigation {

class path_finder_channel : boost::noncopyable {
public:
				path_finder_channel (
						triangles_mesh_type const& triangles_mesh,
						path_finder_funnel::channel_type& channel,
						u32 const start_vertex_id,
						u32 const target_vertex_id,
						float3 const& start_position,
						float3 const& target_position,
						float const agent_radius
					);

private:
	triangles_mesh_type	const&	m_triangles_mesh;
}; // class path_find_channel

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef PATH_FINDER_CHANNEL_H_INCLUDED