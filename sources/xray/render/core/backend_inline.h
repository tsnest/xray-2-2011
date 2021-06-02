////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BACKEND_INLINE_H_INCLUDED
#define BACKEND_INLINE_H_INCLUDED

namespace xray {
namespace render {

void backend::set_rasterizer_state		( ID3DRasterizerState*		rasterizer_state)
{
	m_dirty_objects.rasterizer_state |= m_rasterizer_state != rasterizer_state;
	m_rasterizer_state = rasterizer_state;
}

void backend::set_depth_stencil_state	( ID3DDepthStencilState*	depth_stencils_State)
{
	m_dirty_objects.depth_stencil_state |= m_depth_stencils_state != depth_stencils_State;
	m_depth_stencils_state = depth_stencils_State;
}

void backend::set_blend_state			( ID3DBlendState *			effect_State)
{
	m_dirty_objects.effect_state |= m_effect_state != effect_State;
	m_effect_state = effect_State;
}

inline void backend::set_vb( untyped_buffer* vb, u32 vb_stride, u32 offset /* in bytes*/)
{
	m_dirty_objects.vertex_buffer |= ( m_vb != vb || vb_stride != m_vb_stride || m_vb_offset != offset);

	m_vb		= vb;
	m_vb_stride	= vb_stride;
	m_vb_offset	= offset;
}

inline void backend::set_ib( untyped_buffer* ib, u32 offset /*in bytes*/)
{
	m_dirty_objects.index_buffer |= (m_ib != ib || m_ib_offset != offset);
	m_ib = ib;
	m_ib_offset = offset;
}

inline void backend::set_vs( res_vs_hw * vs)
{
	m_dirty_objects.vertex_shader |= m_vs != vs;
	m_vs = vs;
	
	// Reseting input layout that the new shader will need new layout.
	// if no layout will be specified until a draw call then a 
	// corresponding input layout will be chosen from resource_manager.
	m_input_layout = m_dirty_objects.vertex_shader ? NULL : m_input_layout;
	m_dirty_objects.input_layout = m_dirty_objects.vertex_shader;
}

inline void backend::set_gs( res_gs_hw * gs)
{
	m_dirty_objects.geometry_shader |= m_gs != gs;
	m_gs = gs;
}

inline void backend::set_ps( res_ps_hw * ps)
{
	m_dirty_objects.pixel_shader |= m_ps != ps;
	m_ps = ps;
}

inline void backend::set_declaration ( res_declaration * decl)
{
	if( m_decl != decl)
	{
		m_decl = decl;
		m_dirty_objects.input_declaration = true;

		// Reseting input layout that the new decl will need new layout.
		// if no layout will be specified until a draw call then a 
		// corresponding input layout will be chosen from resource_manager.
		m_input_layout = NULL;
		m_dirty_objects.input_layout = true;
	}
}

inline	void backend::set_input_layout( res_input_layout * layout)
{
	m_dirty_objects.input_layout |= m_input_layout != layout;
	m_input_layout = layout;
}

inline void backend::set_vs_constants( shader_constant_table * ctable)
{
	if( m_vs_constants_handler.m_current == ctable)
		return;

	m_vs_constants_handler.assign		( ctable);

	m_dirty_objects.vertex_constants = true;

	ASSERT( m_constant_update_markers[enum_shader_type_vertex] != m_constant_update_counter,
		"This means that shader constant buffers are being set more than once before a draw call. \
		Some outdated constants from previous shader onstant buffers may stay as actual.");
	m_constant_update_markers[enum_shader_type_vertex] = m_constant_update_counter;
}

inline void backend::set_gs_constants( shader_constant_table * ctable)
{
	if( m_gs_constants_handler.m_current == ctable)
		return;

	m_gs_constants_handler.assign		( ctable);

	m_dirty_objects.geometry_constants = true;

	ASSERT( m_constant_update_markers[enum_shader_type_geometry] != m_constant_update_counter,
		"This means that shader constant buffers are being set more than once before a draw call. \
		Some outdated constants from previous shader onstant buffers may stay as actual.");
	m_constant_update_markers[enum_shader_type_geometry] = m_constant_update_counter;
}

inline void backend::set_ps_constants( shader_constant_table * ctable)
{
	if( m_ps_constants_handler.m_current == ctable)
		return;

	m_ps_constants_handler.assign		( ctable);

	m_dirty_objects.pixel_constants = true;

	ASSERT( m_constant_update_markers[enum_shader_type_pixel] != m_constant_update_counter,
		"This means that shader constant buffers are being set more than once before a draw call. \
		Some outdated constants from previous shader constant buffers may stay as actual.");

	m_constant_update_markers[enum_shader_type_pixel] = m_constant_update_counter;
}

inline void backend::set_vs_textures	( res_texture_list * textures)
{
	if( m_vs_textures_handler.m_current == textures)
		return;
	
	m_vs_textures_handler.assign( textures);
	m_dirty_objects.vertex_textures = true;
}

inline void backend::set_gs_textures	( res_texture_list * textures)
{
	if( m_gs_textures_handler.m_current == textures)
		return;

	m_gs_textures_handler.assign( textures);
	m_dirty_objects.geometry_textures = true;
}

inline void backend::set_ps_textures	( res_texture_list * textures)
{
	if( m_ps_textures_handler.m_current == textures)
		return;

	m_ps_textures_handler.assign( textures);
	m_dirty_objects.pixel_textures = true;
}

inline void backend::set_vs_texture	( char const * name, res_texture * texture)
{
	m_dirty_objects.vertex_textures = m_vs_textures_handler.set_overwrite( name, texture);
}

inline void backend::set_gs_texture	( char const * name, res_texture * texture)
{
	m_dirty_objects.geometry_textures = m_gs_textures_handler.set_overwrite( name, texture);
}

inline void backend::set_ps_texture	( char const * name, res_texture * texture)
{
	m_dirty_objects.pixel_textures = m_ps_textures_handler.set_overwrite( name, texture);
}

inline void backend::set_vs_samplers	( res_sampler_list * samplers)
{
	if( m_vs_samplers_handler.m_current == samplers)
		return;

	m_vs_samplers_handler.assign( samplers);
	m_dirty_objects.vertex_samplers = true;
}

inline void backend::set_gs_samplers	( res_sampler_list * samplers)
{
	if( m_gs_samplers_handler.m_current == samplers)
		return;

	m_gs_samplers_handler.assign( samplers);
	m_dirty_objects.geometry_samplers = true;
}

inline void backend::set_ps_samplers	( res_sampler_list * samplers)
{
	if( m_ps_samplers_handler.m_current == samplers)
		return;

	m_ps_samplers_handler.assign( samplers);
	m_dirty_objects.pixel_samplers = true;
}

inline	void backend::set_render_targets( render_target const * rt0, render_target const * rt1, render_target const * rt2, render_target const * rt3)
{
	set_render_target( enum_target_rt0, rt0);
	set_render_target( enum_target_rt1, rt1);
	set_render_target( enum_target_rt2, rt2);
	set_render_target( enum_target_rt3, rt3);
}

inline	void backend::reset_render_targets		( bool only_the_base)
{
	m_dirty_targets.render_targets[0] |=	m_targets[0] != m_base_rt;
	m_targets[0] = m_base_rt;

	for( int i = 1; i < enum_target_count && !only_the_base; ++i)
	{
		m_dirty_targets.render_targets[i] |=  NULL != m_targets[i];
		m_targets[i] = NULL;
	}

}

inline	void backend::reset_depth_stencil_target	()
{
	m_dirty_targets.depth_stencil |=	m_zb != m_base_zb;
	m_zb = m_base_zb;
}

inline void backend::flush_rt_shader_resources()
{
	// TODO: D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT
	ID3DShaderResourceView* rv[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
	device::ref().d3d_context()->PSSetShaderResources(0, array_size(rv), &rv[0]);
	device::ref().d3d_context()->VSSetShaderResources(0, array_size(rv), &rv[0]);
	device::ref().d3d_context()->GSSetShaderResources(0, array_size(rv), &rv[0]);
}

inline void backend::flush_rt_views()
{
	ID3DRenderTargetView* tv[D3D_SIMULTANEOUS_RENDER_TARGET_COUNT];
	xray::memory::zero(tv);
	
	device::ref().d3d_context()->OMSetRenderTargets(D3D_SIMULTANEOUS_RENDER_TARGET_COUNT, &tv[0], 0);
}

inline void backend::flush_rt()
{
	ASSERT( enum_target_zb == enum_target_count+1);
	
	if( m_dirty_targets.any())
		device::ref().d3d_context()->OMSetRenderTargets( enum_target_count, &m_targets[0], m_zb);

	m_dirty_targets.reset();
}

void	backend::set_render_output		( res_render_output const * render_output)
{
	m_render_output	 = render_output;

	m_base_rt = m_render_output.c_ptr() != NULL ? m_render_output->m_base_rt : NULL;
	m_base_zb = m_render_output.c_ptr() != NULL ? m_render_output->m_base_zb : NULL;
}

//////////////////////////////////////////////////////////////////////////
// VS constants
template <typename T>
void backend::set_vs_constant( shader_constant_host const * c, T const & arg)
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_type_traits_are_supported );
	ASSERT( c != NULL);

