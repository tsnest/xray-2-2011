////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_effects.h"
#include <xray/render/core/custom_config.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_texture.h>
#include <xray/render/facade/vertex_input_type.h>
#include "material.h"

namespace xray {
namespace render {

material_effects::material_effects()
{
	// This is correct until the class is not polymorphic !
	//xray::memory::zero( this , sizeof(*this));
	m_cull_mode				= D3D_CULL_NONE;
	m_vertex_input_type		= null_vertex_input_type;
	xray::memory::zero		(stage_enable);
	organic_clear_color		= math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	is_cast_shadow			= true;
	is_organic				= false;
	organic_clear_color		= xray::math::float4(1.0f, 1.0f, 1.0f, 1.0f);
}

u32 material_effects::get_render_complexity() const
{
	u32 result = 0;
	for (u32 effect_index=0; effect_index<num_render_stages; effect_index++)
	{
		if (m_effects[effect_index])
		{
			result += m_effects[effect_index]->get_total_pixel_shader_instruction_count();
		}
	}
	return result;
}

void material_effects::get_max_used_texture_dimension(u32& out_size_x, u32& out_size_y) const
{
	out_size_x = out_size_y = 1;
	
	for (u32 effect_index=0; effect_index<num_render_stages; effect_index++)
	{
		if (m_effects[effect_index])
		{
			u32 current_size_x, current_size_y;
			m_effects[effect_index]->get_max_used_texture_dimension(current_size_x, current_size_y);
			out_size_x = xray::math::max(current_size_x, out_size_x);
			out_size_y = xray::math::max(current_size_y, out_size_y);
		}
	}
}

enum_vertex_input_type material_effects::get_vertex_input_type() const 
{ 
	return m_vertex_input_type; 
}

} // namespace xray
} // namespace render