////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/shader_macros.h>
#include <xray/render/core/options.h>

namespace xray {
namespace render {	

pcstr const str_global_gbuffer_pos_packing			=	"GLOBAL_GBUFFER_POS_PACKING";
pcstr const str_global_gbuffer_normal_packing		=	"GLOBAL_GBUFFER_NORMAL_PACKING";
pcstr const str_global_fp16_blend					=	"GLOBAL_FP16_BLEND";
pcstr const str_global_use_branching				=	"GLOBAL_USE_BRANCHING";
pcstr const str_global_allow_steepparallax			=	"GLOBAL_ALLOW_STEEPPARALLAX";
pcstr const str_global_master_gold					=	"GLOBAL_MASTER_GOLD";

pcstr const str_cfg_use_diffuse						=	"CONFIG_TDIFFUSE";
pcstr const str_cfg_use_anisotropic_direction		=	"CONFIG_TANISOTROPIC_DIRECTION";
pcstr const str_cfg_use_normal						=	"CONFIG_TNORMAL";
pcstr const str_cfg_use_detail_normal				=	"CONFIG_TDETAIL_NORMAL";
pcstr const str_cfg_use_alpha_test					=	"CONFIG_ALPHA_TEST";
pcstr const str_cfg_use_parallax					=	"CONFIG_PARALLAX";
pcstr const str_cfg_use_trancluency_texture			=	"CONFIG_TRANSLUCENCY";
pcstr const str_cfg_use_specular_intensity_texture	=	"CONFIG_TSPECULAR_INTENSITY";
pcstr const str_cfg_use_specular_power_texture		=	"CONFIG_TSPECULAR_POWER";
pcstr const str_cfg_is_anisotropic_material			=	"CONFIG_ANISOTROPIC_MATERIAL";
pcstr const str_cfg_use_emissive					=	"CONFIG_EMISSIVE";
pcstr const str_cfg_use_diffuse_power_texture		=	"CONFIG_TDIFFUSE_POWER";
pcstr const str_cfg_use_detail						=	"CONFIG_TDETAIL";
pcstr const str_cfg_use_detail_bump					=	"CONFIG_TDETAIL_BUMP";
pcstr const str_cfg_use_subuv						=	"CONFIG_USE_SUBUV";
pcstr const str_cfg_use_reflection					=	"CONFIG_REFLECTION";
pcstr const str_cfg_use_ttransparency				=	"CONFIG_TTRANSPARENCY";
pcstr const str_cfg_use_tfresnel_at_0_degree		=	"CONFIG_TFRESNEL_AT_0_DEGREE";
pcstr const str_cfg_light_type						=	"CONFIG_LIGHT_TYPE";
pcstr const str_cfg_vertex_input_type				=	"CONFIG_VERTEX_INPUT_TYPE";
pcstr const str_cfg_use_variation_mask				=	"CONFIG_VARIATION_MASK";
pcstr const str_cfg_use_shadowed_light				=	"CONFIG_SHADOWED_LIGHT";
pcstr const str_cfg_use_bokeh_dof					=	"CONFIG_USE_BOKEH_DOF";
pcstr const str_cfg_use_bokeh_image					=	"CONFIG_USE_BOKEH_IMAGE";
pcstr const str_cfg_wind_motion						=	"CONFIG_WIND_MOTION";
pcstr const str_cfg_lighting_model					=	"CONFIG_LIGHTING_MODEL";
pcstr const str_cfg_decal_material					=	"CONFIG_DECAL_MATERIAL";
pcstr const str_cfg_decal_type						=	"CONFIG_DECAL_TYPE";
pcstr const str_cfg_reflection_mask					=	"CONFIG_REFLECTION_MASK";

pcstr const str_cfg_use_ao_texture					=	"CONFIG_USE_AO_TEXTURE";
pcstr const str_cfg_use_sequence					=	"CONFIG_SEQUENCE";

// for organic materials (skin, body, meat, etc)
pcstr const str_cfg_use_organic_scattering_amount_mask		= "CONFIG_USE_ORGANIC_SCATTERING_AMOUNT_MASK";
pcstr const str_cfg_use_organic_scattering_depth_texture	= "CONFIG_USE_ORGANIC_SCATTERING_DEPTH_TEXTURE";
pcstr const str_cfg_use_organic_back_illumination_texture	= "CONFIG_USE_ORGANIC_BACK_ILLUMINATION_TEXTURE";
pcstr const str_cfg_use_organic_subdermal_texture			= "CONFIG_USE_ORGANIC_SUBDERMAL_TEXTURE";


namespace { 

inline void add_macro( shader_defines_list&  macros, pcstr name, pcstr value)
{
	macros.push_back( shader_macro() );
	macros.back().name       = name;
	macros.back().definition = value;
}

inline void add_bool_macro( shader_defines_list&  macros, pcstr name, bool flag)
{
	//if( !flag)
	//	return;

	macros.push_back( shader_macro() );
	macros.back().name       = name;
	macros.back().definition = flag ? "1" : "0";
}

inline void add_u5_macro( shader_defines_list&  macros, pcstr name, u8 value)
{
	shader_macro macro;
	macro.name       = name;
	switch( value)
	{
		case 0 :  macro.definition = "0"; break;
		case 1 :  macro.definition = "1"; break;
		case 2 :  macro.definition = "2"; break;
		case 3 :  macro.definition = "3"; break;
		case 4 :  macro.definition = "4"; break;
		case 5 :  macro.definition = "5"; break;
		case 6 :  macro.definition = "6"; break;
		case 7 :  macro.definition = "7"; break;
		case 8 :  macro.definition = "8"; break;
		case 9 :  macro.definition = "9"; break;
		case 10 :  macro.definition = "10"; break;
		case 11 :  macro.definition = "11"; break;
		case 12 :  macro.definition = "12"; break;
		case 13 :  macro.definition = "13"; break;
		case 14 :  macro.definition = "14"; break;
		case 15 :  macro.definition = "15"; break;
		case 16 :  macro.definition = "16"; break;
		case 17 :  macro.definition = "17"; break;
		case 18 :  macro.definition = "18"; break;
		case 19 :  macro.definition = "19"; break;
		case 20 :  macro.definition = "20"; break;
		case 21 :  macro.definition = "21"; break;
		case 22 :  macro.definition = "22"; break;
		case 23 :  macro.definition = "23"; break;
		case 24 :  macro.definition = "24"; break;
		case 25 :  macro.definition = "25"; break;
		case 26 :  macro.definition = "26"; break;
		case 27 :  macro.definition = "27"; break;
		case 28 :  macro.definition = "28"; break;
		case 29 :  macro.definition = "29"; break;
		case 30 :  macro.definition = "30"; break;
		case 31 :  macro.definition = "31"; break;
		
		default : ASSERT( 0, "Values greather that 31 are not supported." ); break;
	}
	macros.push_back( macro);
}


} // namespace 


shader_macros::shader_macros()
{
	register_available_macros();
}

void shader_macros::merge_with_declared_macroses( shader_declarated_macroses_list const& declared_macroses, shader_defines_list&  macros)
{
	for (shader_defines_list::iterator define_it = macros.begin(); define_it!=macros.end(); ++define_it)
	{
		if (!define_it->name.length())
			continue;
		
		bool found = false;
		for (shader_declarated_macroses_list::const_iterator decl_it = declared_macroses.begin(); decl_it!=declared_macroses.end(); ++decl_it)
		{
			if (strings::compare(define_it->name.c_str(), (*decl_it).c_str())==0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			//LOG_ERROR("definition '%s' doesn't exist in declared defines, but used in shader.", define_it->Name);
			//macros.erase(define_it);
		}
	}
}

struct shader_macros_dort_predicate
{
	inline bool operator () ( char const * first, char const * second)
	{
		return strcmp( first, second) < 0;
	}
};

void shader_macros::register_available_macros()
{
	// global flags
	m_available_macros.push_back(str_global_gbuffer_pos_packing);
	m_available_macros.push_back(str_global_gbuffer_normal_packing);
	m_available_macros.push_back(str_global_fp16_blend);
	m_available_macros.push_back(str_global_use_branching);
	m_available_macros.push_back(str_global_allow_steepparallax);
	m_available_macros.push_back(str_global_master_gold);
	
	// configuration flags
	m_available_macros.push_back(str_cfg_use_diffuse);
	m_available_macros.push_back(str_cfg_use_anisotropic_direction);
	m_available_macros.push_back(str_cfg_use_normal);
	m_available_macros.push_back(str_cfg_use_detail_normal);
	m_available_macros.push_back(str_cfg_use_alpha_test);
	m_available_macros.push_back(str_cfg_use_parallax);
	m_available_macros.push_back(str_cfg_use_trancluency_texture);
	m_available_macros.push_back(str_cfg_use_specular_intensity_texture);
	m_available_macros.push_back(str_cfg_use_specular_power_texture);
	m_available_macros.push_back(str_cfg_use_emissive);
	m_available_macros.push_back(str_cfg_use_diffuse_power_texture);
	m_available_macros.push_back(str_cfg_use_detail);
	m_available_macros.push_back(str_cfg_use_detail_bump);
	m_available_macros.push_back(str_cfg_use_subuv);
	m_available_macros.push_back(str_cfg_use_reflection);
	m_available_macros.push_back(str_cfg_use_ttransparency);
	m_available_macros.push_back(str_cfg_use_tfresnel_at_0_degree);
	m_available_macros.push_back(str_cfg_light_type);
	m_available_macros.push_back(str_cfg_vertex_input_type);
	
	m_available_macros.push_back(str_cfg_wind_motion);
	
	m_available_macros.push_back(str_cfg_use_variation_mask);
	m_available_macros.push_back(str_cfg_use_shadowed_light);
	m_available_macros.push_back(str_cfg_use_bokeh_dof);
	m_available_macros.push_back(str_cfg_use_bokeh_image);
	
	m_available_macros.push_back(str_cfg_lighting_model);
	m_available_macros.push_back(str_cfg_decal_material);
	m_available_macros.push_back(str_cfg_decal_type);
	
	m_available_macros.push_back(str_cfg_is_anisotropic_material);
	
	m_available_macros.push_back(str_cfg_use_ao_texture);
	
	m_available_macros.push_back(str_cfg_use_organic_scattering_amount_mask);
	m_available_macros.push_back(str_cfg_use_organic_scattering_depth_texture);
	m_available_macros.push_back(str_cfg_use_organic_back_illumination_texture);
	m_available_macros.push_back(str_cfg_use_organic_subdermal_texture);

	m_available_macros.push_back(str_cfg_use_sequence);

	m_available_macros.push_back(str_cfg_reflection_mask);
	
	std::sort( m_available_macros.begin(), m_available_macros.end(), shader_macros_dort_predicate());
}

void	shader_macros::fill_shader_macro_list		( shader_defines_list&  macros, shader_configuration shader_config)
{
	macros.clear();
	
	fill_global_macros( macros);
	fill_shader_configuration_macros ( macros, shader_config);
//	
//	macros.push_back( shader_macro() );
//	macros.back().name			= 0;
//	macros.back().definition	= 0;
}

void	shader_macros::fill_shader_configuration_macros ( shader_defines_list&  macros, shader_configuration shader_config)
{
//#ifdef MASTER_GOLD
	add_bool_macro  ( macros, str_global_master_gold,					true);
//#endif // #ifdef MASTER_GOLD
	
	add_bool_macro 	(macros, str_cfg_use_diffuse,						shader_config.use_diffuse_texture);
	add_bool_macro 	(macros, str_cfg_use_normal,						shader_config.use_normal_texture);
	add_bool_macro 	(macros, str_cfg_use_detail_normal,					shader_config.use_detail_normal_texture);
	add_bool_macro 	(macros, str_cfg_use_alpha_test,					shader_config.use_alpha_test);
	add_bool_macro 	(macros, str_cfg_use_parallax, 						shader_config.use_parallax);
	add_bool_macro 	(macros, str_cfg_use_trancluency_texture,			shader_config.use_transluceny_texture);
	add_bool_macro 	(macros, str_cfg_use_specular_intensity_texture,	shader_config.use_specular_intensity_texture);
	add_bool_macro 	(macros, str_cfg_use_specular_power_texture,		shader_config.use_specular_power_texture);
	add_u5_macro	(macros, str_cfg_use_emissive,						shader_config.use_emissive);
	add_bool_macro 	(macros, str_cfg_use_diffuse_power_texture,			shader_config.use_diffuse_power_texture);
	add_bool_macro 	(macros, str_cfg_use_detail,						shader_config.use_detail_texture);
	add_bool_macro 	(macros, str_cfg_use_subuv,							shader_config.use_subuv);
	add_bool_macro 	(macros, str_cfg_use_ttransparency,					shader_config.use_transparency_texture);
	add_u5_macro 	(macros, str_cfg_use_reflection,					shader_config.use_reflection);
	add_bool_macro 	(macros, str_cfg_use_tfresnel_at_0_degree,			shader_config.use_tfresnel_at_0_degree);
	add_u5_macro 	(macros, str_cfg_light_type,						shader_config.light_type);
	add_u5_macro 	(macros, str_cfg_vertex_input_type,					shader_config.vertex_input_type);
	add_bool_macro 	(macros, str_cfg_use_bokeh_dof,						shader_config.use_bokeh_dof);
	add_bool_macro 	(macros, str_cfg_use_bokeh_image,					shader_config.use_bokeh_image);
	add_bool_macro 	(macros, str_cfg_use_variation_mask,				shader_config.use_variation_mask);
	add_bool_macro 	(macros, str_cfg_use_shadowed_light,				shader_config.shadowed_light);
	add_u5_macro 	(macros, str_cfg_wind_motion,						shader_config.wind_motion);
	add_bool_macro 	(macros, str_cfg_decal_material,					shader_config.decal_material);
	add_u5_macro 	(macros, str_cfg_decal_type,						shader_config.decal_type);
	add_bool_macro 	(macros, str_cfg_use_anisotropic_direction,			shader_config.use_anisotropic_direction_texture);
	add_bool_macro 	(macros, str_cfg_is_anisotropic_material,			shader_config.is_anisotropic_material);
	
	add_bool_macro 	(macros, str_cfg_use_ao_texture,					shader_config.use_ao_texture);
	
	add_bool_macro 	(macros, str_cfg_use_organic_scattering_amount_mask,	shader_config.use_organic_scattering_amount_mask);
	add_bool_macro 	(macros, str_cfg_use_organic_scattering_depth_texture,	shader_config.use_organic_scattering_depth_texture);
	add_bool_macro 	(macros, str_cfg_use_organic_back_illumination_texture,	shader_config.use_organic_back_illumination_texture);
	add_bool_macro 	(macros, str_cfg_use_organic_subdermal_texture,			shader_config.use_organic_subdermal_texture);

	add_bool_macro 	(macros, str_cfg_use_sequence,						shader_config.use_sequence);

	add_bool_macro 	(macros, str_cfg_reflection_mask,					shader_config.use_reflection_mask);

	
}

void	shader_macros::fill_global_macros( shader_defines_list& defines)
{
	xray::render::options::ref().fill_global_macros(defines);
}


} // namespace render
} // namespace xray
