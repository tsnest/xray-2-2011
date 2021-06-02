 ////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_model_skeleton.h"
#include <xray/render/engine/model_format.h>
#include <xray/animation/anim_track_common.h>
#include <xray/render/core/pix_event_wrapper.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/backend.h>

namespace xray {
namespace render {

skeleton_render_surface::skeleton_render_surface( )
{
	m_vertex_input_type=skeletal_mesh_vertex_input_type;
}

skeleton_render_model::skeleton_render_model( )
{
	m_bones_matrices_shader_constant	= 
		backend::ref().register_constant_host( "bones_matrices",  rc_float );
}

skeleton_render_model::~skeleton_render_model( )
{
	children::iterator			it		=  m_children.begin();
	children::const_iterator	end		=  m_children.end();
	for( ; it != end; ++it)
		DELETE( *it);

	if(m_locators)
		FREE(m_locators);
}

void skeleton_render_model::load_bones( memory::reader& bones_chunk )
{
	u16 bone_count				= bones_chunk.r_u16( );

	m_inverted_bones_matrices_in_bind_pose.resize		( bone_count );
	animation::frame			frm;
	
	for(u16 i=0; i<bone_count; ++i)
	{
		pcstr bone_name			= bones_chunk.r_string( );
		frm.translation			= bones_chunk.r_float3( );
		frm.rotation			= bones_chunk.r_float3( );
		frm.scale				= bones_chunk.r_float3( );
		
		float4x4 frm_matrix			= frame_matrix( frm );
		float4x4& bind_matrix_inv	= m_inverted_bones_matrices_in_bind_pose[i];
		bind_matrix_inv.try_invert	( frm_matrix );
		LOG_INFO("%s %f %f %f", bone_name, frm_matrix.c.x, frm_matrix.c.y, frm_matrix.c.z);
	}
}

void skeleton_render_model::append_surface( render_surface* child)
{
	m_children.push_back(static_cast<skeleton_render_surface*>(child));
}

void skeleton_render_model::update( render::vector<float4x4> const& bones )
{
	children::iterator it		= m_children.begin();
	children::iterator it_e		= m_children.end();

	for( ; it!=it_e; ++it)
	{
		skeleton_render_surface* s = *it;
		s->update	( bones );
	}
}

skeleton_render_model_instance::skeleton_render_model_instance( )
:m_surface_instances( NULL ),
m_instances_count	( 0 )
{}

skeleton_render_model_instance::~skeleton_render_model_instance( )
{
	DELETE_ARRAY	( m_surface_instances );
}

void skeleton_render_model_instance::assign_original( skeleton_render_model_ptr v )
{
	m_original					= v;
	m_bones_matrices.resize		( m_original->m_inverted_bones_matrices_in_bind_pose.size() );

	render::vector< float4x4 >::iterator i			= m_bones_matrices.begin( );
	render::vector< float4x4 >::iterator const e	= m_bones_matrices.end( );
	for( ; i != e; ++i )
		(*i).identity			( );

	m_instances_count	= (u16)m_original->m_children.size();
	m_surface_instances = NEW_ARRAY( render_surface_instance, m_instances_count );
	
	for( u16 i = 0; i < m_instances_count; ++i )
	{
		render_surface_instance& info = m_surface_instances[i];
		info.m_parent				= this;
		info.m_render_surface		= m_original->m_children[i];
		info.m_transform			= &m_transform;
		info.m_visible				= true;
	}

	//m_children.create_buffer( v->models().size());
	//composite_render_model_skeleton::children::const_iterator	it_m	= v->models().begin();
	//composite_render_model_skeleton::children::const_iterator	end_m	= v->models().end();

	//for( ; it_m != end_m; ++it_m)
	//{
	//	render_model_skeleton& original = **it_m;
	//	m_children.push_back( render_model_instance_skeleton( *this, original, &original.m_material_effects));
	//}
}

void skeleton_render_model_instance::get_surfaces( render_surface_instances& list, bool visible_only )
{
	list.reserve(list.size()+m_instances_count);

	for( u32 i = 0; i < m_instances_count; ++i )
	{
		render_surface_instance* inst = m_surface_instances+i;
		if( !visible_only || inst->m_visible )
			list.push_back		( inst );
	}
}

void skeleton_render_model_instance::update( )
{
	PIX_EVENT( skeleton_render_model_instance );
	m_original->update		( m_bones_matrices );

	for( u32 i = 0; i < m_instances_count; ++i )
	{
		render_surface_instance* inst = m_surface_instances+i;
		if( inst->m_visible )
		{
			
			render::vector< float4x4 >::const_iterator inv_bones_it		= m_original->m_inverted_bones_matrices_in_bind_pose.begin(), 
													   inv_bones_end	= m_original->m_inverted_bones_matrices_in_bind_pose.end(),
													   bones_it			= m_bones_matrices.begin();
			inst->m_render_surface->m_aabbox.zero();
			
#pragma message (XRAY_TODO("IronNick to IronNick: always change parent aabb?"))
			
			for (; inv_bones_it != inv_bones_end; ++inv_bones_it, ++bones_it)
				inst->m_render_surface->m_aabbox.modify(math::mul4x4(math::invert4x3(*inv_bones_it), math::transpose(*bones_it)).lines[3].xyz());
		}
	}

}

void skeleton_render_model_instance::set_constants	( )
{
	u32 const max_bones_count		= 64;
	R_ASSERT_CMP					( m_bones_matrices.size( ), <=, max_bones_count );
	backend::ref().set_vs_constant	( m_original->m_bones_matrices_shader_constant, &*m_bones_matrices.begin(), max_bones_count );
}

void skeleton_render_model_instance::update_render_matrices( float4x4 const* matrices, u32 count )
{
	R_ASSERT					(m_bones_matrices.size() == count);
	R_ASSERT					( m_original->m_inverted_bones_matrices_in_bind_pose.size() == count );

	for(u32 i=0; i<count; ++i)
		m_bones_matrices[i]		= transpose( m_original->m_inverted_bones_matrices_in_bind_pose[i] * matrices[i] );
}

bool skeleton_render_model_instance::get_locator( pcstr locator_name, model_locator_item& result ) const
{
	return m_original->get_locator( locator_name, result );
}

} // namespace render 
} // namespace xray 
