////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/backend.h>
#include "com_utils.h"
#include <xray/render/core/res_declaration.h>
#include <xray/render/core/res_xs_hw.h>
#include <xray/render/core/render_target.h>
#include <xray/render/core/res_sampler_list.h>

namespace xray {
namespace render {

backend::backend	( ) :
	m_device					( device::ref().d3d_device() ),
	m_constant_update_counter	(1),
	m_rasterizer_state			(NULL),
	m_depth_stencils_state		(NULL),
	m_effect_state				(NULL),
	m_stencil_ref				(0x00),
	m_sample_mask				(0xFFFFFFFF),
	m_vb						(NULL),
	m_ib						(NULL),
	m_vs						(NULL),
	m_ps						(NULL),
	m_gs						(NULL),
	m_vs_textures_handler		(m_vs),
	m_gs_textures_handler		(m_gs),
	m_ps_textures_handler		(m_ps),
	m_vs_samplers_handler		(m_vs),
	m_gs_samplers_handler		(m_gs),
	m_ps_samplers_handler		(m_ps),
	m_decl						(NULL),
	m_input_layout				(NULL),
	m_render_output				(NULL),
	vertex						( (512+1024)*Kb ),
	index						( 512*Kb ),
	num_setted_shader_constants	( 0 ),
	num_draw_calls				( 0 ),
	disabled_shader_constansts_set ( false ),
	num_total_rendered_points	( 0 ),
	num_total_rendered_triangles( 0 )
{
	for ( int i = 0; i < enum_shader_types_count; ++i )
		m_constant_update_markers[i] = 1;

	m_device->AddRef			( );
	reset						( );
}

backend::~backend()
{
	for( constant_hosts::iterator it = m_constant_hosts.begin(); it< m_constant_hosts.end(); ++it)
		DELETE( *it);

	on_device_destroy();
}

void backend::on_device_destroy()
{
	safe_release(m_device);
	m_device = 0;

// 	vertex.destroy();
// 	index.destroy();
}

// static inline bool sorted_vector_predicate( shader_constant_host const * first, shader_constant_host const * second)
// {
// 	return first->name < second->name;
// }

static inline bool sorted_vector_predicate( shader_constant_host const * first, shared_string const & second)
{
	return first->name() < second;
}

shader_constant_host * backend::register_constant_host	( shader_constant const& shader_constant)
{
	return register_constant_host( shader_constant.host().name(), shader_constant.host().type() );
}

shader_constant_host * backend::register_constant_host	( shared_string const& name, enum_constant_type const type )
{
	constant_hosts::iterator it = std::lower_bound( m_constant_hosts.begin(), m_constant_hosts.end(), name, sorted_vector_predicate);

	if ( it == m_constant_hosts.end() || !((*it)->name() == name))
		return *m_constant_hosts.insert( it, NEW (shader_constant_host)(name, type));
	else
		return *it;
}

shader_constant_host const * backend::find_constant_host		( shared_string const& name, enum_constant_type const type , bool const create_if_not )
{
	constant_hosts::iterator it = std::lower_bound( m_constant_hosts.begin(), m_constant_hosts.end(), name, sorted_vector_predicate);

	if ( it == m_constant_hosts.end() || !(**it == name) )
	{
		if( !create_if_not)
			return  NULL;
		else
			return register_constant_host( name, type );
	}
	else
		return *it;
}

void backend::reset_constant_update_markers	()
{
	LOG_INFO("reset_constant_update_markers called !!!");
	constant_hosts::iterator		it =	m_constant_hosts.begin();
	constant_hosts::const_iterator	end =	m_constant_hosts.end();

	for( ; it!= end; ++it)
		(*it)->reset_update_markers	( );

	m_constant_update_counter = 1;
}

void backend::update_input_layout()
{
	// Set always
	//if( m_dirty_objects.input_layout)
	{
		if( !m_input_layout)
		{
			ASSERT( m_decl, "It is needed to define either input layout or a vertex declaration.");
			m_input_layout = m_decl->get( &*m_vs->data().signature);
		}

		ASSERT( m_input_layout);
		device::ref().d3d_context()->IASetInputLayout( m_input_layout->hardware_layout());
	}
}

void backend::set_render_target( enum_render_target_enum target, render_target const * rt)
{
	ID3DRenderTargetView * rt_view = (rt == NULL) ? NULL : rt->get_target_view();

	if( m_targets[target] != rt_view)
	{
		m_targets[target] = rt_view;
		m_dirty_targets.render_targets[target] = true;
	}
}

void backend::set_depth_stencil_target( render_target const * zrt)
{
	ID3DDepthStencilView* zrt_view = (zrt == NULL) ? NULL : zrt->get_depth_stencil_view();

	m_dirty_targets.depth_stencil |=	m_zb != zrt_view;
	m_zb = zrt_view;
}

void backend::flush()
{
	//Here may be used caching to prevent reseting the same state.
	if( m_dirty_objects.rasterizer_state)
		device::ref().d3d_context()->RSSetState				( m_rasterizer_state);

	if( m_dirty_objects.depth_stencil_state)
		device::ref().d3d_context()->OMSetDepthStencilState	( m_depth_stencils_state, m_stencil_ref);

	if( m_dirty_objects.effect_state)
	{
		// --Porting to DX10_
		// give correct blend factors
		float blend_factor[4]  = {0.f,0.f,0.f,0.f} ;
		device::ref().d3d_context()->OMSetBlendState			( m_effect_state, blend_factor, m_sample_mask);
	}

	flush_rt();

// 	if( m_dirty_objects.any())
// 	{
		if( m_dirty_objects.vertex_shader)
		{
			ID3DVertexShader * vs	= (m_vs == (res_vs_hw*)NULL) ? NULL : m_vs->hardware_shader();
			device::ref().d3d_context()->VSSetShader( vs SHADER_PARAM_STUB);
		}
		
		if( m_dirty_objects.geometry_shader)
		{
			ID3DGeometryShader * gs	= (m_gs == (res_gs_hw*)NULL) ? NULL : m_gs->hardware_shader();
			device::ref().d3d_context()->GSSetShader( gs SHADER_PARAM_STUB);
		}
		
		if( m_dirty_objects.pixel_shader)
		{
			ID3DPixelShader * ps	= (m_ps == (res_ps_hw*)NULL) ? NULL : m_ps->hardware_shader();
			device::ref().d3d_context()->PSSetShader( ps SHADER_PARAM_STUB);
		}

		if( m_dirty_objects.vertex_buffer)
		{
			ID3DVertexBuffer * buffer = (m_vb == (untyped_buffer*)NULL) ? NULL : m_vb->hardware_buffer();
			device::ref().d3d_context()->IASetVertexBuffers( 0, 1, &buffer, &m_vb_stride, &m_vb_offset);
			//CHECK_RESULT( m_device->SetStreamSource( 0, m_vb, 0, m_vb_stride));
		}

		if( m_dirty_objects.index_buffer)
		{
			ID3DIndexBuffer * buffer = (m_ib == (untyped_buffer*)NULL) ? NULL : m_ib->hardware_buffer();
			device::ref().d3d_context()->IASetIndexBuffer( buffer, DXGI_FORMAT_R16_UINT, m_ib_offset);
			//CHECK_RESULT( m_device->SetIndices( m_ib));
		}


		m_vs_constants_handler.update_buffers();
		m_gs_constants_handler.update_buffers();
		m_ps_constants_handler.update_buffers();

		if( m_dirty_objects.vertex_constants)
			m_vs_constants_handler.apply();

		if( m_dirty_objects.vertex_textures)
			m_vs_textures_handler.apply();

		if( m_dirty_objects.vertex_samplers)
			m_vs_samplers_handler.apply();

		if( m_dirty_objects.geometry_constants)
			m_gs_constants_handler.apply();

		if( m_dirty_objects.geometry_textures)
			m_gs_textures_handler.apply();

		if( m_dirty_objects.geometry_samplers)
			m_gs_samplers_handler.apply();
		
		if( m_dirty_objects.pixel_constants)
			m_ps_constants_handler.apply();

		if( m_dirty_objects.pixel_textures)
			m_ps_textures_handler.apply();

		if( m_dirty_objects.pixel_samplers)
			m_ps_samplers_handler.apply();
		
		m_constant_update_counter++;

		m_dirty_objects.reset();
//	}
}

} // namespace render
} // namespace xray