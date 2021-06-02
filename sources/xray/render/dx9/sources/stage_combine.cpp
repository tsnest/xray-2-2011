////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_combine.h"
#include <xray/render/common/sources/blender_combine.h>
#include <xray/render/common/sources/environment.h>
#include <xray/render/common/sources/shader_manager.h>

namespace xray {
namespace render{

stage_combine::stage_combine(scene_context* context): stage(context)
{
	static D3DVERTEXELEMENT9 combine_decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },	// pos+uv
		{ 0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_COLOR,		0 },
		{ 0, 20, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
		D3DDECL_END()
	};

	backend& be = backend::get_ref();
	blender_combine	b_combine;

	m_sh_combine = shader_manager::get_ref().create_shader(&b_combine, "r2\\combine", "");
	m_g_combine = shader_manager::get_ref().create_geometry(combine_decl, be.vertex.get_buffer(), m_context->m_quad_ib);
}

stage_combine::~stage_combine()
{
}

namespace
{
	float hclip(float v, float dim) {return 2.0f*v/dim - 1.0f;}

}

void stage_combine::execute( )
{
	PIX_EVENT(stage_combine);

	set_rt( m_context->m_rt_generic_0,  m_context->m_rt_generic_1, 0, m_context->m_base_zb);
	//light* sun = lights_db::get_ref().get_sun().get();//(light*)RImplementation.Lights.sun_adapted._get()	;

	//if (!sun)
	//{
	//	return;
	//}

	backend& be = backend::get_ref();

	be.set_cull_mode(D3DCULL_NONE);

	// low/hi RTs
	//u_setrt(rt_Generic_0,rt_Generic_1,0,HW.pBaseZB);
	be.set_stencil(FALSE);

	//BOOL	split_the_scene_to_minimize_wait			= FALSE;
	//if (ps_r2_ls_flags.test(R2FLAG_EXP_SPLIT_SCENE))	split_the_scene_to_minimize_wait=TRUE;

	// 
	//if (RImplementation.o.bug)	{
		//be.set_Stencil					(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);	// stencil should be >= 1
		//if (RImplementation.o.nvstencil)	{
		//	u_stencil_optimize				(FALSE);
		//	be.set_ColorWriteEnable		();
		//}
	//}

	// calc m-blur matrices
	//float4x4		m_previous, m_current;
	//Fvector2	m_blur_scale;
	//{
	//	static float4x4		m_saved_viewproj;
	//	
	//	// (new-camera) -> (world) -> (old_viewproj)
	//	float4x4	m_invview;	m_invview.invert	(Device.mView);
	//	m_previous.mul		(m_saved_viewproj,m_invview);
	//	m_current.set		(Device.mProject)		;
	//	m_saved_viewproj.set(Device.mFullTransform)	;
	//	float	scale		= ps_r2_mblur/2.f;
	//	m_blur_scale.set	(scale,-scale).div(12.f);
	//}

	// Draw full-screen quad textured with our scene image
	//if (!_menu_pp)
	//{
		// Compute params
		float4x4		m_v2w(m_context->get_inv_v());

		//CEnvDescriptorMixer& envdesc= *g_pGamePersistent->Environment().CurrentEnv		;

		//const float minamb = 0.001f;

		//float4	ambclr = {std::max(envdesc.ambient.x*2,minamb), std::max(envdesc.ambient.y*2,minamb), std::max(envdesc.ambient.z*2,minamb),	0};
		//ambclr.mul(ps_r2_sun_lumscale_amb);
		//float4	ambclr(0.04f, 0.04f, 0.04f, 0);
		////float4	envclr = {envdesc.hemi_color.x*2+math::epsilon_5, envdesc.hemi_color.y*2+math::epsilon_5, envdesc.hemi_color.z*2+math::epsilon_5, envdesc.weight};
		//float4	envclr(0.46055603f, 0.38911888f, 0.35198119f, 1);

		float4	ambclr(environment::get_ref().get_amb_color());
		float4	envclr(2*environment::get_ref().get_env_color());
		float4	fogclr(environment::get_ref().get_fog_color());

		//envclr.x *= 2*ps_r2_sun_lumscale_hemi; 
		//envclr.y *= 2*ps_r2_sun_lumscale_hemi; 
		//envclr.z *= 2*ps_r2_sun_lumscale_hemi;

		//float4	fogclr = {envdesc.fog_color.x, envdesc.fog_color.y, envdesc.fog_color.z, 0};
		//float4 fogclr(0.26033184f, 0.27797237f, 0.38404618f, 0);

		//float4	sunclr,sundir;

		//float		fSSAONoise = 2.0f;
		//fSSAONoise *= tan(deg2rad(67.5f));
		//fSSAONoise /= tan(deg2rad(Device.fFOV));

		//float		fSSAOKernelSize = 150.0f;
		//fSSAOKernelSize *= tan(deg2rad(67.5f));
		//fSSAOKernelSize /= tan(deg2rad(Device.fFOV));

		// sun-params
		//{
		//	light*		fuckingsun		= (light*)RImplementation.Lights.sun_adapted._get()	;
		//	Fvector		L_dir,L_clr;	float L_spec;
		//	L_clr.set					(fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
		//	L_spec						= u_diffuse2s	(L_clr);
		//	Device.mView.transform_dir	(L_dir,fuckingsun->direction);
		//	L_dir.normalize				();

		//	sunclr.set				(L_clr.x,L_clr.y,L_clr.z,L_spec);
		//	sundir.set				(L_dir.x,L_dir.y,L_dir.z,0);
		//}

		//float3	sun_dir(m_mat_view.transform_direction((float3(0.47652841f, -0.80543172f, -0.35242066f))/*sun->direction*/));
		//float3	sun_clr(0.90578169f, 0.83912235f, 0.69403785f);//(sun->color.r, sun->color.g, sun->color.b);
		//float3	sun_dir(m_mat_view.transform_direction(/*(float3(0.47652841f, -0.80543172f, -0.35242066f))*/sun->direction));
		//float3	sun_clr/*(0.90578169f, 0.83912235f, 0.69403785f);*/(sun->color.r, sun->color.g, sun->color.b);
		//float	sun_spec = u_diffuse2s(sun_clr);
		//sundir.set(sun_dir.x, sun_dir.y, sun_dir.z, 0);
		//sunclr.set(0.90578169f, 0.83912235f, 0.69403785f, sun_spec);

		// Fill VB
		float w = float(hw_wrapper::get_ref().get_width());
		float h = float(hw_wrapper::get_ref().get_height());
		
		float2	p0,p1;
		u32		Offset = 0;

		p0.set(0.5f/w, 0.5f/h);
		p1.set((w+0.5f)/w, (h+0.5f)/h );

		float	scale_X	= w / float(tex_jitter);
		float	scale_Y	= h / float(tex_jitter);

		FVF::TL* pv	= (FVF::TL*)be.vertex.lock(4, m_g_combine->get_stride(), Offset);
		pv->set(hclip(math::epsilon_5, w), hclip(h+math::epsilon_5, h), p0.x, p1.y, 0, 0, scale_Y); pv++;
		pv->set(hclip(math::epsilon_5, w), hclip(math::epsilon_5, h), p0.x, p0.y, 0, 0, 0); pv++;
		pv->set(hclip(w+math::epsilon_5, w), hclip(h+math::epsilon_5, h), p1.x, p1.y, 0, scale_X, scale_Y); pv++;
		pv->set(hclip(w+math::epsilon_5, w), hclip(math::epsilon_5, h), p1.x, p0.y, 0, scale_X, 0); pv++;
		be.vertex.unlock(4, m_g_combine->get_stride());

		//dxEnvDescriptorMixerRender &envdescren = *(dxEnvDescriptorMixerRender*)(&*envdesc.m_pDescriptorMixer);

		// Setup textures
		//IDirect3DBaseTexture9*	e0	= _menu_pp?0:envdescren.sky_r_textures_env[0].second->surface_get();
		//IDirect3DBaseTexture9*	e1	= _menu_pp?0:envdescren.sky_r_textures_env[1].second->surface_get();
		//t_envmap_0->surface_set		(e0);	_RELEASE(e0);
		//t_envmap_1->surface_set		(e1);	_RELEASE(e1);
	
		// Draw
		m_sh_combine->apply();
		m_g_combine->apply();

		be.set_c("m_v2w", m_v2w);
		be.set_c("L_ambient", ambclr);
		//be.set_c("Ldynamic_color", sunclr);
		//be.set_c("Ldynamic_dir", sundir);
		be.set_c("env_color", envclr);
		be.set_c("fog_color", fogclr);
		//be.set_c("ssao_params", fSSAONoise, fSSAOKernelSize, 0.0f, 0.0f);

		be.set_stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0x01, 0x00);
		be.render(D3DPT_TRIANGLELIST, Offset, 0, 4, 0, 2);
	//}
}

} // namespace render
} // namespace xray

