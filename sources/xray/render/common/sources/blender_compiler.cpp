////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_compiler.h"
#include "manager_common_inline.h"
#include <xray/render/common/sources/shader_manager.h>

namespace xray {
namespace render {


//blender_compiler::blender_compiler()
//{
//
//}

blender_compiler::~blender_compiler()
{

}

blender_compiler& blender_compiler::begin_pass(LPCSTR vs, LPCSTR ps, shader_defines_list& defines)
{
	m_sim_states.clear();
	m_constants.clear();
	m_tex_lst.clear();
	m_bindings.clear();

	//shader_compilation_opts	opts; //TODO HACK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	m_vs = resource_manager::get_ref().create_vs(vs, defines);
	m_ps = resource_manager::get_ref().create_ps(ps, defines);

	m_vs->merge_ctable_in(m_constants);
	m_ps->merge_ctable_in(m_constants);
	
	set_z_test(TRUE);
	set_alpha_blend(FALSE, D3DBLEND_ONE, D3DBLEND_ZERO);
	set_alpha_test(FALSE, 0);

	return *this;
}

blender_compiler& blender_compiler::set_z_test(BOOL is_write_enabled, D3DCMPFUNC cmp_func)
{
	m_sim_states.set_rs(D3DRS_ZWRITEENABLE, is_write_enabled);
	m_sim_states.set_rs(D3DRS_ZFUNC, cmp_func);

	return *this;
}

blender_compiler& blender_compiler::set_stencil	(BOOL enable, u32 func, u32 ref, u32 mask, u32 writemask, u32 fail, u32 pass, u32 zfail)
{
	m_sim_states.set_rs(	D3DRS_STENCILENABLE,	enable );

	if (!enable) 
		return *this;

	m_sim_states.set_rs(	D3DRS_STENCILFUNC,		func);
	m_sim_states.set_rs(	D3DRS_STENCILMASK,		mask);
	m_sim_states.set_rs(	D3DRS_STENCILREF,		ref);
	m_sim_states.set_rs(	D3DRS_STENCILWRITEMASK,	writemask);
	m_sim_states.set_rs(	D3DRS_STENCILFAIL,		fail);
	m_sim_states.set_rs(	D3DRS_STENCILPASS,		pass);
	m_sim_states.set_rs(	D3DRS_STENCILZFAIL,		zfail);
	
	//	Since we never really support different options for
	//	CW/CCW stencil use it to mimic DX9 behaviour for 
	//	single-sided stencil
	m_sim_states.set_rs(	D3DRS_CCW_STENCILFUNC,		func);
	m_sim_states.set_rs(	D3DRS_CCW_STENCILFAIL,		fail);
	m_sim_states.set_rs(	D3DRS_CCW_STENCILPASS,		pass);
	m_sim_states.set_rs(	D3DRS_CCW_STENCILZFAIL,		zfail);

	return *this;
}

blender_compiler& blender_compiler::set_alpha_blend(BOOL is_blend_enabled, D3DBLEND src_blend, D3DBLEND dest_blend)
{
	if (is_blend_enabled && D3DBLEND_ONE == src_blend && D3DBLEND_ZERO == dest_blend)
		is_blend_enabled = FALSE;

	m_sim_states.set_rs(D3DRS_ALPHABLENDENABLE,	is_blend_enabled);
	m_sim_states.set_rs(D3DRS_SRCBLEND,	is_blend_enabled ? src_blend : D3DBLEND_ONE);
	m_sim_states.set_rs(D3DRS_DESTBLEND, is_blend_enabled ? dest_blend : D3DBLEND_ZERO);

	return *this;
}

blender_compiler& blender_compiler::set_alpha_test(BOOL is_atest_enabled, DWORD aref_value)
{
	math::clamp<DWORD>(aref_value, 0, 255);
	m_sim_states.set_rs(D3DRS_ALPHATESTENABLE, is_atest_enabled);
	if (is_atest_enabled)
		m_sim_states.set_rs(D3DRS_ALPHAREF,	aref_value);

	return *this;
}

blender_compiler& blender_compiler::def_sampler(LPCSTR name, shared_string texture, u32 address, u32 fmin, u32 fmip, u32 fmag)
{
	DWORD stage = get_sampler(name);
	if (u32(-1) != stage)
	{
		set_samp_texture(stage, texture.c_str());

		//FILTERING CHANGING IS HACK!!!!!!! Try to localize it someway or another
		//May be make it texture attr?
		
		// force ANISO-TF for "s_base"
		if ((0 == strcmp(name,"s_base")) && (fmin==D3DTEXF_LINEAR))
		{
			fmin = D3DTEXF_ANISOTROPIC;
			fmag = D3DTEXF_ANISOTROPIC;
		}

		if (0 == strcmp(name,"s_base_hud"))
		{
			fmin = D3DTEXF_GAUSSIANQUAD; //D3DTEXF_PYRAMIDALQUAD; //D3DTEXF_ANISOTROPIC; //D3DTEXF_LINEAR; //D3DTEXF_POINT; //D3DTEXF_NONE
			fmag = D3DTEXF_GAUSSIANQUAD; //D3DTEXF_PYRAMIDALQUAD; //D3DTEXF_ANISOTROPIC; //D3DTEXF_LINEAR; //D3DTEXF_POINT; //D3DTEXF_NONE; 
		}

		if ((0 == strcmp(name,"s_detail")) && (fmin==D3DTEXF_LINEAR))
		{
			fmin = D3DTEXF_ANISOTROPIC;
			fmag = D3DTEXF_ANISOTROPIC;
		}

		// Sampler states
		set_samp_address(stage, address);
		set_samp_filter(stage, fmin, fmip, fmag);
	}
	return *this;
}

blender_compiler& blender_compiler::def_sampler_clf(LPCSTR name, shared_string texture)
{
	return def_sampler(name, texture, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);
}

blender_compiler& blender_compiler::def_sampler_rtf(LPCSTR name, shared_string texture)
{
	return def_sampler(name,texture, D3DTADDRESS_CLAMP, D3DTEXF_POINT, D3DTEXF_NONE, D3DTEXF_POINT);
}

blender_compiler& blender_compiler::def_sampler_clw(LPCSTR name, shared_string texture)
{
	u32 s = get_sampler(name);
	
	if (u32(-1) != s)
	{
		def_sampler(name, texture, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);
		m_sim_states.set_samp(s,D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	}

	return *this;
}

blender_compiler& blender_compiler::def_constant(shared_string name, const_setup_cb setup)
{
	m_bindings.add(name, setup);
	return *this;
}

blender_compiler& blender_compiler::end_pass()
{
	resource_manager& res_mgr = resource_manager::get_ref();
	shader_manager& sh_mgr = shader_manager::get_ref();

	m_bindings.apply(&m_constants);

	ref_state state = res_mgr.create_state(m_sim_states);
	ref_const_table constants= res_mgr.create_const_table(m_constants);
	ref_texture_list tex_lst= sh_mgr.create_texture_list(m_tex_lst);
	
	ref_pass pass = sh_mgr.create_pass(res_pass(m_vs, m_ps, constants, state, tex_lst));

	m_sh_technique.m_passes.push_back(pass);

	m_sim_states.clear();
	m_constants.clear();
	m_tex_lst.clear();
	m_bindings.clear();

	m_vs = 0;
	m_ps = 0;

	++m_pass_idx;

	return *this;
}

blender_compiler& blender_compiler::begin_technique()
{
	m_sh_technique.m_passes.clear();
	m_pass_idx = 0;

	return *this;
}

void blender_compiler::end_technique()
{
	shader_manager& sh_mgr = shader_manager::get_ref();
	ref_shader_technique se = sh_mgr.create_shader_technique(m_sh_technique);

	m_compilation_target.m_sh_techniques.push_back(se);

	m_sh_technique.m_passes.clear();

	++m_technique_idx; m_pass_idx = 0;
}

u32	blender_compiler::get_sampler(LPCSTR name)
{
	//string256				name;
	//strings::copy					(name,_name);
	////. andy	if (strext(name)) *strext(name)=0;
	//fix_texture_name		(name);

	// Find index
	ref_constant c = m_constants.get(name);
	if (!c)
		return	u32(-1);

	R_ASSERT(c->get_type() == rc_sampler);

	// Create texture
	// while (stage>=passTextures.size())	passTextures.push_back		(NULL);
	return c->m_samp.m_index;
}

void blender_compiler::set_samp_texture(u32 samp, LPCSTR name)
{
	if (!name)
		return;

	m_tex_lst.push_back(mk_pair(samp, ref_texture(resource_manager::get_ref().create_texture(name))));
}

void blender_compiler::set_samp_address(u32 samp, u32	address)
{
	m_sim_states.set_samp(samp, D3DSAMP_ADDRESSU, address);
	m_sim_states.set_samp(samp, D3DSAMP_ADDRESSV, address);
	m_sim_states.set_samp(samp, D3DSAMP_ADDRESSW, address);
}

void blender_compiler::set_samp_filter_min(u32 samp, u32 filter)
{
	m_sim_states.set_samp(samp, D3DSAMP_MINFILTER, filter);
}

void blender_compiler::set_samp_filter_mip(u32 samp, u32 filter)
{
	m_sim_states.set_samp(samp, D3DSAMP_MIPFILTER, filter);
}

void blender_compiler::set_samp_filter_mag(u32 samp, u32 filter)
{
	m_sim_states.set_samp(samp, D3DSAMP_MAGFILTER, filter);
}

void blender_compiler::set_samp_filter(u32 samp, u32 min, u32 mip, u32 mag)
{
	set_samp_filter_min(samp, min);
	set_samp_filter_mip(samp, mip);
	set_samp_filter_mag(samp, mag);
}

} // namespace render 
} // namespace xray 


//void fix_texture_name(LPSTR fn);
//
//void	CBlender_Compile::r_Pass		(LPCSTR _vs, LPCSTR _ps, bool bFog, BOOL bZtest, BOOL bZwrite,	BOOL bABlend, D3DBLEND abSRC, D3DBLEND abDST, BOOL aTest, u32 aRef)
//{
//	RS.Invalidate			();
//	ctable.clear			();
//	passTextures.clear		();
//	passMatrices.clear		();
//	passConstants.clear		();
//	dwStage					= 0;
//
//	// Setup FF-units (Z-buffer, blender)
//	PassSET_ZB				(bZtest,bZwrite);
//	PassSET_Blend			(bABlend,abSRC,abDST,aTest,aRef);
//	PassSET_LightFog		(FALSE,bFog);
//
//	// Create shaders
//	SPS* ps					= DEV->_CreatePS			(_ps);
//	SVS* vs					= DEV->_CreateVS			(_vs);
//	dest.ps					= ps;
//	dest.vs					= vs;
//#ifdef	USE_DX10
//	SGS* gs					= DEV->_CreateGS			("null");
//	dest.gs					= gs;
//#endif	//	USE_DX10
//	ctable.merge			(&ps->constants);
//	ctable.merge			(&vs->constants);
//	SetMapping				();
//
//	// Last Stage - disable
//	if (0==stricmp(_ps,"null"))	{
//		RS.SetTSS				(0,D3DTSS_COLOROP,D3DTOP_DISABLE);
//		RS.SetTSS				(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
//	}
//}
//
//void	CBlender_Compile::r_Constant	(LPCSTR name, R_constant_setup* s)
//{
//	R_ASSERT				(s);
//	ref_constant C			= ctable.get(name);
//	if (C)					C->handler	= s;
//}
//
//void CBlender_Compile::r_ColorWriteEnable( bool cR, bool cG, bool cB, bool cA)
//{
//	BYTE	Mask = 0;
//	Mask |= cR ? D3DCOLORWRITEENABLE_RED : 0;
//	Mask |= cG ? D3DCOLORWRITEENABLE_GREEN : 0;
//	Mask |= cB ? D3DCOLORWRITEENABLE_BLUE : 0;
//	Mask |= cA ? D3DCOLORWRITEENABLE_ALPHA : 0;
//
//	m_sim_states.set_rs( D3DRS_COLORWRITEENABLE, Mask);
//	m_sim_states.set_rs( D3DRS_COLORWRITEENABLE1, Mask);
//	m_sim_states.set_rs( D3DRS_COLORWRITEENABLE2, Mask);
//	m_sim_states.set_rs( D3DRS_COLORWRITEENABLE3, Mask);
//}
//
//#ifndef	USE_DX10
//u32		CBlender_Compile::i_Sampler		(LPCSTR _name)
//{
//	//
//	string256				name;
//	strings::copy					(name,_name);
//	//. andy	if (strext(name)) *strext(name)=0;
//	fix_texture_name		(name);
//
//	// Find index
//	ref_constant C			= ctable.get(name);
//	if (!C)					return	u32(-1);
//
//	R_ASSERT				(C->type == RC_sampler);
//	u32 stage				= C->samp.index;
//
//	// Create texture
//	// while (stage>=passTextures.size())	passTextures.push_back		(NULL);
//	return					stage;
//}
//void	CBlender_Compile::i_Texture		(u32 s, LPCSTR name)
//{
//	if (name)	passTextures.push_back	(mk_pair(s, ref_texture(DEV->_CreateTexture(name))));
//}
//void	CBlender_Compile::i_Projective	(u32 s, bool b)
//{
//	if	(b)	RS.SetTSS	(s,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE | D3DTTFF_PROJECTED);
//	else	RS.SetTSS	(s,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
//}
//void	CBlender_Compile::i_Address		(u32 s, u32	address)
//{
//	RS.SetSAMP			(s,D3DSAMP_ADDRESSU,	address);
//	RS.SetSAMP			(s,D3DSAMP_ADDRESSV,	address);
//	RS.SetSAMP			(s,D3DSAMP_ADDRESSW,	address);
//}
//void	CBlender_Compile::i_Filter_Min		(u32 s, u32	f)
//{
//	RS.SetSAMP			(s,D3DSAMP_MINFILTER,	f);
//}
//void	CBlender_Compile::i_Filter_Mip		(u32 s, u32	f)
//{
//	RS.SetSAMP			(s,D3DSAMP_MIPFILTER,	f);
//}
//void	CBlender_Compile::i_Filter_Mag		(u32 s, u32	f)
//{
//	RS.SetSAMP			(s,D3DSAMP_MAGFILTER,	f);
//}
//void	CBlender_Compile::i_Filter			(u32 s, u32 _min, u32 _mip, u32 _mag)
//{
//	i_Filter_Min	(s,_min);
//	i_Filter_Mip	(s,_mip);
//	i_Filter_Mag	(s,_mag);
//}
//u32		CBlender_Compile::r_Sampler		(LPCSTR _name, LPCSTR texture, bool b_ps1x_ProjectiveDivide, u32 address, u32 fmin, u32 fmip, u32 fmag)
//{
//	dwStage					= i_Sampler	(_name);
//	if (u32(-1)!=dwStage)
//	{
//		i_Texture				(dwStage,texture);
//
//		// force ANISO-TF for "s_base"
//		if ((0==xr_strcmp(_name,"s_base")) && (fmin==D3DTEXF_LINEAR))	{ fmin = D3DTEXF_ANISOTROPIC; fmag=D3DTEXF_ANISOTROPIC; }
//
//		if ( 0==xr_strcmp(_name,"s_base_hud") )
//		{
//			fmin	= D3DTEXF_GAUSSIANQUAD; //D3DTEXF_PYRAMIDALQUAD; //D3DTEXF_ANISOTROPIC; //D3DTEXF_LINEAR; //D3DTEXF_POINT; //D3DTEXF_NONE
//			fmag	= D3DTEXF_GAUSSIANQUAD; //D3DTEXF_PYRAMIDALQUAD; //D3DTEXF_ANISOTROPIC; //D3DTEXF_LINEAR; //D3DTEXF_POINT; //D3DTEXF_NONE; 
//		}
//
//		if ((0==xr_strcmp(_name,"s_detail")) && (fmin==D3DTEXF_LINEAR))	{ fmin = D3DTEXF_ANISOTROPIC; fmag=D3DTEXF_ANISOTROPIC; }
//
//		// Sampler states
//		i_Address				(dwStage,address);
//		i_Filter				(dwStage,fmin,fmip,fmag);
//		//.i_Filter				(dwStage,D3DTEXF_POINT,D3DTEXF_POINT,D3DTEXF_POINT); // show pixels
//		if (dwStage<4)			i_Projective		(dwStage,b_ps1x_ProjectiveDivide);
//	}
//	return	dwStage;
//}
//
//void	CBlender_Compile::r_Sampler_rtf	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide)
//{
//	r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
//}
//void	CBlender_Compile::r_Sampler_clf	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide)
//{
//	r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
//}
//void	CBlender_Compile::r_Sampler_clw	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide)
//{
//	u32 s			= r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
//	if (u32(-1)!=s)	RS.SetSAMP	(s,D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
//}
//
//void	CBlender_Compile::r_End			()
//{
//	dest.constants			= DEV->_CreateConstantTable(ctable);
//	dest.state				= DEV->_CreateState		(RS.GetContainer());
//	dest.T					= DEV->_CreateTextureList	(passTextures);
//	dest.C					= 0;
//#ifdef _EDITOR
//	dest.M					= 0;
//	SH->passes.push_back	(DEV->_CreatePass(dest.state,dest.ps,dest.vs,dest.constants,dest.T,dest.M,dest.C));
//#else
//	ref_matrix_list			temp(0);
//	SH->passes.push_back	(DEV->_CreatePass(dest.state,dest.ps,dest.vs,dest.constants,dest.T,temp,dest.C));
//#endif
//}
//#endif	//	USE_DX10