	//if (!disabled_shader_constansts_set)
	{
		m_vs_constants_handler.set_constant( *c, arg);
		num_setted_shader_constants++;
	}
}

template <typename T, u32 array_size>
void backend::set_vs_constant	( shader_constant_host const * c,  T const (&arg)[array_size] )
{
	set_vs_constant	( c, &arg[0], array_size);
}

template <typename T>
void backend::set_vs_constant	( shader_constant_host const * c, T const* arg, u32 array_size )
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_type_traits_are_supported_ );
	ASSERT( c != NULL);

	//if (!disabled_shader_constansts_set)
	{
		m_vs_constants_handler.set_constant_array( *c, arg, array_size);
		num_setted_shader_constants++;
	}
}

//////////////////////////////////////////////////////////////////////////
// GS constants
template <typename T>
void backend::set_gs_constant( shader_constant_host const * c, T const & arg)
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_type_traits_are_supported );
	ASSERT( c != NULL);

	if (!disabled_shader_constansts_set)
	{
		m_gs_constants_handler.set_constant( *c, arg);
		num_setted_shader_constants++;
	}
}

template <typename T, u32 array_size>
void backend::set_gs_constant	( shader_constant_host const * c,  T const (&arg)[array_size] )
{
	set_gs_constant	( c, &arg[0], array_size);
}

