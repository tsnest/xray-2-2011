////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/res_xs_hw.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/utils.h>

namespace xray {
namespace render {

res_pass::~res_pass()
{
	
}

void res_pass::destroy_impl	() const
{
	effect_manager::ref().delete_pass( this );
}

bool res_pass::equal(const res_pass& other) const
{
	return	m_state	== other.m_state 
			&& m_ps	== other.m_ps 
			&& m_vs	== other.m_vs 
			&& m_gs	== other.m_gs
			&& m_input_layout == other.m_input_layout;
}

void res_pass::apply() const
{
	m_vs->apply();
	m_gs->apply();
	m_ps->apply();
	
	backend::ref().set_input_layout( m_input_layout.c_ptr());
	
	m_state->apply();
}

void res_pass::init_layout( res_declaration const & decl)
{
	return;

	XRAY_UNREFERENCED_PARAMETER( decl);
#pragma message (RENDER_TODO("implement fast way to set predefined input layouts."))
// 	ASSERT( m_vs);
// 	ASSERT( !m_input_layout);
// 
// 	m_input_layout = resource_manager::ref().create_input_layout( &decl, &*m_vs->hardware_shader()->data().signature);
}

res_shader_technique::~res_shader_technique()
{
	
}

void res_shader_technique::destroy_impl() const
{
	effect_manager::ref().delete_effect_technique(this);
}

//SGeometry::~SGeometry					()			{	DEV->DeleteGeom			(this);			}
bool res_shader_technique::equal(const res_shader_technique& other) const
{
	if (m_flags.priority != other.m_flags.priority) return false;
	if (m_flags.strict_b2f != other.m_flags.strict_b2f) return false;
	if (m_flags.has_emissive != other.m_flags.has_emissive) return false;
	if (m_flags.has_wmark != other.m_flags.has_wmark) return false;
	if (m_flags.has_distort != other.m_flags.has_distort) return false;
	
	if (m_passes.size() != other.m_passes.size()) return false;

	for (u32 p = 0; p < m_passes.size(); ++p)
		if (m_passes[p] != other.m_passes[p])
			return false;

	return true;
}

void res_shader_technique::init_layouts( res_declaration const & decl)
{
	for( u32 i = 0; i < m_passes.size(); ++i)
		m_passes[i]->init_layout( decl);
}

res_effect::~res_effect()
{
	
}

void res_effect::destroy_impl() const
{
	effect_manager::ref().delete_effect(this);
}

bool res_effect::equal(const res_effect& other) const
{
	if (m_techniques.size() != other.m_techniques.size())
		return false;

	for (u32 i = 0; i < m_techniques.size(); ++i)
		if (m_techniques[i] != other.m_techniques[i])
			return false;

	return true;

	//return m_techniques[0]->equal(*other.m_techniques[0]) &&
	//	m_techniques[1]->equal(*other.m_techniques[1]) &&
	//	m_techniques[2]->equal(*other.m_techniques[2]) &&
	//	m_techniques[3]->equal(*other.m_techniques[3]) &&
	//	m_techniques[4]->equal(*other.m_techniques[4]) &&
	//	m_techniques[5]->equal(*other.m_techniques[5]);
}

void res_effect::apply_pass(u32 id)
{
	ref_shader_technique	technique = m_techniques[m_cur_technique];

	ASSERT(id < technique->m_passes.size());

	ref_pass pass = technique->m_passes[id];
	
	pass->apply();
}

void res_effect::init_layouts( res_declaration const & decl)
{
	for( u32 i = 0; i < m_techniques.size(); ++i)
		m_techniques[i]->init_layouts( decl);
}

void res_effect::get_max_used_texture_dimension(u32& out_width, u32& out_height)
{
	out_width = out_height = 0;
	for (techniques_array_type::const_iterator tech_it=m_techniques.begin(); tech_it!=m_techniques.end(); ++tech_it)
	{
		for (res_shader_technique::passes_array_type::const_iterator pass_it=(*tech_it)->m_passes.begin(); pass_it!=(*tech_it)->m_passes.end(); ++pass_it)
		{
			res_texture_list::textures_type const& textures = (*pass_it)->get_ps()->m_textures->m_container;
			
			for (res_texture_list::textures_type::const_iterator tex_it=textures.begin(); tex_it!=textures.end(); ++tex_it)
			{
				if (!*tex_it)
					continue;
				
				u32 current_width  = (*tex_it)->width();
				u32 current_height = (*tex_it)->height();
				
				if (current_width>out_width)
					out_width = current_width;
				
				if (current_height>out_height)
					out_height = current_height;
			}
		}
	}
}

u32 res_effect::get_total_pixel_shader_instruction_count() const
{
	u32 result = 0;
	for (techniques_array_type::const_iterator tech_it=m_techniques.begin(); tech_it!=m_techniques.end(); ++tech_it)
	{
		for (res_shader_technique::passes_array_type::const_iterator pass_it=(*tech_it)->m_passes.begin(); pass_it!=(*tech_it)->m_passes.end(); ++pass_it)
		{
			result += (*pass_it)->get_ps()->hardware_shader()->data().instruction_count;
		}
	}
	return result;
}

} // namespace render 
} // namespace xray 
