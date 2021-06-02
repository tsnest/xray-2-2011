////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_MATERIAL_EFFECTS_H_INCLUDED
#define RENDER_MATERIAL_EFFECTS_H_INCLUDED

#include <xray/render/facade/render_stage_types.h>
#include "post_process_parameters.h"
#include "material.h"

namespace xray {
namespace render {

class res_effect;

typedef	resources::resource_ptr < 
	res_effect, 
	resources::unmanaged_intrusive_base 
> ref_effect;

enum enum_vertex_input_type;

struct material_effects
{
							material_effects				();
	u32						get_render_complexity			() const;
	void					get_max_used_texture_dimension	(u32& out_size_x, u32& out_size_y) const;
	enum_vertex_input_type  get_vertex_input_type			() const;
	
	post_process_parameters m_post_process_stage_parameters;
	
	bool					stage_enable[num_render_stages];
	bool					is_emissive;
	bool					is_organic;
	bool					is_cast_shadow;
	
	xray::math::float4		organic_clear_color;
	enum_vertex_input_type  m_vertex_input_type;
	D3D_CULL_MODE			m_cull_mode;
	ref_effect				m_effects[num_render_stages];
}; // struct material_effects


struct material_effects_instance: public resources::unmanaged_resource
{
	material_effects&		get_material_effects	() { return m_material_effects; }
	void					set_material_effects	(material_effects const& in_material_effects) { m_material_effects = in_material_effects; }
	
	fs_new::virtual_path_string const&	get_material_name		() const { return m_material_name; }
	
private:
	friend class			material_effects_instance_cook;
	material_effects		m_material_effects;
	fs_new::virtual_path_string	m_material_name;
}; // struct material_effects_instance

typedef	resources::resource_ptr<
	material_effects_instance, 
	resources::unmanaged_intrusive_base
> material_effects_instance_ptr;

} // namespace render 
} // namespace xray 

#endif // #ifndef RENDER_MATERIAL_EFFECTS_H_INCLUDED