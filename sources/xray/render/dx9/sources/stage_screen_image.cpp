////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright ( C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_screen_image.h"
#include <xray/render/common/sources/blender_combine.h>
#include <xray/render/common/sources/environment.h>
#include <xray/render/common/sources/shader_manager.h>

namespace xray {
namespace render{

class scr_quad_blender: public blender
{
public:
	void compile( blender_compiler& compiler, const blender_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETER	( options );

		shader_defines_list	defines;
		make_defines( defines);

		compiler.begin_technique( /*LEVEL*/);
		compiler.begin_pass( "sa_quad", "image", defines)
			.set_z_test( FALSE, D3DCMP_ALWAYS)
			.set_alpha_blend( FALSE)
			//.def_sampler( "s_base", r2_rt_albedo)
			.end_pass()
			.end_technique();
	}

private:
};


stage_screen_image::stage_screen_image( scene_context* context):stage( context)
{
	scr_quad_blender b;
	D3DVERTEXELEMENT9	dcl[MAX_FVF_DECL_SIZE];
	R_CHK( D3DXDeclaratorFromFVF( D3DFVF_XYZRHW | D3DFVF_TEX1, dcl));
	m_decl_ptr	= resource_manager::get_ref().create_decl( dcl);
	m_sh		= shader_manager::get_ref().create_shader( &b);
}

void stage_screen_image::execute( float x1, float y1, float x2, float y2, ref_texture t_image)
{
	PIX_EVENT( stage_screen_image );

	set_default_rt();
	backend::get_ref().clear( D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0), 1.0f, 0);
	backend::get_ref().set_stencil( FALSE);

	m_sh->apply();
	m_decl_ptr->apply();

	float vertices_type[][6] =
	{
		/* x,  y,   z,   w,    tc */
		{ x1, y1, 0.0f, 1.0f, 0, 0 },
		{ x2, y1, 0.0f, 1.0f, 1, 0 },
		{ x1, y2, 0.0f, 1.0f, 0, 1 },
		{ x2, y2, 0.0f, 1.0f, 1, 1 }
	};

	t_image->apply(0);

	backend::get_ref().flush();
	hw_wrapper::get_ref().device()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vertices_type, 6*4);
}

} // namespace render
} // namespace xray

