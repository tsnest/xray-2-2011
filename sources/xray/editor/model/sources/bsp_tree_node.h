////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BSP_TREE_NODE_H_INCLUDED
#define BSP_TREE_NODE_H_INCLUDED

#pragma managed(push, off)
#include "bsp_portal_blank.h"
#include "bsp_tree_triangle.h"
#include "bsp_typedefs.h"
#include <xray\math_convex.h>
#pragma managed(pop)

namespace xray {

namespace render {
namespace debug {
	class renderer;
} //namespace debug
} // namespace render

namespace model_editor {

class bsp_tree_node : boost::noncopyable
{
public:
	typedef vector<bsp_portal_blank> portal_blanks_type;
public:
	static float			ms_min_portal_square;
	static float			ms_slimness_threshold;
	static bool				ms_draw_solid;
	static bool				ms_draw_aabb;
	static bool				ms_draw_double_sided;
	static bool				ms_draw_divider;
	static bool				ms_draw_geometry;
	static bool				ms_draw_portal_blanks;
	static bool				ms_draw_portal;
	static bool				ms_draw_outer_edges;
							
	explicit				bsp_tree_node			( triangles_mesh_type& triangle_mesh );
							bsp_tree_node			( triangles_mesh_type& triangle_mesh, triangle_ids_type const& triangle_ids, math::convex const& bounding_convex );
							~bsp_tree_node			( );
	void					build_bsp				( );
	bool					is_leaf					( ) const;
	void					fill_tree_view_node		( System::Windows::Forms::TreeNode^ node );
	void					render					( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					serialise				( configs::lua_config_value& node_config ) const;
	void					restore					( configs::binary_config_value const& node_config );
	math::aabb				get_aabb				() const;
	void					generate_portals		( portal_blanks_type& blanks, triangle_ids_type& portal_triangles );
private:
	void					render					( render::scene_ptr const& scene, render::debug::renderer& r, math::color const& c ) const;
	u32						choose_dividing_polygon	();
	void					draw_polygons			( render::scene_ptr const& scene, render::debug::renderer& r, math::color const& c, bool solid = false, bool double_sided = false ) const;
	void					draw_bounding_convex	( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					draw_divider_triangle	( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					draw_portal_blanks		( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					draw_outer_edges		( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					draw_portals			( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	void					process_triangle		( u32 triangle_id, math::plane const& divider, triangle_ids_type& coplanar_triangles, triangle_ids_type& positive_ids, triangle_ids_type& negative_ids );
	void					process_coplanar_triangle( u32 triangle_id, math::plane const& divider, triangle_ids_type& coplanar_triangles, triangle_ids_type& positive_ids, triangle_ids_type& negative_ids );
	void					preprocess_triangulation_input( math::float3 const& normal, edges_type& edges );
	typedef ai::navigation::delaunay_triangulator delaunay_triangulator;
	typedef	delaunay_triangulator::indices_type triangulator_indices_type;
	void					extract_portal_triangles ( triangulator_indices_type const& indices, edges_type const& edges, input_indices_type const& indices_from_blank, math::plane const& portal_blank_plane );	
	bsp_tree_node*			m_positive;
	bsp_tree_node*			m_negative;
	triangles_mesh_type&	m_mesh;
	triangle_ids_type		m_triangle_ids;
	math::convex			m_convex;
	math::convex::cache		m_convex_cache;
	bool					m_broken;
	u32						m_divider_id;
	u32						m_predicted_spanning;
	u32						m_real_spanning;
	bsp_portal_blank		m_poratal_blank;
	portal_blanks_type		m_portal_blanks;
	triangle_ids_type		m_portal_triangles;
	typedef std::pair<math::float3, math::float3> outer_edge_type;
	typedef debug::vector<outer_edge_type> outer_edges_type;
	outer_edges_type		m_outer_edges;
}; // class bsp_tree_node

value struct bsp_tree_node_wrapper
{
	bsp_tree_node_wrapper( bsp_tree_node* n ):node(n){}
	bsp_tree_node const* node;
};// value struct bsp_tree_node_wrapper

} // namespace model_editor
} // namespace xray

#endif // #ifndef BSP_TREE_NODE_H_INCLUDED