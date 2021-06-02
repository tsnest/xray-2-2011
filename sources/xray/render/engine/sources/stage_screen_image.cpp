////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_screen_image.h"
#include <xray/render/core/effect_descriptor.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/pix_event_wrapper.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/res_declaration.h>

namespace xray {
namespace render {

#pragma pack(push,4)
struct screen_image_vertex
{
	inline screen_image_vertex( float  x, float  y, float tcu, float tcv ) : position ( x, y), text_coords ( tcu, tcv) {}

	float2  position;
	float2  text_coords;
};
#pragma pack(pop)

class scr_quad_effect: public effect_descriptor
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config)
	{
		XRAY_UNREFERENCED_PARAMETER	( config );

		compiler.begin_technique( /*LEVEL*/);
		compiler.begin_pass( "sa_quad", NULL, "image", shader_configuration(), NULL)
					.set_depth			( false, false, D3D_COMPARISON_ALWAYS)
					.set_alpha_blend	( false)
					.set_stencil		( false)
					//.def_sampler( "s_base", r2_rt_albedo)
				.end_pass()
			.end_technique();
	}
private:
};

stage_screen_image::stage_screen_image( renderer_context* context):stage( context)
{
// 	D3D_INPUT_ELEMENT_DESC	dcl[MAX_FVF_DECL_SIZE];
// 	CHECK_RESULT( D3DXDeclaratorFromFVF( D3DFVF_XYZRHW | D3DFVF_TEX1, dcl));
	D3D_INPUT_ELEMENT_DESC dcl[] = 
	{
		{"POSITIONT",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 8, 	D3D_INPUT_PER_VERTEX_DATA, 0}
	};
	m_decl_ptr	= resource_manager::ref().create_declaration( dcl);

	effect_manager::ref().create_effect<scr_quad_effect>(&m_sh);

	m_textures.resize(1, NULL);
}

bool stage_screen_image::is_effects_ready() const
{
	return m_sh.c_ptr() != NULL;
}

void stage_screen_image::execute( ref_texture t_image)
{
	PIX_EVENT( stage_screen_image );
	
	if (!is_effects_ready())
		return;
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}

	backend::ref().reset_render_targets();
	backend::ref().set_depth_stencil_target( NULL);

	backend::ref().clear_render_targets( math::color( 0.f, 0.f, 0.f, 0.f));

	m_sh->apply();

	ASSERT( m_textures.size() == 1);
	m_textures[0] = t_image;
//	backend::ref().set_ps_textures( NULL);      // To refuse caching of texture list.
	backend::ref().set_ps_texture( "t_base", &*t_image);
	//t_image->apply(0);

	backend::ref().set_declaration( &*m_decl_ptr);

	u32 base_offset;
	screen_image_vertex* vertices = (screen_image_vertex*)backend::ref().vertex.lock( 4, sizeof(screen_image_vertex), base_offset);
	
// 	vertices[1] = screen_image_vertex( x1-orig_x, y1-orig_y, 0, 0 );
// 	vertices[0] = screen_image_vertex( x2-orig_x, y1-orig_y, 1, 0 );
// 	vertices[3] = screen_image_vertex( x1-orig_x, y2-orig_y, 0, 1 );
// 	vertices[2] = screen_image_vertex( x2-orig_x, y2-orig_y, 1, 1 );

	vertices[0] = screen_image_vertex( -1,  1, 0, 0 );
	vertices[1] = screen_image_vertex(  1,  1, 1, 0 );
	vertices[2] = screen_image_vertex( -1, -1, 0, 1 );
	vertices[3] = screen_image_vertex(  1, -1, 1, 1 );

	backend::ref().vertex.unlock();

	backend::ref().set_vb( &backend::ref().vertex.buffer(), sizeof(screen_image_vertex));


// 	backend::ref().flush();
// 	device::ref().device()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices, 6*4);

	backend::ref().render( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, base_offset);
}

} // namespace render
} // namespace xray

