////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef BSP_TREE_H_INCLUDED
#define BSP_TREE_H_INCLUDED
#include "bsp_tree_triangle.h"
#include "generated_portal.h"
#include "sectors_generator.h"

namespace xray {

namespace render {
namespace debug {
	class renderer;
} //namespace debug
} // namespace render
namespace model_editor {

class bsp_tree_node;
class bsp_tree {
public:
	static bool						ms_draw_extracted_portals;
									bsp_tree				( );
									~bsp_tree				( );
	void							generate_bsp_tree		( math::float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count );
	bool							is_created				() const { return m_bsp_ready && m_mesh_ready; }
	void							generate_portals		();
	bool							are_portals_generated	() const { return m_portals_generated; }
	void							fill_tree_view			( System::Windows::Forms::TreeView^ tree_view );
	void							set_selected_node		( bsp_tree_node const* selected_node );
	void							render					( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	math::aabb						get_aabb				() const;
	void							serialise_selected_node ( configs::lua_config_ptr ) const;
	void							serialise_whole_tree	( configs::lua_config_ptr ) const;
	void							serialise_triangles_mesh( configs::lua_config_ptr config ) const;
	void							restore_triangles_mesh	( configs::binary_config_ptr const config );
	void							restore					( configs::binary_config_ptr );
	generated_portals_type const&	get_portals				() const { return m_portals; }
	void							reset					();
private:
	void					draw_generated_portals  ( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					remove_portal_triangles_coincident_to_geomertry( triangle_ids_type& portal_triangles );
	bsp_tree_node*			m_root;
	bsp_tree_node const*	m_selected_node;
	bool					m_portals_generated;
	triangles_mesh_type		m_triangle_mesh;
	u32						m_bsp_tree_triangle_count;
	generated_portals_type	m_portals;
	u32						m_initial_vertex_count;
	u32						m_initial_index_count;
	bool					m_bsp_ready;
	bool					m_mesh_ready;
}; // class bsp_tree
} // namespace model_editor
} // namespace xray

#endif // #ifndef BSP_TREE_H_INCLUDED