////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_manager.h"
#include <xray/fs_watcher.h>
#include "render_model.h"
#include "render_particle_emitter_instance.h"
#include <xray/particle/world.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/destroy_data_helper.h>
#include <xray/render/core/effect_options_descriptor.h>
#include "post_process_parameters.h"
#include <xray/render/core/resource_manager.h>
#include "material_effects.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/untyped_buffer.h>

#include <xray/render/facade/material_effects_instance_cook_data.h>

namespace xray {
namespace render {


class material_shader_base: public effect_material_base
{
	material_shader_base(enum_vertex_input_type /*vertex_input_type*/, material_ptr /*mtl*/)
	{
		
	}
	u32				m_vertex_type;
	material_ptr	m_mtl;
	ref_effect		m_shader;
};

material_manager::material_manager()
{
#ifndef MASTER_GOLD
	m_watcher_materials_subscribe_id	=	xray::resources::subscribe_watcher(get_materials_path(), 
											boost::bind(&material_manager::on_material_source_changed, this, _1));

	m_watcher_material_instanced_subscribe_id	=	
											xray::resources::subscribe_watcher(get_material_instances_path(), 
											boost::bind(&material_manager::on_material_source_changed, this, _1));
#endif // #ifndef MASTER_GOLD
}

material_manager::~material_manager()
{
#ifndef MASTER_GOLD
	xray::resources::unsubscribe_watcher	(m_watcher_materials_subscribe_id);
	xray::resources::unsubscribe_watcher	(m_watcher_material_instanced_subscribe_id);
#endif // #ifndef MASTER_GOLD

	{
		material_effects_entries_type	temp_material_effects;
		m_material_effects.swap			(temp_material_effects);
		m_material_effects.clear		();
	}
//	resources::unregister_cook(resources::material_effects_instance_class);
}

static pcstr resources_materials_string						=	"resources/materials";
static pcstr resources_material_instances_string			=	"resources/material_instances";
static pcstr resources_materials_string_with_slash			=	"resources/materials/";
static pcstr resources_material_instances_string_with_slash =	"resources/material_instances/";


pcstr material_manager::get_materials_path() const
{
	return resources_materials_string;
}

pcstr material_manager::get_material_instances_path() const
{
	return resources_material_instances_string;
}


void material_manager::get_output_text(fs_new::virtual_path_string * out_text, u32& num_lines)
{
	num_lines = 0;
	
	for (material_effects_entries_type::iterator it = m_material_effects.begin(); 
												 it != m_material_effects.end(); 
												 ++it)
	{
		out_text->assignf("[%d] %s", it->m_material_effects_instance_ptr->c_ptr(), it->m_material_name.c_str());
		
		out_text++;
		num_lines++;
		if (num_lines==100)
			break;
	}
}

#ifndef MASTER_GOLD
static xray::fs_new::virtual_path_string get_short_material_name(pcstr name)
{
	xray::fs_new::virtual_path_string result = name;
	result.replace	(resources_materials_string_with_slash, "");
	result.replace	(resources_material_instances_string_with_slash, "");
	
	xray::fs_new::virtual_path_string::size_type pos = result.rfind('.');
	if (pos!=xray::fs_new::virtual_path_string::npos)
		result.set_length(pos);
	
	return result;
}
#endif // #ifndef MASTER_GOLD

void material_manager::on_material_effects_instance_loaded(xray::resources::queries_result& data, material_effects_instance_ptr* mtl_effects_instance_ptr)
{
	if (!data[0].is_successful())
		return;
	
	//pcstr requested_path		= data[0].get_requested_path();
	*mtl_effects_instance_ptr	= static_cast_resource_ptr<material_effects_instance_ptr>(data[0].get_unmanaged_resource());
	
	//mtl_effects_instance_ptr->set_material_effects		(new_material->get_material_effects());
}

void material_manager::on_material_loaded(xray::resources::queries_result& data, 
										  post_process_parameters* in_out_post_process_parameters)
{
	if (!data[0].is_successful())
		return;
	
	initialize_post_process_parameters(
		in_out_post_process_parameters, 
		static_cast_resource_ptr<material_ptr>(data[0].get_unmanaged_resource()),
		false
	);
}

#ifndef MASTER_GOLD

void material_manager::update_material_from_config(fs_new::virtual_path_string const& material_name,  xray::configs::lua_config_value const& config_value)
{
	on_material_source_changed(material_name, create_material(material_name, config_value));
}

material_ptr material_manager::create_material(fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value)
{
	material_ptr								out_material_ptr;
	
	resources::user_data_variant				user_data;
	
	configs::binary_config_ptr binary_config	= xray::configs::create_binary_config(config_value);
	user_data.set								(binary_config);
	
	query_resource_and_wait						(
		material_name.c_str(), 
		resources::material_class, 
		boost::bind(
			&material_manager::on_material_created, 
			this, 
			_1, 
			&out_material_ptr
		),
		render::g_allocator,
		&user_data
	);
	
	return out_material_ptr;
}

void material_manager::on_material_created(xray::resources::queries_result& data, material_ptr* out_material_ptr)
{
	if (data[0].is_successful())
	{
		*out_material_ptr = static_cast_resource_ptr<material_ptr>(data[0].get_unmanaged_resource());
	}
	else
	{
		*out_material_ptr = NULL;
	}
}


void material_manager::on_material_source_changed(fs_new::virtual_path_string const& material_name, material_ptr const& in_material)
{
	bool found_in_effects = false;
	for (material_effects_entries_type::iterator it = m_material_effects.begin(); 
												 it != m_material_effects.end(); 
												 ++it)
	{
		if (it->m_material_name == material_name)
		{
			resources::user_data_variant user_data;
			user_data.set					(
				NEW(material_effects_instance_cook_data)(
					(*it->m_material_effects_instance_ptr)->get_material_effects().get_vertex_input_type(),
					in_material.c_ptr() ? static_cast_resource_ptr<resources::unmanaged_resource_ptr>(in_material) : NULL
				)
			);
			
			query_resource_and_wait			(
 				material_name.c_str(), 
 				resources::material_effects_instance_class, 
 				boost::bind(
					&material_manager::on_material_effects_instance_loaded, 
					this, 
					_1, 
					it->m_material_effects_instance_ptr
				),
 				render::g_allocator,
				&user_data
 			);
			
			found_in_effects				= true;
		}
	}
	
	if (!found_in_effects)
	{
		post_process_parameters_type::iterator it_begin = m_post_process_parameters.begin();
		post_process_parameters_type::iterator it_end	= m_post_process_parameters.end();
		post_process_parameters_type::iterator it		= it_begin;
		
		for ( ; it != it_end; ++it)
		{
			if (it->second != material_name)
				continue;
			
			if (!in_material.c_ptr())
			{
				query_resource_and_wait			(
					material_name.c_str(), 
					resources::material_class, 
					boost::bind(
						&material_manager::on_material_loaded, 
						this, 
						_1, 
						it->first
					),
					render::g_allocator
				);
			}
			else
			{
				initialize_post_process_parameters(
					it->first, 
					in_material,
					false
				);
			}
		}
	}
}

// FIX: on_material_source_changed calling twice
void material_manager::on_material_source_changed(xray::vfs::vfs_notification const & info)
{
	if(info.type!=xray::vfs::vfs_notification::type_modified)
		return;
	
	xray::fs_new::virtual_path_string short_file_name	= get_short_material_name(info.virtual_path);
	on_material_source_changed				(short_file_name);
}
#endif // #ifndef MASTER_GOLD

void material_manager::remove_post_process_parameters(post_process_parameters* in_post_process_parameters)
{
	post_process_parameters_type::iterator found		= m_post_process_parameters.find(in_post_process_parameters);
	
	if (found != m_post_process_parameters.end())
		m_post_process_parameters.erase(found);
}

void material_manager::initialize_post_process_parameters(post_process_parameters* out_post_process_parameters_ptr, material_ptr mtl, bool force_recompilation)
{
	XRAY_UNREFERENCED_PARAMETER									(force_recompilation);
	
	//remove_post_process_parameters								(out_post_process_parameters_ptr);
	
	u32 crc														= 0;
	custom_config_ptr material_config_ptr						= create_custom_config(mtl->m_config->get_root()["material"], crc, false);
	custom_config_value const& material_config					= material_config_ptr->root();
	
	if (!material_config.value_exists("post_process"))
		return;
	
	fs_new::virtual_path_string mtl_name						= mtl->get_material_name();
	
	m_post_process_parameters[out_post_process_parameters_ptr]	= mtl_name;
	
	custom_config_value const& config							= material_config["post_process"];
	
	post_process_parameters& out_post_process_parameters		= *out_post_process_parameters_ptr;
	out_post_process_parameters									=  post_process_parameters();
	
	if (config.value_exists("dof_height_lights"))
		out_post_process_parameters.dof_height_lights			= float3(config["dof_height_lights"]["value"]);
	
	if (config.value_exists("dof_focus_power"))
		out_post_process_parameters.dof_focus_power				= float(config["dof_focus_power"]["value"]);
	
	if (config.value_exists("dof_focus_region"))
		out_post_process_parameters.dof_focus_region			= float(config["dof_focus_region"]["value"]);

	if (config.value_exists("dof_focus_distance"))
		out_post_process_parameters.dof_focus_distance			= float(config["dof_focus_distance"]["value"]);

	if (config.value_exists("dof_near_blurriness_amount"))
		out_post_process_parameters.dof_near_blur_amount		= float(config["dof_near_blurriness_amount"]["value"]);

	if (config.value_exists("dof_near_blurriness_far"))
		out_post_process_parameters.dof_far_blur_amount			= float(config["dof_near_blurriness_far"]["value"]);
	
	if (config.value_exists("use_bokeh_dof"))
		out_post_process_parameters.use_bokeh_dof				= bool(config["use_bokeh_dof"]["value"]);
	
	if (config.value_exists("bokeh_dof_radius"))
		out_post_process_parameters.bokeh_dof_radius			= float(config["bokeh_dof_radius"]["value"]);

	if (config.value_exists("bokeh_dof_density"))
		out_post_process_parameters.bokeh_dof_density			= float(config["bokeh_dof_density"]["value"]);

	if (config.value_exists("use_bokeh_image"))
		out_post_process_parameters.use_bokeh_image				= bool(config["use_bokeh_image"]["value"]);
	
	if (config.value_exists("enable_ssao"))
		out_post_process_parameters.enable_ssao					= bool(config["enable_ssao"]["value"]);
	
	if (config.value_exists("ssao_saturation"))
		out_post_process_parameters.ssao_saturation				= float(config["ssao_saturation"]["value"]);
	
	if (config.value_exists("ssao_radius_scale"))
		out_post_process_parameters.ssao_radius_scale			= float(config["ssao_radius_scale"]["value"]);
	
	if (config.value_exists("bloom_scale"))
		out_post_process_parameters.bloom_scale					= float(config["bloom_scale"]["value"]);

	if (config.value_exists("bloom_max_color"))
		out_post_process_parameters.bloom_max_color				= float(config["bloom_max_color"]["value"]);

	if (config.value_exists("bloom_blurring_kernel"))
		out_post_process_parameters.bloom_blurring_kernel		= float(config["bloom_blurring_kernel"]["value"]);

	if (config.value_exists("bloom_blurring_intencity"))
		out_post_process_parameters.bloom_blurring_intencity	= float(config["bloom_blurring_intencity"]["value"]);
	
	
	if (config.value_exists("frame_desaturation"))
		out_post_process_parameters.frame_desaturation			= float(config["frame_desaturation"]["value"]);

	if (config.value_exists("frame_height_lights"))
		out_post_process_parameters.frame_height_lights			= float3(config["frame_height_lights"]["value"]);
	
	if (config.value_exists("frame_mid_tones"))
		out_post_process_parameters.frame_mid_tones				= float3(config["frame_mid_tones"]["value"]);

	if (config.value_exists("frame_shadows"))
		out_post_process_parameters.frame_shadows				= float3(config["frame_shadows"]["value"]);

	if (config.value_exists("frame_gamma_correction_factor"))
		out_post_process_parameters.frame_gamma_correction_factor	= float(config["frame_gamma_correction_factor"]["value"]);
	

	if (config.value_exists("environment_far_fog_color") &&
		config.value_exists("environment_far_fog_distance") &&
		config.value_exists("environment_near_fog_distance"))
	{
		out_post_process_parameters.environment_far_fog_color		= float4(config["environment_far_fog_color"]["value"]).xyz();
		out_post_process_parameters.environment_far_fog_distance	= float(config["environment_far_fog_distance"]["value"]);
		out_post_process_parameters.environment_near_fog_distance	= float(config["environment_near_fog_distance"]["value"]);
	}
	
	if (config.value_exists("environment_ambient_color"))
		out_post_process_parameters.environment_ambient_color		= float4(config["environment_ambient_color"]["value"]).xyz();
	
	if (config.value_exists("environment_sun_color"))
		out_post_process_parameters.environment_sun_color			= float4(config["environment_sun_color"]["value"]).xyz();
	
	if (config.value_exists("environment_shadow_transparency"))
		out_post_process_parameters.environment_shadow_transparency	= float(config["environment_shadow_transparency"]["value"]);
	
	if (config.value_exists("environment_skylight_upper_color"))
		out_post_process_parameters.environment_skylight_upper_color	= float4(config["environment_skylight_upper_color"]["value"]);	
	
	if (config.value_exists("environment_skylight_lower_color"))
		out_post_process_parameters.environment_skylight_lower_color	= float4(config["environment_skylight_lower_color"]["value"]);	

	if (
		config.value_exists("environment_skylight_upper_power")	&&
		config.value_exists("environment_skylight_lower_power")
		)
		out_post_process_parameters.environment_skylight_parameters		= float4(
			float(config["environment_skylight_upper_power"]["value"]),
			float(config["environment_skylight_lower_power"]["value"]),
			0.0f,
			0.0f
		);	
	
	if (config.value_exists("adaptation_speed"))
		out_post_process_parameters.adaptation_speed			= math::max(float(config["adaptation_speed"]["value"]), 0.25f);
	
	if (config.value_exists("tonemap_bright_threshold"))
		out_post_process_parameters.tonemap_bright_threshold	= float(config["tonemap_bright_threshold"]["value"]);
	if (config.value_exists("tonemap_median"))
		out_post_process_parameters.tonemap_median				= float(config["tonemap_median"]["value"]);
	if (config.value_exists("tonemap_darkness_threshold"))
		out_post_process_parameters.tonemap_darkness_threshold	= float(config["tonemap_darkness_threshold"]["value"]);
	if (config.value_exists("tonemap_middle_gray"))
		out_post_process_parameters.tonemap_middle_gray			= float(config["tonemap_middle_gray"]["value"]);
	
	
	
	if (config.value_exists("wind_direction")) 
		out_post_process_parameters.wind_direction				= float3(config["wind_direction"]["value"]);
	
	if (config.value_exists("wind_strength")) 
		out_post_process_parameters.wind_strength				= float(config["wind_strength"]["value"]);

	if (config.value_exists("enable_bloom"))
		out_post_process_parameters.enable_bloom				= bool(config["enable_bloom"]["value"]);

	if (config.value_exists("enable_msaa"))
		out_post_process_parameters.use_msaa					= bool(config["enable_msaa"]["value"]);
	
	
	if (config.value_exists("environment_skycolor_up"))
		out_post_process_parameters.environment_skycolor[3]		= float4(config["environment_skycolor_up"]["value"]);
	
	if (config.value_exists("environment_skycolor_x"))
		out_post_process_parameters.environment_skycolor[1]		= float4(config["environment_skycolor_x"]["value"]);
	
	if (config.value_exists("environment_skycolor_minus_x"))
		out_post_process_parameters.environment_skycolor[0]		= float4(config["environment_skycolor_minus_x"]["value"]);
	
	if (config.value_exists("environment_skycolor_z"))
		out_post_process_parameters.environment_skycolor[5]		= float4(config["environment_skycolor_z"]["value"]);
	
	if (config.value_exists("environment_skycolor_minus_z"))
		out_post_process_parameters.environment_skycolor[4]		= float4(config["environment_skycolor_minus_z"]["value"]);
	
	if (config.value_exists("environment_use_skycolor"))
		out_post_process_parameters.use_environment_skycolor	= bool(config["environment_use_skycolor"]["value"]);
	
#pragma message( XRAY_TODO("Add color_grading_texture to post_process_parameters in other platforms") )
	
	if (config.value_exists("use_color_grading_lut"))
		out_post_process_parameters.use_color_grading_lut		= bool(config["use_color_grading_lut"]["value"]);
	
#if XRAY_PLATFORM_WINDOWS
	if (config.value_exists("color_grading_texture"))
		out_post_process_parameters.color_grading_texture		= resource_manager::ref().create_texture(pcstr(config["color_grading_texture"]["value"]));
#endif // #if XRAY_PLATFORM_WINDOWS
}

void material_manager::remove_material_effects(material_effects_instance_ptr const& in_material_effects_instance)
{
	material_effects_entries_type::iterator it_begin	= m_material_effects.begin();
	material_effects_entries_type::iterator it_end		= m_material_effects.end();
	material_effects_entries_type::iterator it			= it_begin;
	
	for ( ; it != it_end; ++it)
	{
		if (*it->m_material_effects_instance_ptr == in_material_effects_instance)
			m_material_effects.erase(it);
	}
}

void material_manager::add_material_effects(material_effects_instance_ptr& in_material_effects_instance, 
											fs_new::virtual_path_string const& in_material_name)
{
	material_effects_entries_type::iterator it_begin	= m_material_effects.begin();
	material_effects_entries_type::iterator it_end		= m_material_effects.end();
	material_effects_entries_type::iterator it			= it_begin;
	
	for ( ; it != it_end; ++it)
	{
		if (*it->m_material_effects_instance_ptr == in_material_effects_instance)
			return;
	}
	
	m_material_effects.push_back(
		material_effects_entry(
			&in_material_effects_instance, 
			in_material_name
		)
	);
}

} // namespace render 
} // namespace xray 

