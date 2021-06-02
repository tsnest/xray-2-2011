////////////////////////////////////////////////////////////////////////////
//	Created		: 26.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_deffer_base.h"

namespace xray {
namespace render{

template<typename T1, typename T2>
void add_param(T1& str, const T2& param)
{
	if (!str.empty())
		str += ",";

	str += param;
}

void define_macro(shader_defines_list& defines, const char* param)
{
	define_macro(defines, param, "1");
}

void define_macro(shader_defines_list& defines, const char* param, const char* value)
{
	D3DXMACRO	macro;
	macro.Name = param;
	macro.Definition = value;
	defines.push_back(macro);
}

blender_deffer_base::blender_deffer_base(bool use_detail, bool use_bump, bool use_steep_plax):
  m_use_bump(true/*use_bump*/),
  m_use_steep_plax(true/*use_steep_plax*/),
  m_use_detail(false/*use_detail*/)
{
	XRAY_UNREFERENCED_PARAMETERS	( use_detail, use_bump, use_steep_plax );

	//add_property(pid_material_name,pt_string);
	//add_property(pid_lmap_name, pt_string);
	//add_property(pid_use_bump, pt_bool);
	//add_property(pid_use_steep_paralax, pt_bool);
	//add_property(pid_use_detail, pt_bool);
}

//REQUIRE FURTHER REFACTOR!!!!!!!!!!!!!
void blender_deffer_base::uber_deffer(blender_compiler& compiler,LPCSTR vs_base, LPCSTR ps_base, bool is_hq, bool is_aref, const blender_compilation_options& options)
{
	shader_defines_list	defines;

	bool has_lmap = false;

	//lmap does not work
	pcstr lmap_name = get<pcstr>(pid_lmap_name);
	if (lmap_name)
	{
		has_lmap = true;
	}

	//pcstr mat_name = get<pcstr>(pid_material_name);
	//ASSERT(mat_name);
	ASSERT(options.tex_list.size());
	shared_string mat_name = options.tex_list[0];
	ref_material mat = material_manager::get_ref().get_material(mat_name);

	bool has_bump = m_use_bump && mat->has_bump();
	const char* bump_name = mat->get_bump_name().c_str();
	fixed_string<128> ext_bump_name(bump_name);
	ext_bump_name += "#";
	
	bool use_steep = false;
	if (has_bump)
		use_steep = m_use_steep_plax && mat->use_steep_parallax();

	if (!has_bump)
		is_hq = false;

	bool has_detail = m_use_detail && mat->has_detail();
	const_setup_cb	dt_scaler = mat->get_detail_setup();
	ref_material	det_mat;
	const char* detail_name = NULL;
	if(has_detail)
	{
		detail_name		= mat->get_detail_name().c_str();
		det_mat			= material_manager::get_ref().get_material(detail_name);
	}
	ASSERT(has_detail ? detail_name&&detail_name[0]&&mat->has_detail()&&det_mat : true);

	fixed_string<128> ext_det_bump_name;
	bool has_detail_bump = false;
	const char* det_bump_name = 0;
	if (det_mat)
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
	vs += (has_bump) ? "_bump" : "_flat";
	
	vs+="(";
	
	if (has_lmap) add_param(params, "lmh");
	
	if (has_detail && has_detail_bump)
		add_param(params, "db");
	else if (has_detail)
		add_param(params, "d");
	
	if (is_hq)
		add_param(params, "hq");
	
	vs+=params;
	vs+=")";

	//vs += (has_lmap) ? "_lmh" : "";
	//vs += (has_detail) ? "_d" : "";
	//vs += (has_detail_bump) ? "b" : "";
	//vs += (is_hq) ? "-hq" : "";

	params.clear();

	ps += "deffer_";
	ps += ps_base;
	//ps += (has_lmap) ? "_lmh" : "";
	ps += (is_aref) ? "_aref" : "";
	ps += (has_bump) ? "_bump" : "_flat";
	//ps += (has_detail) ? "_d" : "";
	//ps += (has_detail_bump) ? "b" : "";
	//ps += (is_hq) ? "-hq" : "";
	ps+="(";
	
	if (has_lmap)
	{
		define_macro(defines, "USE_LM_HEMI");
		add_param(params, "lmh");
	}
	
	if (has_detail && has_detail_bump)
	{
		define_macro(defines, "USE_TDETAIL");
		define_macro(defines, "USE_TDETAIL_BUMP");
		add_param(params, "db");
	}
	else if (has_detail)
	{
		define_macro(defines, "USE_TDETAIL");
		add_param(params, "d");
	}
	
	if (is_hq && use_steep)
	{
		define_macro(defines, "USE_STEEPPARALLAX");
		add_param(params, "hq");
	}
	else if (is_hq)
	{
		define_macro(defines, "USE_PARALLAX");
		add_param(params, "hq");
	}
	
	ps+=params;
	ps+=")";

	make_defines(defines);//Do not move it adds terminator!!!!!!!!!!!!!!
	compiler.begin_pass(vs.c_str(), ps.c_str(), defines)
		//.set_z_test(TRUE)
		//.set_alpha_blend(FALSE)
		.def_sampler("s_base", mat_name, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	
	if (has_bump)
	{
		compiler
			.def_sampler("s_bump", bump_name, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC)	// should be before base bump
			.def_sampler("s_bumpX", ext_bump_name.c_str(), D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	}

	if (has_detail)
	{
		compiler
			.def_sampler("s_bumpD", detail_name, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC)
			.def_sampler("s_detail", detail_name, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	}

	if (has_detail_bump)
	{
		compiler
			.def_sampler("s_detailBump", det_bump_name, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC)
			.def_sampler("s_detailBumpX", ext_det_bump_name.c_str(), D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	}

	if (has_lmap)
		compiler.def_sampler("s_hemi", lmap_name, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);

	if (has_detail && dt_scaler)
		compiler.def_constant("dt_params", dt_scaler);

	if( m_stencil_marking )
		compiler.set_stencil( TRUE, D3DCMP_ALWAYS, m_stencil_marker, 0xff, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);

	compiler.end_pass();
}

} // namespace render
} // namespace xray

