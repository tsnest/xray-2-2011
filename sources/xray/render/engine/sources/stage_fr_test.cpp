////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include "stage_fr_test.h"
#include <xray/render/core/resource_manager.h>
#include "vertex_formats.h"
#include <xray/render/core/pix_event_wrapper.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/backend.h>
#include "renderer_context.h"

namespace xray {
namespace render {


stage_fr_test::stage_fr_test( renderer_context* context):stage( context)
{
	m_decl_ptr	= resource_manager::ref().create_declaration( vertex_formats::F_V);

// 	m_sh		= effect_manager::ref().create_effect<stage_texture_mip_clamp_test_effect>();
}

void stage_fr_test::execute( )
{
	PIX_EVENT( stage_fr_test );

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

	backend::ref().set_vb( &backend::ref().vertex.buffer(), sizeof(vertex_formats::V));

	backend::ref().render( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, base_offset);
	
	m_sh->apply( 1 );

	backend::ref().render( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, base_offset+4);

}

} // namespace render
} // namespace xray

