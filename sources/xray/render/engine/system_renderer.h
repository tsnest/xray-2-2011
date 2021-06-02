////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SYSTEM_RENDERER_H_INCLUDED
#define SYSTEM_RENDERER_H_INCLUDED

#include <xray/render/base/common_types.h>
#include "xray/render/core/vertex_stream.h"
#include "xray/render/core/index_stream.h"
#include "xray/render/core/res_effect.h"
#include "xray/render/core/res_geometry.h"
#include "scene_render.h"

namespace xray {
namespace render_dx10 {

namespace vertex_formats {
struct TL;
} // namespace vertex_formats

class system_renderer : public  quasi_singleton<system_renderer>
{
public:
	typedef xray::render::vector< xray::render::vertex_colored >	colored_vertices_type;
	typedef xray::render::vector< u16 >								colored_indices_type;

public:
	system_renderer( const render_dx10::scene_render& scene, res_buffer * quad_ib);
	~system_renderer();

	void draw_lines			( colored_vertices_type const& vertices, colored_indices_type  const& indices);
	void draw_triangles		( colored_vertices_type const& vertices, colored_indices_type  const& indices);
	void draw_screen_lines	( xray::math::float3 const* points, u32 count, color clr, float width, u32 pattern, bool use_depth);
	void draw_3D_point		( xray::math::float3 const& position, float width, color const color);
	void setup_grid_render_mode	( u32 grid_density );
	void remove_grid_render_mode( );

	void setup_rotation_control_modes ( bool color_write);

	void draw_ui_vertices( vertex_formats::TL const * vertices, u32 const & count, int prim_type, int point_type );


private:
	
	const render_dx10::scene_render& m_scene;

	// system line
	ref_effect				m_sh_sl;
	ref_geometry			m_colored_geom_sl;
	constant_host*			m_WVP_sl;

	vertex_stream			m_vertex_stream;
	index_stream			m_index_stream;



	ref_effect				m_sh_vcolor;
	ref_effect				m_sh_grid_25;
	ref_effect				m_sh_grid_50;
	ref_effect				m_sh_ui;

	ref_geometry			m_colored_geom;
	ref_geometry			m_grid_geom;
	ref_geometry			m_ui_geom;
	ref_texture				m_grid_texture_25;
	ref_texture				m_grid_texture_50;

	ref_state				m_rotation_mode_states[2];

	bool					m_grid_mode;
	float					m_grid_density;
	constant_host*			m_grid_density_constant;

	bool					m_color_write;
	u32						m_rotation_mode_ref_value;

}; // class system_renderer

} // namespace render_dx10
} // namespace xray

#endif // #ifndef SYSTEM_RENDERER_H_INCLUDED