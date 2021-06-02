////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GENERATED_PORTAL_H_INCLUDED
#define GENERATED_PORTAL_H_INCLUDED
#include "bsp_typedefs.h"
#include "bsp_tree_triangle.h"
#include "sector_generator_portal.h"

namespace xray {
namespace model_editor {

class generated_portal
{
public:
								generated_portal			( triangles_mesh_type const& mesh, triangle_ids_type const& triangles );	
	float						get_square					() const { return m_square; }
	float						get_slimness				() const { return m_slimness; }
	edges_type const&			get_outer_edges				() const { return m_outer_edges; }
	void						check_hanged				( edges_buffer_type const& edges, indices_buffer_type const& indices, edges_buffer_type const& edges_by_finish );
	void						render						( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	math::plane const&			get_plane					() const { return m_mesh->data[ m_triangles.front() ].plane; }
	u32							get_id						() const { return m_mesh ? m_mesh->data[ m_triangles.front() ].portal_id : bsp_tree_triangle::msc_empty_id; }
	triangles_mesh_type const&	get_mesh					() const { R_ASSERT( m_mesh ); return *m_mesh; }
	sector_generator_portal		to_sector_generator_portal	() const;
private:
	void						update_square_slimness_and_outer_edges();
	triangle_ids_type			m_triangles;
	triangles_mesh_type const*	m_mesh; 
	float						m_square;
	float						m_slimness;
	edges_type					m_outer_edges;
}; // class generated_portal
typedef debug::vector<generated_portal> generated_portals_type;
void extract_portals_from_mesh( triangles_mesh_type& portals_mesh, triangle_ids_type const& triangles, generated_portals_type& portals );
} // namespace model_editor
} // namespace xray

#endif // #ifndef GENERATED_PORTAL_H_INCLUDED