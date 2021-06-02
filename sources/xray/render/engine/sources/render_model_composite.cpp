////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_model_composite.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {

composite_render_model::composite_render_model( )
:m_surfaces_count( 0 )
{}

void composite_render_model::append_model( render_model_ptr v, float4x4 const& offset )
{
	composite_item			item;
	item.m_model			= static_cast_resource_ptr<static_render_model_ptr>(v);
	item.m_offset			= offset;
	m_children.push_back	( item );
	m_surfaces_count		+= (u16)item.m_model->m_children.size();
}

composite_render_model_instance::composite_render_model_instance( )
:m_surface_instances( NULL ),
m_instances_count	( 0 )
{}

composite_render_model_instance::~composite_render_model_instance( )
{
	DELETE_ARRAY( m_surface_instances );
	DELETE_ARRAY( m_surface_matrices );
}

void composite_render_model_instance::set_transform( float4x4 const& transform )
{
	super::set_transform		( transform );
	R_ASSERT					( m_instances_count>0 );

	// update transform matrices rof render surfaces
	u16 surface_idx = 0;
	for( u16 model_idx = 0; model_idx < m_original->m_children.size(); ++model_idx )
	{
		R_ASSERT(surface_idx<m_instances_count);

		composite_render_model::composite_item& item	= m_original->m_children[model_idx];
		u16 childs_count								= (u16)item.m_model->m_children.size();
		
		for( u16 model_surf_idx = 0; model_surf_idx < childs_count; ++model_surf_idx)
		{
			m_surface_matrices[surface_idx]		= item.m_offset * m_transform;
			++surface_idx;
		}
	}
}

void composite_render_model_instance::get_surfaces( render_surface_instances& list, bool visible_only )
{
	list.reserve(list.size()+m_instances_count);
	for( u32 i = 0; i < m_instances_count; ++i )
	{
		render_surface_instance* inst = m_surface_instances+i;
		if( !visible_only || inst->m_visible )
			list.push_back		( inst );
	}
}

void composite_render_model_instance::assign_original( composite_render_model_ptr v )
{
	m_original			= v;
	m_instances_count	= m_original->m_surfaces_count;
	m_surface_instances = NEW_ARRAY( render_surface_instance, m_instances_count );
	m_surface_matrices	= NEW_ARRAY( float4x4, m_instances_count );

	u16 surface_idx = 0;
	for( u16 model_idx = 0; model_idx < m_original->m_children.size(); ++model_idx )
	{
		R_ASSERT(surface_idx<m_instances_count);

		composite_render_model::composite_item& item	= m_original->m_children[model_idx];
		u16 childs_count								= (u16)item.m_model->m_children.size();
		
		for( u16 model_surf_idx = 0; model_surf_idx < childs_count; ++model_surf_idx)
		{
			render_surface_instance& info	= m_surface_instances[surface_idx];
			info.m_parent					= this;
			info.m_render_surface			= item.m_model->m_children[model_surf_idx];
			info.m_transform				= &m_surface_matrices[surface_idx];
			info.m_visible					= true;

			++surface_idx;
		}
	}
}

} // namespace render
} // namespace xray
