////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_model.h"
#include <xray/render/engine/model_format.h>
#include "material_manager.h"
#include <xray/render/core/res_texture.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {

render_model::render_model( )
:m_locators(NULL),
m_locators_count(0),
m_aabbox( math::create_identity_aabb() )
{}

void render_model::load_properties( configs::binary_config_value const& properties )
{
	m_aabbox.max			= xray::math::float3(properties["bounding_box"]["max"]);
	m_aabbox.min			= xray::math::float3(properties["bounding_box"]["min"]);

	m_locators_count		= (u16)properties.value_exists("locators") ? (u16)properties["locators"].size() : 0;

	if(m_locators_count)
	{
		m_locators	= ALLOC(model_locator_item, m_locators_count);
		for(u16 i=0; i<m_locators_count; ++i)
		{
			configs::binary_config_value const& v = properties["locators"][i];
			model_locator_item& item	= m_locators[i];
			pcstr name					= v["name"];
			int bone_idx				= v["bone_idx"];
			item.m_bone					= u16(bone_idx);
			strings::copy				( item.m_name, name );
			item.m_offset				= math::create_rotation(v["rotation"]) * math::create_translation(v["position"]);
		}
	}

}

bool render_model::get_locator( pcstr locator_name, model_locator_item& result ) const
{
	for( u16 i=0; i<m_locators_count; ++i)
	{
		model_locator_item const& item = m_locators[i];
		if(strings::equal(item.m_name, locator_name))
		{
			result = item;
			return true;
		}
	}
	return false;
}

material_effects& render_surface::get_material_effects	( )
{
	// TODO: fix it!!!
	if (!m_materail_effects_instance.c_ptr())
	{
		return material::nomaterial_material( get_vertex_input_type() );
	}
	else
	{
		return m_materail_effects_instance->get_material_effects();
	}
}

render_surface::~render_surface()
{
	material_manager::ref().remove_material_effects(m_materail_effects_instance);
}

void render_surface::set_default_material( )
{
	m_materail_effects_instance = 0;
}

void render_surface::set_material_effects( material_effects_instance_ptr mtl_instance_ptr, pcstr material_name )
{
	if (mtl_instance_ptr)
	{
		material_manager::ref().remove_material_effects(m_materail_effects_instance);
		
		m_materail_effects_instance	= mtl_instance_ptr;
		
		material_manager::ref().add_material_effects(
			m_materail_effects_instance,
			material_name
		);
	}
}

void render_surface::load( configs::binary_config_value const& properties, memory::chunk_reader& chunk )
{
	(void)&chunk;
	
	m_aabbox.max			= xray::math::float3(properties["bounding_box"]["max"]);
	m_aabbox.min			= xray::math::float3(properties["bounding_box"]["min"]);
}

void render_surface_instance::set_constants( )
{
	m_parent->set_constants	( );
}

#pragma warning(push)
#pragma warning(disable:4355)
render_model_instance_impl::render_model_instance_impl( )
:m_collision_object	(this)
{}
#pragma warning(pop)


} // namespace render 
} // namespace xray 