template <typename T>
void backend::set_gs_constant	( shader_constant_host const * c, T const* arg, u32 array_size )
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_type_traits_are_supported_ );
	ASSERT( c != NULL);

	if (!disabled_shader_constansts_set)
	{
		m_gs_constants_handler.set_constant_array( *c, arg, array_size);
		num_setted_shader_constants++;
	}
}

//////////////////////////////////////////////////////////////////////////
// PS constants
template <typename T>
void backend::set_ps_constant( shader_constant_host const * c, T const & arg)
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_type_traits_are_supported );
	ASSERT( c != NULL);

	//if (!disabled_shader_constansts_set)
	{
		m_ps_constants_handler.set_constant( *c, arg);
		num_setted_shader_constants++;
	}
}

template <typename T, u32 array_size>
void backend::set_ps_constant	( shader_constant_host const * c,  T const (&arg)[array_size])
{
	set_ps_constant	( c,  &arg[0], array_size);
}

template <typename T>
void backend::set_ps_constant	( shader_constant_host const * c,  T const *arg, u32 array_size)
{
	COMPILE_ASSERT(	constant_type_traits<T>::value == true, Only_types_described_in_constant_type_traits_are_supported_ );
	ASSERT( c != NULL);
	
	//if (!disabled_shader_constansts_set)
	{
		m_ps_constants_handler.set_constant_array( *c, arg, array_size);
		num_setted_shader_constants++;
	}
}

//////////////////////////////////////////////////////////////////////////
inline void backend::reset()
{
	set_stencil_ref			( 0x00);
	set_sample_mask			( 0xFFFFFFFF);

	set_declaration( 0);
	set_rasterizer_state( 0);

	set_depth_stencil_state	( 0);
	set_blend_state	( 0);
	set_vs( 0);
	set_vs_constants( 0);
	set_vs_samplers( 0);
	set_vs_textures( 0);

	set_gs( 0);
	set_gs_constants( 0);
	set_gs_samplers( 0);
	set_gs_textures( 0);

	set_ps( 0);
	set_ps_constants( 0);
	set_ps_samplers( 0);
	set_ps_textures( 0);

	set_vs_constants( 0);
	set_vb( 0, 0, 0);
	set_ib( 0, 0); 

	// temporary uncomment this when render targets size compatibility issue is fixed.
	set_render_targets	(0, 0, 0, 0);//reset_render_targets();
	set_depth_stencil_target(0);

	m_constant_update_counter ++;
}


inline void backend::clear_render_targets( float r, float g, float b, float a)
{
	float color_elements[4] = {r, g, b, a};
	
	for( int i = 0; i< enum_target_count; ++i)
		if( m_targets[i])
			device::ref().d3d_context()->ClearRenderTargetView( m_targets[i], color_elements );
}

inline void backend::clear_render_targets( math::color color )
{
	float color_elements[4];
	color.get_RGBA(color_elements[0], color_elements[1], color_elements[2], color_elements[3] );

	for( int i = 0; i< enum_target_count; ++i)
		if( m_targets[i])
			device::ref().d3d_context()->ClearRenderTargetView( m_targets[i], color_elements );
}


inline void backend::clear_render_targets( enum_render_target_enum slot, math::color color)
{
	if (m_targets[slot])
	{
		float color_elements[4];
		color.get_RGBA(color_elements[0], color_elements[1], color_elements[2], color_elements[3]);
		device::ref().d3d_context()->ClearRenderTargetView( m_targets[slot], color_elements);
	}
}

