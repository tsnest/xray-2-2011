////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/options.h>
#include <xray/console_command.h>
#include <xray/console_command_processor.h>

namespace xray {
namespace render {

using namespace console_commands;

render_cc::render_cc(pcstr define_name):
	m_define_name(define_name)
{
	render_next		= options::ref().first_render_command;
	options::ref().first_render_command = this;
}

class render_cc_bool: public render_cc, public cc_bool 
{
public:
	render_cc_bool(pcstr name, pcstr define_name, bool& value, bool serializable, command_type const command_type):
		cc_bool(name, value, serializable, command_type),
		render_cc(define_name)
	{
	}
	virtual bool fill_macro(shader_macro& out_macro) const
	{
		if (define_name())
		{
			out_macro.definition = cc_bool::m_value ? "1" : "0";
			out_macro.name		 = define_name();
			return true;
		}
		else
			return false;
	}
};// render_cc_bool

class render_cc_float: public render_cc, public cc_float
{
public:
	render_cc_float(pcstr name, pcstr define_name, float& value, float const min, float const max, bool serializable, command_type const command_type):
		cc_float(name, value, min, max, serializable, command_type),
		render_cc(define_name)
	{
	}
	virtual bool fill_macro(shader_macro& out_macro) const
	{
		if (define_name())
		{
			out_macro.definition.assignf("%f", cc_float::m_value);
			out_macro.name		 = define_name();
			return true;
		}
		else
			return false;
	}
};// render_cc_float

class render_cc_u32: public render_cc, public cc_u32
{
public:
	render_cc_u32(pcstr name, pcstr define_name, u32& value, u32 const min, u32 const max, bool serializable, command_type const command_type):
		cc_u32(name, value, min, max, serializable, command_type),
		render_cc(define_name)
	{
	}
	virtual bool fill_macro(shader_macro& out_macro) const
	{
		if (define_name())
		{
			out_macro.definition.assignf("%d", cc_u32::m_value);
			out_macro.name		 = define_name();
			return true;
		}
		else
			return false;
	}
};// render_cc_u32

options::options():
	first_command			(NULL),
	last_command			(NULL),
	first_render_command	(NULL)
{
	register_console_commands();
	set_default_values();
}

void options::set_default_values()
{
	m_enabled_g_stage					= true;
	m_enabled_g_stage_pre_pass			= true;
	m_enabled_g_stage_material_pass		= true;
	m_enabled_decals_accumulate_stage	= true;
	m_enabled_distortion_stage			= true;
	m_enabled_sun_shadows_stage			= true;
	m_enabled_sun_stage					= true;
	m_enabled_lighting_stage			= true;
	m_enabled_forward_lighting_stage	= true;
	m_enabled_deferred_lighting_stage	= true;
	m_enabled_forward_stage				= true;
	m_enabled_particles_stage			= true;
	m_enabled_post_process_stage		= true;
	m_enabled_sky_box_stage				= true;
	m_test_float_option					= 1.0f;
	m_use_parallax						= true;
	m_use_loop_unrolling				= true;
	m_use_branching						= true;
	m_enabled_ambient_occlusion_stage	= true;
	m_enabled_light_propagation_volumes_stage = true;
	m_enabled_mlaa						= true;
	m_use_cpu_mlaa						= false;
	m_shadow_map_size					= 1024;
	m_spot_shadow_map_size				= 1024;
	m_organic_irradiance_texture_size	= 1024;
	m_shadow_quality					= 1;	
	m_gbuffer_pos_packing				= 1;	// enum_gbuffer_pos_packing
	m_gbuffer_normal_packing			= 1;	// enum_gbuffer_normal_packing
	m_enabled_local_light_shadows		= true;
	m_enabled_terrain_shadows			= true;
	m_enabled_draw_terrain				= true;
	m_enabled_draw_speedtree			= true;
	m_enabled_draw_speedtree_billboards	= true;
	m_enabled_draw_speedtree_branches	= true;
	m_enabled_draw_speedtree_fronds		= true;
	m_enabled_draw_speedtree_leafcards	= true;
	m_enabled_draw_speedtree_leafmeshes	= true;
	m_enabled_fxaa						= false;
	
	m_light_propagation_volumes_rsm_size= 128;
	m_num_radiance_volume_cells			= 32;
	m_radiance_volume_scale				= 10.0f;
	m_num_propagate_iterations			= 2;
	m_enabled_lpv_occluders				= true;
	
	m_lpv_flux_amplifier				= 3.5f;
	m_lpv_interreflection_contribution	= 1.5f;
	
	m_lpv_movable						= true;
	
	m_lpv_num_cascades					= 3;
	
	m_lpv_gather_occluders_from_light_view = true;
	m_lpv_gather_occluders_from_camera_view = true;
	
	m_lpv_disable_rsm_generating		= false;
	m_lpv_disable_rsm_downsampling		= false;
	m_lpv_disable_vpl_injection			= false;
	m_lpv_disable_gv_injection			= false;
	m_lpv_disable_propagation			= false;
	m_lpv_disable_lpv_lookup			= false;
	
	m_lpv_refresh_once_per_frames		= 5;
	m_lpv_occlusion_amplifier			= 1.0f;
	
	m_enabled_draw_models				= true;
	m_enabled_clouds_stage				= true;
}

fs_new::virtual_path_string	options::get_current_configuration()
{
	cc_string* command					= static_cast_checked<cc_string*>(xray::console_commands::find("r_current_render_configuration"));
	console_command::status_str			status;
	command->status						(status);
	fs_new::virtual_path_string						file_name;
	file_name.append					(status);
	
	return file_name;
}

string256 s_current_render_configuration = "default";

void options::register_console_commands()
{
	static xray::console_commands::cc_string current_render_configuration_cc("r_current_render_configuration", s_current_render_configuration, 256, true, xray::console_commands::command_type_engine_internal );
	
	
	static render_cc_bool	enabled_g_stage_cc					("r_enabled_g_stage",					0, m_enabled_g_stage,					true, command_type_engine_internal);
	static render_cc_bool	enabled_g_stage_pre_pass_cc			("r_enabled_g_stage_pre_pass",			0, m_enabled_g_stage_pre_pass,			true, command_type_engine_internal);
	static render_cc_bool	enabled_g_stage_material_pass_cc	("r_enabled_g_stage_material_pass",		0, m_enabled_g_stage_material_pass,		true, command_type_engine_internal);
	static render_cc_bool	enabled_decals_accumulate_stage_cc	("r_enabled_decals_accumulate_stage",	0, m_enabled_decals_accumulate_stage,	true, command_type_engine_internal);
	static render_cc_bool	enabled_distortion_stage_cc			("r_enabled_distortion_stage",			0, m_enabled_distortion_stage,			true, command_type_engine_internal);
	static render_cc_bool	enabled_sun_shadows_stage_cc		("r_enabled_sun_shadows_stage",			0, m_enabled_sun_shadows_stage,			true, command_type_engine_internal);
	static render_cc_bool	enabled_sun_stage_cc				("r_enabled_sun_stage",					0, m_enabled_sun_stage,					true, command_type_engine_internal);
	static render_cc_bool	enabled_lighting_stage_cc			("r_enabled_lighting_stage",			0, m_enabled_lighting_stage,			true, command_type_engine_internal);
	static render_cc_bool	enabled_forward_lighting_stage_cc	("r_enabled_forward_lighting_stage",	0, m_enabled_forward_lighting_stage,	true, command_type_engine_internal);
	static render_cc_bool	enabled_deferred_lighting_stage_cc	("r_enabled_deferred_lighting_stage",	0, m_enabled_deferred_lighting_stage,	true, command_type_engine_internal);
	static render_cc_bool	enabled_forward_stage_cc			("r_enabled_forward_stage",				0, m_enabled_forward_stage,				true, command_type_engine_internal);
	static render_cc_bool	enabled_particles_stage_cc			("r_enabled_particles_stage",			0, m_enabled_particles_stage,			true, command_type_engine_internal);
	static render_cc_bool	enabled_post_process_stage_cc		("r_enabled_post_process_stage",		0, m_enabled_post_process_stage,		true, command_type_engine_internal);
	static render_cc_bool	enabled_sky_box_stage_cc			("r_enabled_sky_box_stage",				0, m_enabled_sky_box_stage,				true, command_type_engine_internal);
	static render_cc_bool	enabled_ambient_occlusion_stage_cc	("r_enabled_ambient_occlusion_stage",	0, m_enabled_ambient_occlusion_stage,	true, command_type_engine_internal);
	static render_cc_bool	enabled_clouds_stage_cc				("r_enabled_clouds_stage",				0, m_enabled_clouds_stage,				true, command_type_engine_internal);
	
	// Terrain parameters.
	static render_cc_bool	enabled_draw_terrain_cc				("r_enabled_draw_terrain",				0, m_enabled_draw_terrain,				true, command_type_engine_internal);
	static render_cc_bool	enabled_terrain_shadows_cc			("r_enabled_terrain_shadows",			0, m_enabled_terrain_shadows,			true, command_type_engine_internal);
	
	// SpeedTree parameters.
	static render_cc_bool	enabled_draw_speedtree_cc			("r_enabled_draw_speedtree",			0, m_enabled_draw_speedtree,			true, command_type_engine_internal);
	static render_cc_bool	enabled_draw_speedtree_branches_cc	("r_enabled_draw_speedtree_branches",	0, m_enabled_draw_speedtree_branches,	true, command_type_engine_internal);
	static render_cc_bool	enabled_draw_speedtree_fronds_cc	("r_enabled_draw_speedtree_fronds",		0, m_enabled_draw_speedtree_fronds,		true, command_type_engine_internal);
	static render_cc_bool	enabled_draw_speedtree_leafmeshes_cc("r_enabled_draw_speedtree_leafmeshes",	0, m_enabled_draw_speedtree_leafmeshes,	true, command_type_engine_internal);
	static render_cc_bool	enabled_draw_speedtree_leafcards_cc	("r_enabled_draw_speedtree_leafcards",	0, m_enabled_draw_speedtree_leafcards,	true, command_type_engine_internal);
	static render_cc_bool	enabled_draw_speedtree_billboards_cc("r_enabled_draw_speedtree_billboards",	0, m_enabled_draw_speedtree_billboards,	true, command_type_engine_internal);
	
	// Models parameters.
	static render_cc_bool	enabled_draw_models_cc				("r_enabled_draw_models",				0, m_enabled_draw_models,				true, command_type_engine_internal);
	
	static render_cc_bool	enabled_fxaa_cc						("r_enabled_fxaa",						0, m_enabled_fxaa,						true, command_type_engine_internal);
	static render_cc_bool	enabled_mlaa_cc						("r_enabled_mlaa",						0, m_enabled_mlaa,						true, command_type_engine_internal);
	static render_cc_bool	use_cpu_mlaa_cc						("r_use_cpu_mlaa",						0, m_use_cpu_mlaa,						false, command_type_engine_internal);
	
	// Lighting stage parameters.
	static render_cc_bool	enabled_local_light_shadows_cc		("r_enabled_local_light_shadows",		0, m_enabled_local_light_shadows,		true, command_type_engine_internal);
	
	static render_cc_bool	use_parallax_cc						("r_use_parallax",						"GLOBAL_ALLOW_STEEPPARALLAX",			m_use_parallax, true, command_type_engine_internal);
	static render_cc_bool	use_branching_cc					("r_use_branching",						"GLOBAL_USE_BRANCHING",					m_use_branching, true, command_type_engine_internal);
	static render_cc_bool	use_loop_unrolling_cc				("r_use_loop_unrolling",				"GLOBAL_USE_LOOP_UNROLLING",			m_use_loop_unrolling, true, command_type_engine_internal);
	
	static render_cc_u32	organic_irradiance_texture_size_cc	("r_organic_irradiance_texture_size",	"GLOBAL_ORGANIC_IRRADIANCE_TEXTURE_SIZE",	m_organic_irradiance_texture_size, 128, 2048, true, command_type_engine_internal);
	static render_cc_u32	shadow_map_size_cc					("r_shadow_map_size",					"GLOBAL_SHADOWMAP_SIZE",					m_shadow_map_size, 128, 2048, true, command_type_engine_internal);
	static render_cc_u32	spot_shadow_map_size_cc				("r_spot_shadow_map_size",				"GLOBAL_SPOT_SHADOWMAP_SIZE",				m_spot_shadow_map_size, 128, 2048, true, command_type_engine_internal);
	static render_cc_u32	shadow_quality_cc					("r_shadow_quality",					"GLOBAL_SHADOWMAP_QUALITY",					m_shadow_quality, 0, 2, true, command_type_engine_internal);
	static render_cc_u32	gbuffer_pos_packing_cc				("r_gbuffer_pos_packing",				"GLOBAL_GBUFFER_POS_PACKING",				m_gbuffer_pos_packing, 0, 1, true, command_type_engine_internal);
	static render_cc_u32	gbuffer_normal_packing_cc			("r_gbuffer_normal_packing",			"GLOBAL_GBUFFER_NORMAL_PACKING",			m_gbuffer_normal_packing, 0, 2, true, command_type_engine_internal);
	static render_cc_float  test_float_option_cc				("r_test_float_option",					"GLOBAL_TEST_FLOAT",						m_test_float_option, 0.0f, 100.0f, true, command_type_engine_internal);
	
	// Light Propagation Volumes stage parameters.
	static render_cc_bool	enabled_light_propagation_volumes_stage_cc	("r_enabled_light_propagation_volumes_stage",	0, m_enabled_light_propagation_volumes_stage,	true, command_type_engine_internal);
	static render_cc_u32	light_propagation_volumes_rsm_size_cc		("r_light_propagation_volumes_rsm_size",		0, m_light_propagation_volumes_rsm_size,	1, 2048, true, command_type_engine_internal);
	static render_cc_u32	num_radiance_volume_cells_cc				("r_num_radiance_volume_cells",					0, m_num_radiance_volume_cells,	8, 128, true, command_type_engine_internal);
	static render_cc_u32	num_propagate_iterations_cc					("r_num_propagate_iterations",					0, m_num_propagate_iterations,	0, 32, true, command_type_engine_internal);
	static render_cc_float	radiance_volume_scale_cc					("r_radiance_volume_scale",						0, m_radiance_volume_scale,	1.0f, 100.0f, true, command_type_engine_internal);
	
	static render_cc_float	lpv_flux_amplifier_cc						("r_lpv_flux_amplifier",						0, m_lpv_flux_amplifier,	0.0f, 100.0f, true, command_type_engine_internal);
	static render_cc_float	lpv_interreflection_contribution_cc			("r_lpv_interreflection_contribution",			0, m_lpv_interreflection_contribution,	0.0f, 100.0f, true, command_type_engine_internal);
	static render_cc_bool	lpv_movable_cc								("r_lpv_movable",								0, m_lpv_movable,	true, command_type_engine_internal);
	
	static render_cc_u32	lpv_num_cascades_cc							("r_lpv_num_cascades",							0, m_lpv_num_cascades,	1, 4, true, command_type_engine_internal);
 
	static render_cc_bool	lpv_gather_occluders_from_light_view_cc		("r_lpv_gather_occluders_from_light_view",		0, m_lpv_gather_occluders_from_light_view,	true, command_type_engine_internal);
	static render_cc_bool	lpv_gather_occluders_from_camera_view_cc	("r_lpv_gather_occluders_from_camera_view",		0, m_lpv_gather_occluders_from_camera_view,	true, command_type_engine_internal);
	
	static render_cc_bool	lpv_disable_rsm_generating_cc				("r_lpv_disable_rsm_generating",				0, m_lpv_disable_rsm_generating,	false, command_type_engine_internal);
	static render_cc_bool	lpv_disable_rsm_downsampling_cc				("r_lpv_disable_rsm_downsampling",				0, m_lpv_disable_rsm_downsampling,	false, command_type_engine_internal);
	static render_cc_bool	lpv_disable_vpl_injection_cc				("r_lpv_disable_vpl_injection",					0, m_lpv_disable_vpl_injection,	false, command_type_engine_internal);
	static render_cc_bool	lpv_disable_gv_injection_cc					("r_lpv_disable_gv_injection",					0, m_lpv_disable_gv_injection,	false, command_type_engine_internal);
	static render_cc_bool	lpv_disable_propagation_cc					("r_lpv_disable_propagation",					0, m_lpv_disable_propagation,	false, command_type_engine_internal);
	static render_cc_bool	lpv_disable_lpv_lookup_cc					("r_lpv_disable_lpv_lookup",					0, m_lpv_disable_lpv_lookup,	false, command_type_engine_internal);
	
	static render_cc_u32	lpv_refresh_once_per_frames_cc				("r_lpv_refresh_once_per_frames",				0, m_lpv_refresh_once_per_frames,	1, 20, true, command_type_engine_internal);
	static render_cc_float	lpv_occlusion_amplifier_cc					("r_lpv_occlusion_amplifier",					0, m_lpv_occlusion_amplifier,		0.0f, 100.0f, true, command_type_engine_internal);
	
	first_command				= &enabled_g_stage_cc;
 	console_command* command	= first_command;
 	
	while (command)
 	{
 		if (!command->next())
 		{
 			last_command = command;
 			break;
 		}
 		command = command->next();
 	}
}

void options::save(pcstr file_name)
{
	memory::writer			f(g_allocator);
	console_command* current = first_command;
	save_storage			s(*g_allocator);
	
	while(current)
	{
		if(current->is_serializable())
			current->save_to( s, g_allocator );
		
		if (current == last_command)
			break;
		
		current				= current->next();
	}

	s.save_to				( f );
	f.save_to				( file_name );
}

void options::on_config_loaded( resources::queries_result& data )
{
	if( !data.is_successful( ) )
		return;
	
	resources::pinned_ptr_const<u8> pinned_data	(data[ 0 ].get_managed_resource( ));
	memory::reader				F( pinned_data.c_ptr( ), pinned_data.size( ) );
	load_impl					( F );
}

static bool is_line_term (char a) {	return (a==13)||(a==10); };

static u32	advance_term_string(memory::reader& F)
{
	u32 sz		= 0;
	while (!F.eof()) 
	{
		F.advance(1);				//Pos++;
		sz++;
		if (!F.eof() && is_line_term(*(char*)F.pointer()) ) 
		{
			while(!F.eof() && is_line_term(*(char*)F.pointer())) 
				F.advance(1);		//Pos++;
			break;
		}
	}
	return sz;
}

static void r_string(memory::reader& F, string4096& dest)
{
	char *src 	= (char *) F.pointer();
	u32 sz 		= advance_term_string(F);
	xray::strings::copy_n	(dest, sizeof(dest), src, sz);
}

void options::load_impl(xray::memory::reader& F)
{
	string4096				str;
	while (!F.eof())
	{
		r_string			(F, str);
		xray::console_commands::execute				(str, xray::console_commands::execution_filter_all);
	}
}

void options::load(pcstr file_name)
{
	resources::query_resource	(
		file_name,
		xray::resources::raw_data_class,
		boost::bind(&options::on_config_loaded, this, _1),
		g_allocator
	);
}

void options::fill_global_macros(shader_defines_list& out_defines)
{
	render_cc* current = first_render_command;
	
	while (current)
	{
		shader_macro d;
		
		if (current->fill_macro(d))
			out_defines.push_back(d);
		
		current				= current->render_next;
	}
}

void options::load_from_config(xray::configs::binary_config_value const& config)
{
 	xray::console_commands::console_command* command = first_command;
 	while (command)
 	{
 		if (config.value_exists(command->name()))
 		{
			xray::configs::binary_config_value const& value_config = config[command->name()];
			
 			xray::configs::enum_types type = (xray::configs::enum_types)value_config.type;
 			switch (type)
 			{
 			case xray::configs::t_boolean:
 				command->execute(
 					bool(value_config) ? 
 					"true" : "false"
 					);
 				break;
 			case xray::configs::t_float:
 				{
 					fs_new::virtual_path_string value;
 					value.assignf("%f", float(value_config));
 					command->execute(value.c_str());
 					break;
 				}
 			case xray::configs::t_integer:
 				{
 					fs_new::virtual_path_string value;
 					value.assignf("%d", s32(value_config));
 					command->execute(value.c_str());
 					break;
 				}
 			default: NODEFAULT();
 			};
 		}
		
		if (command == last_command)
			break;
 		command = command->next();
 	}	
}

void options::save_current_configuration()
{
#ifndef MASTER_GOLD
	fs_new::virtual_path_string		file_name;
	file_name.assignf	("%s.cfg", get_current_configuration().c_str());
	save_to_file		(file_name.c_str());
#endif // #ifndef MASTER_GOLD
}

#ifndef MASTER_GOLD
void options::on_config_loaded2(resources::queries_result& data)
{
	if (data.is_successful())
	{
		load_from_config(
			xray::static_cast_resource_ptr<xray::configs::binary_config_ptr>(
				data[0].get_unmanaged_resource()
			)->get_root()["options"]
		);
	}
}

void options::load_from_file(pcstr file_name)
{
	fs_new::virtual_path_string						path;
	path.assignf						("resources/render/%s", file_name);
	
	query_resource						( 
		path.c_str(), 
		resources::binary_config_class, 
		boost::bind( &options::on_config_loaded2, this, _1), 
		g_allocator
	);
}

void options::save_to_file(pcstr file_name)
{
	configs::lua_config_ptr config_ptr	= configs::create_lua_config();
	configs::lua_config_value& root		= config_ptr->get_root();
	
	configs::lua_config_value options	= root["options"];
	save_to_config						( options );
	fs_new::native_path_string			path;
	path.assignf_with_conversion		("resources/render/%s", file_name);
	root.save_as						( path.c_str(), configs::target_sources );
}

void options::save_to_config(xray::configs::lua_config_value& config)
{
	console_command* current = first_command;
	
	while(current)
	{
		console_command::info_str	info;
		console_command::status_str status;
		
		current->info				(info);
		current->status				(status);
		
		pcstr name					= current->name();
		
		if (strings::equal(info, "boolean value."))
		{
			if (strings::equal(status, "on"))
				config[name]		= true;
			else
				config[name]		= false;
		}
		else if (strings::equal(info, "floating point value."))
		{
			float value;
			if (strings::convert_string_to_number(status, &value))
			{
				config[name]		= value;
			}
		}
		else if (strings::equal(info, "unsigned integer value."))
		{
			u32 value;
			if (strings::convert_string_to_number<u32>(status, &value))
			{
				config[name]		= value;
			}
		}
			
		if (current == last_command)
			break;
		
		current				= current->next();
	}
}

#endif // #ifndef MASTER_GOLD

} // namespace render
} // namespace xray
