////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/stage_texture_mip_clamp_test.h>
#include <xray/render/engine/effect_combine.h>
#include <xray/render/engine/environment.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/engine/vertex_formats.h>

namespace xray {
namespace render_dx10 {

class stage_texture_mip_clamp_test_effect: public effect
{
public:
	void compile( effect_compiler& compiler, const effect_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETER	( options );

		shader_defines_list	defines;
		make_defines( defines);

		ref_texture tex0 = resource_manager::ref().create_texture( "wood/wood_old", 0, true);
		ref_texture tex1 = resource_manager::ref().create_texture( "wood/wood_old_",4, true);

		static float bias0 = 0.f;
		static float bias1 = 0.f;

		compiler.begin_technique( )
				.begin_pass( "deffer_base_flat_tex_test", "deffer_base_flat_tex_test", defines)
					.set_cull_mode( D3D_CULL_NONE)
					.set_stencil( true, 0x01, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
					.bind_constant( "base_tex_bias", &bias0)
					.set_texture( "t_base", &*tex0)
				.end_pass()
			.end_technique();

		compiler.begin_technique( )
				.begin_pass( "deffer_base_flat_tex_test", "deffer_base_flat_tex_test", defines)
					.set_cull_mode( D3D_CULL_NONE)
					.set_stencil( true, 0x01, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP)
					.bind_constant( "base_tex_bias", &bias1)
					.set_texture( "t_base", &*tex1)
				.end_pass()
			.end_technique();
	}

private:
};

stage_texture_mip_clamp_test::stage_texture_mip_clamp_test( scene_context* context):stage( context)
{
	m_decl_ptr	= resource_manager::ref().create_declaration( vertex_formats::F_V);

	stage_texture_mip_clamp_test_effect b;
	m_sh		= effect_manager::ref().create_effect( &b);
}

void stage_texture_mip_clamp_test::execute( )
{
	PIX_EVENT( stage_texture_mip_clamp_test );

	m_sh->apply( 0 );


	backend::ref().set_declaration( &*m_decl_ptr);

	//m_context->set_w( math::create_scale( float3( 0.2f, 0.2f, 0.2f)));
	m_context->set_w( float4x4().identity());
	

	u32 base_offset;
	vertex_formats::V* vertices;
	backend::ref().vertex.lock( 8, &vertices, base_offset);

	vertices[0].set( -1,  1,  1,  0, 0 );
	vertices[1].set(  1,  1,  1,  1, 0 );
	vertices[2].set( -1,  1, -1,  0, 1 );
	vertices[3].set(  1,  1, -1,  1, 1 );

	vertices += 4;
	
	float shift = 2;

	vertices[0].set( -1+shift,  1,  1,  0, 0 );
	vertices[1].set(  1+shift,  1,  1,  1, 0 );
	vertices[2].set( -1+shift,  1, -1,  0, 1 );
	vertices[3].set(  1+shift,  1, -1,  1, 1 );


	backend::ref().vertex.unlock();

	backend::ref().set_vb( backend::ref().vertex.buffer(), sizeof(vertex_formats::V));

	backend::ref().render( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, base_offset);
	
	m_sh->apply( 1 );

	backend::ref().render( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, base_offset+4);

}

} // namespace render
} // namespace xray

