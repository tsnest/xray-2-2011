////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SECTORS_GENERATOR_H_INCLUDED
#define SECTORS_GENERATOR_H_INCLUDED
#include "bsp_tree_triangle.h"
#include "spatial_sector.h"
#include "triangles_mesh_utilities.h"
#include "sector_generator_portal.h"
namespace xray {
namespace model_editor {

class sectors_generator : private core::noncopyable
{
public:
	static bool	ms_draw_delimited_edges;
	static bool ms_draw_sectors;
	static bool ms_draw_cuboids;
	static bool ms_draw_all_portals;
	static bool ms_draw_hanged_portals;
								sectors_generator			( math::float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count );
	void						add_portal					( sector_generator_portal const& portal );
	void						generate_sectors			();
	void						render						( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	triangles_mesh_type&		get_mesh					() { return m_mesh; }
	void						merge_small_sectors			( float min_square );
	void						fill_property_container		( xray::editor::wpf_controls::property_container^ cont );
	void						set_active_sector			( u32 sector_id, bool active );
	void						set_active_portal			( u32 portal_id, bool active );
private:
	struct bad_adjacency
	{
		bad_adjacency( u32 triangle, u8 edge )
			:triangle_id( triangle ), edge_id( edge ){}
		u32 triangle_id;
		u8	edge_id;
	};
	typedef debug::vector<bad_adjacency> bad_adjacencies_type;
	typedef associative_vector< edge, bad_adjacencies_type, vector, edge_less > edge_to_bad_adjacencies_type;
private:
	void						initialize_delimited_edges	();
	void						add_triangle_to_sector		( u32 triangle_id, spatial_sector& sector );
	void						split_triangle_by_portal	( u32 triangle_id, sector_generator_portal const& p );
	void						check_coplanar_edges		( u32 triangle_id, sector_generator_portal const& p );
	bool						look_for_incorrect_adjacency();
	void						fix_incorrect_adjacency		( edge_to_bad_adjacencies_type const& bad_adjacencies );
	void						test_are_portals_hanged		();
	void						merge_portalless_sectors	();
	u32							get_best_neighbour_to_merge	( u32 sector_id );
	void						remove_empty_sectors		();
	typedef debug::vector<u32> sector_ids_type;
	sector_ids_type				get_portalless_sectors		() const;
	triangles_mesh_type		m_mesh;
	edge_to_u32_type	m_portal_edges;
	typedef debug::vector<math::float4x4> matrices_type;
	matrices_type			m_matrices_for_cuboid;
	typedef associative_vector<u32, spatial_sector, vector> spatial_sectors_type;
	spatial_sectors_type	m_sectors;
	triangle_ids_type		m_triangles;
	typedef associative_vector<u32, sector_generator_portal, vector> portals_type;
	portals_type			m_portals;
	edge_to_bad_adjacencies_type m_bad_adjacencies;
	u32						m_active_sector;
	u32						m_active_portal;
}; // class sectors_generator
} // namespace model_editor
} // namespace xray

#endif // #ifndef SECTORS_GENERATOR_H_INCLUDED