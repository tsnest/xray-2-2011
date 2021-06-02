////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SPATIAL_SECTOR_H_INCLUDED
#define SPATIAL_SECTOR_H_INCLUDED
#include "bsp_tree_triangle.h"

namespace xray {
namespace model_editor {

class spatial_sector {
public:
	static bool	ms_solid_sectors;
	typedef debug::vector<u32> portal_ids_type;
public:
								spatial_sector		();
								spatial_sector		( triangles_mesh_type& mesh, math::color const& c, u32 sector_id );
	void						add_triangle		( u32 triangle_id );
	triangle_ids_type const&	get_triangles		() const { return m_triangles; }
	void						add_portal			( u32 portal_id );
	portal_ids_type const&		get_portals			() const { return m_portal_ids; }
	void						render				( render::scene_ptr const& scene, render::debug::renderer& r ) const;
	u32							get_id				() const { return m_id; }
	float						get_square			() const { return m_square; }
	math::float3				get_mass_center		() const;
	void						merge				( spatial_sector& other );
	bool						is_empty			() const { return m_triangles.empty(); }
private:
	math::color					m_color;
	math::color					m_line_color;
	triangle_ids_type			m_triangles;
	triangles_mesh_type*		m_mesh;
	float						m_square;
	typedef debug::vector<u32> portal_ids_type;
	portal_ids_type				m_portal_ids;
	u32							m_id;
}; // class spatial_sector

} // namespace model_editor
} // namespace xray

#endif // #ifndef SPATIAL_SECTOR_H_INCLUDED