inline void backend::clear_render_targets( math::color color0, math::color color1, math::color color2, math::color color3)
{
	COMPILE_ASSERT	( enum_target_count == 4, Unknown_render_targets_count );
	float color_elements[4];

	if( m_targets[0])
	{
		color0.get_RGBA(color_elements[0], color_elements[1], color_elements[2], color_elements[3] );
		device::ref().d3d_context()->ClearRenderTargetView( m_targets[0], color_elements);
	}

	if( m_targets[1])
	{
		color1.get_RGBA(color_elements[0], color_elements[1], color_elements[2], color_elements[3] );
		device::ref().d3d_context()->ClearRenderTargetView( m_targets[1], color_elements);
	}

	if( m_targets[2])
	{
		color2.get_RGBA(color_elements[0], color_elements[1], color_elements[2], color_elements[3] );
		device::ref().d3d_context()->ClearRenderTargetView( m_targets[2], color_elements);
	}

	if( m_targets[3])
	{
		color3.get_RGBA(color_elements[0], color_elements[1], color_elements[2], color_elements[3] );
		device::ref().d3d_context()->ClearRenderTargetView( m_targets[3], color_elements);
	}
}

inline void backend::clear_depth_stencil( u32 flags, float z_value, u8 stencil_value)
{
 	//flush_rt();
	if (m_zb)
	{
		device::ref().d3d_context()->ClearDepthStencilView( m_zb, flags, z_value, stencil_value);
	}
}

inline void	backend::render_indexed( D3D_PRIMITIVE_TOPOLOGY type, u32 index_count, u32 start_index, u32 base_vertex)
{
	flush();
	update_input_layout();
	num_draw_calls			++;
	//stat.calls			++;
	//stat.verts			+= 3*PC;
	//stat.polys			+= PC;

	device::ref().d3d_context()->IASetPrimitiveTopology( type);	

	device::ref().d3d_context()->DrawIndexed( index_count, start_index, base_vertex);
	
	// TODO
	switch (type)
	{
		case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST: 
			num_total_rendered_triangles += index_count / 3; 
			break;
		
		case D3D10_PRIMITIVE_TOPOLOGY_POINTLIST:
			num_total_rendered_points += index_count;
			break;
		
		case D3D10_PRIMITIVE_TOPOLOGY_LINELIST: 
			num_total_rendered_triangles += index_count / 2;
			break;
	}
}

inline void	backend::render( D3D_PRIMITIVE_TOPOLOGY type, u32 vertex_count, u32 base_vertex)
{
	flush();
	update_input_layout();
	num_draw_calls			++;
	//stat.calls			++;
	//stat.verts			+= 3*PC;
	//stat.polys			+= PC;

	device::ref().d3d_context()->IASetPrimitiveTopology( type);
	device::ref().d3d_context()->Draw( vertex_count,  base_vertex);

	// TODO
	switch (type)
	{
		case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST: 
			num_total_rendered_triangles += vertex_count / 3; 
			break;
		
		case D3D10_PRIMITIVE_TOPOLOGY_POINTLIST:
			num_total_rendered_points += vertex_count;
			break;
		
		case D3D10_PRIMITIVE_TOPOLOGY_LINELIST: 
			num_total_rendered_triangles += vertex_count / 2;
			break;
	}
}

void backend::set_stencil_ref			( u32 stencil_ref)
{
	m_dirty_objects.depth_stencil_state |= m_stencil_ref != stencil_ref;
	m_stencil_ref = stencil_ref;
}

void backend::set_sample_mask			( u32 sample_mask)
{
	m_dirty_objects.effect_state |= m_sample_mask != sample_mask;
	m_sample_mask = sample_mask;
}

template<enum_shader_type shader_type>
u32 backend::get_constants_marker ()
{
	return m_constant_update_markers[shader_type];
}

void backend::get_viewports ( u32 & count/*in/out*/, D3D_VIEWPORT * viewports)
{
	device::ref().d3d_context()->RSGetViewports( &count, viewports);
}

void backend::set_viewports ( u32 count, D3D_VIEWPORT const * viewports)
{
	device::ref().d3d_context()->RSSetViewports( count, viewports);
}

void backend::get_viewport ( D3D_VIEWPORT & viewport)
{
	u32 count = 1;
	get_viewports ( count, &viewport);
}

void backend::set_viewport ( D3D_VIEWPORT const & viewports)
{
	set_viewports( 1, &viewports);
}

u32	backend::target_width	() const
{
	return m_render_output->width();
}

u32	backend::target_height	() const
{
	return m_render_output->height();
}

} // namespace render 
} // namespace xray 


#endif // #ifndef BACKEND_INLINE_H_INCLUDED