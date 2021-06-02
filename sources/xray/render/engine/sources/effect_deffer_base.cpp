////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/material_manager.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/engine/effect_deffer_base.h>
#include <xray/render/core/backend.h>

namespace xray {
namespace render_dx10 {

template<typename T1, typename T2>
void add_param( T1& str, const T2& param)
{
	if ( !str.empty())
		str += ",";

	str += param;
}

void define_macro( shader_defines_list& defines, const char* param)
{
	define_macro( defines, param, "1");
}

void define_macro( shader_defines_list& defines, const char* param, const char* value)
{
	D3D_SHADER_MACRO	macro;
	macro.Name = param;
	macro.Definition = value;
	defines.push_back( macro);
}

effect_deffer_base::effect_deffer_base( bool use_detail, bool use_bump, bool use_steep_plax):
  m_use_bump( true/*use_bump*/),
  m_use_steep_plax( true/*use_steep_plax*/),
  m_use_detail( false/*use_detail*/)
{
	XRAY_UNREFERENCED_PARAMETERS	( use_detail, use_bump, use_steep_plax);


	static bool samplers_registered = false;
	if( !samplers_registered)
	{
		samplers_registered = true;
		sampler_state_descriptor sampler_sim;
		sampler_sim.set( D3D_FILTER_ANISOTROPIC, D3D_TEXTURE_ADDRESS_WRAP);
		backend::ref().register_sampler( "s_bump", resource_manager::ref().create_sampler_state( sampler_sim));
		backend::ref().register_sampler( "s_bumpX", resource_manager::ref().create_sampler_state( sampler_sim));
		backend::ref().register_sampler( "s_bumpD", resource_manager::ref().create_sampler_state( sampler_sim));
		//backend::ref().register_sampler( "s_detail", resource_manager::ref().create_sampler_state( sampler_sim));
		backend::ref().register_sampler( "s_detailBump", resource_manager::ref().create_sampler_state( sampler_sim));
		backend::ref().register_sampler( "s_detailBumpX", resource_manager::ref().create_sampler_state( sampler_sim));

		sampler_sim.set( D3D_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D_TEXTURE_ADDRESS_CLAMP);
		backend::ref().register_sampler( "s_hemi", resource_manager::ref().create_sampler_state( sampler_sim));
	}

	//add_property( pid_material_name,pt_string);
	//add_property( pid_lmap_name, pt_string);
	//add_property( pid_use_bump, pt_bool);
	//add_property( pid_use_steep_paralax, pt_bool);
	//add_property( pid_use_detail, pt_bool);
}

//REQUIRE FURTHER REFACTOR!!!!!!!!!!!!!
void effect_deffer_base::uber_deffer( effect_compiler& compiler,LPCSTR vs_base, LPCSTR ps_base, bool is_hq, bool is_aref, const effect_compilation_options& options)
{
	shader_defines_list	defines;

	bool has_lmap = false;

	//lmap does not work
	pcstr lmap_name = get<pcstr>( pid_lmap_name);
	if ( lmap_name)
	{
		has_lmap = true;
	}

	//pcstr mat_name = get<pcstr>( pid_material_name);
	//ASSERT( mat_name);
	ASSERT( options.tex_list.size());
	shared_string mat_name = options.tex_list[0];
	ref_material mat = material_manager::ref().get_material( mat_name);

	bool has_bump = m_use_bump && mat->has_bump();
	const char* bump_name = mat->get_bump_name().c_str();
	fixed_string<128> ext_bump_name( bump_name);
	ext_bump_name += "#";
	
	bool use_steep = false;
	if ( has_bump)
		use_steep = m_use_steep_plax && mat->use_steep_parallax();

	if ( !has_bump)
		is_hq = false;

	bool has_detail = m_use_detail && mat->has_detail();
	float4 const *	dt_scaler = mat->get_detail_setup();
	const char* detail_name = mat->get_detail_name().c_str();
	ref_material det_mat = material_manager::ref().get_material( detail_name);

	ASSERT( has_detail ? detail_name&&detail_name[0]&&mat->has_detail()&&det_mat : true);

	fixed_string<128> ext_det_bump_name;
	bool has_detail_bump = false;
	const char* det_bump_name = 0;
	if ( det_mat)
	{
		has_detail_bump = m_use_detail && det_mat->has_detail();
		det_bump_name = det_mat->get_bump_name().c_str();
		ext_det_bump_name = det_bump_name;
		ext_det_bump_name += "#";
	}

	fixed_string<128> vs, ps, params;

	//!!!!!!!!!!!!!!REFACTOR IMPLEMENT DEFINES FOR SHADER COMPILATION
	vs += "deffer_";
	vs += vs_base;
	vs += ( has_bump) ? "_bump" : "_flat";
	
	vs+="( ";
	
	if ( has_lmap) add_param( params, "lmh");
	
	if ( has_detail && has_detail_bump)
		add_param( params, "db");
	else if ( has_detail)
		add_param( params, "d");
	
	if ( is_hq)
		add_param( params, "hq");
	
	vs+=params;
	vs+=")";

	//vs += ( has_lmap) ? "_lmh" : "";
	//vs += ( has_detail) ? "_d" : "";
	//vs += ( has_detail_bump) ? "b" : "";
	//vs += ( is_hq) ? "-hq" : "";

	params.clear();

	ps += "deffer_";
	ps += ps_base;
	//ps += ( has_lmap) ? "_lmh" : "";
	ps += ( is_aref) ? "_aref" : "";
	ps += ( has_bump) ? "_bump" : "_flat";
	//ps += ( has_detail) ? "_d" : "";
	//ps += ( has_detail_bump) ? "b" : "";
	//ps += ( is_hq) ? "-hq" : "";
	ps+="( ";
	
	if ( has_lmap)
	{
		define_macro( defines, "USE_LM_HEMI");
		add_param( params, "lmh");
	}
	
	if ( has_detail && has_detail_bump)
	{
		define_macro( defines, "USE_TDETAIL");
		define_macro( defines, "USE_TDETAIL_BUMP");
		add_param( params, "db");
	}
	else if ( has_detail)
	{
		define_macro( defines, "USE_TDETAIL");
		add_param( params, "d");
	}
	
	if ( is_hq && use_steep)
	{
		define_macro( defines, "USE_STEEPPARALLAX");
		add_param( params, "hq");
	}
	else if ( is_hq)
	{
		define_macro( defines, "USE_PARALLAX");
		add_param( params, "hq");
	}
	
	ps+=params;
	ps+=")";

	make_defines( defines);//Do not move it adds terminator!!!!!!!!!!!!!!
	compiler.begin_pass( vs.c_str(), ps.c_str(), defines)
		//.set_depth( TRUE)
		//.set_alpha_blend( FALSE)
		.set_texture( "t_base", mat_name);
	
	if ( has_bump)
	{
		compiler
			.set_texture( "t_bump", bump_name)	// should be before base bump
			.set_texture( "t_bumpX", ext_bump_name.c_str());
	}

	if ( has_detail)
	{
		compiler
			.set_texture( "t_bumpD", detail_name)
			.set_texture( "t_detail", detail_name);
	}

	if ( has_detail_bump)
	{
		compiler
			.set_texture( "t_detailBump", det_bump_name)
			.set_texture( "t_detailBumpX", ext_det_bump_name.c_str());
	}

	if ( has_lmap)
		compiler.set_texture( "t_hemi", lmap_name);

	if ( has_detail && dt_scaler)
		compiler.bind_constant( "dt_params", dt_scaler);

	if( m_stencil_marking)
		compiler.set_stencil( true, m_stencil_marker, 0xff, 0xff, D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_REPLACE, D3D_STENCIL_OP_KEEP);

	compiler.end_pass();
}

} // namespace render
} // namespace xray

