////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BSP_PORTAL_BLANK_H_INCLUDED
#define BSP_PORTAL_BLANK_H_INCLUDED

#include <xray/math_convex.h>

namespace xray {
namespace model_editor {

class bsp_portal_blank 
{
public:
	typedef math::convex::cache::vertices_type  vertices_type;
public:
							bsp_portal_blank		();	
	void					initialize				( math::convex const& c, math::convex::cache const& cache, math::plane const& p );
	vertices_type const&	get_vertices			() const { return m_vertices; }
	void					split					( math::plane const& divider, bsp_portal_blank& positive, bsp_portal_blank& negative ) const;
	void					limit_by_positive_part	( math::plane const& p );
	math::plane const&		get_plane				() const { return m_plane; }
	float					get_square				() const { return m_square; }
private:
	void					update_square			();
	vertices_type	m_vertices;
	math::plane		m_plane;
	float			m_square;
}; // class bsp_portal_blank

void draw_portal_blank ( render::scene_ptr const& scene, render::debug::renderer& r, bsp_portal_blank const& blank, math::color const& c );

} // model_editor
} // namespace xray

#endif // #ifndef BSP_PORTAL_POLYGON_H_INCLUDED