////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/stage_combine.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/engine/effect_combine.h>
#include <xray/render/engine/environment.h>
#include <xray/render/engine/vertex_formats.h>
#include <xray/render/core/backend.h>
#include <xray/render/engine/lights_db.h>

namespace xray {
namespace render_dx10 {

stage_combine::stage_combine( scene_context* context): stage( context)
{
// 	static D3DVERTEXELEMENT9 combine_decl[] =
// 	{
// 		{ 0, 0,  D3DDECLTYPE_FLOAT4,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },	// pos+uv
// 		{ 0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_COLOR,		0 },
// 		{ 0, 20, D3DDECLTYPE_FLOAT2,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
// 		D3DDECL_END()
// 	};

	effect_combine	b_combine;

	m_sh_combine =	effect_manager::ref().create_effect		( &b_combine, "r2\\combine", "");
	m_g_combine =	resource_manager::ref().create_geometry	( vertex_formats::F_TL, sizeof( vertex_formats::TL), backend::ref().vertex.buffer(), &*m_context->m_quad_ib);

	m_L_ambient		= backend::ref().register_constant_host( "L_ambient");
	m_L_sun_color	= backend::ref().register_constant_host( "L_sun_color");
	m_Ldynamic_dir	= backend::ref().register_constant_host( "Ldynamic_dir");
	m_env_color		= backend::ref().register_constant_host( "env_color");
	m_fog_color		= backend::ref().register_constant_host( "fog_color");
	m_m_v2w			= backend::ref().register_constant_host( "m_v2w");

}

stage_combine::~stage_combine()
{
}

namespace
{
	float hclip( float v, float dim) {return 2.0f*v/dim - 1.0f;}

}

void stage_combine::execute()
{
	PIX_EVENT( stage_combine);

	backend::ref().set_render_targets( &*m_context->m_rt_generic_0, &*m_context->m_rt_generic_1, 0);
	backend::ref().reset_depth_stencil_target();
	// --Porting to DX10_
	//set_render_target( m_context->m_rt_generic_0,  m_context->m_rt_generic_1, 0, m_context->m_base_zb);

	//light* sun = lights_db::ref().get_sun().get();//( light*)RImplementation.Lights.sun_adapted._get()	;

	//if ( !sun)
	//{
	//	return;
	//}

	backend& be = backend::ref();

	// --Porting to DX10_
	//be.set_cull_mode( D3DCULL_NONE);

	// low/hi RTs
	//u_setrt( rt_Generic_0,rt_Generic_1,0,HW.pBaseZB);

	// --Porting to DX10_
	//be.set_stencil( FALSE);

	//BOOL	split_the_scene_to_minimize_wait			= FALSE;
	//if ( ps_r2_ls_flags.test( R2FLAG_EXP_SPLIT_SCENE))	split_the_scene_to_minimize_wait=TRUE;

	// 
	//if ( RImplementation.o.bug)	{
		//be.set_Stencil					( TRUE,D3D_COMPARISON_LESS_EQUAL,0x01,0xff,0x00);	// stencil should be >= 1
		//if ( RImplementation.o.nvstencil)	{
		//	u_stencil_optimize				( FALSE);
		//	be.set_ColorWriteEnable		();
		//}
	//}

	// calc m-blur matrices
	//float4x4		m_previous, m_current;
	//Fvector2	m_blur_scale;
	//{
	//	static float4x4		m_saved_viewproj;
	//	
	//	// ( new-camera) -> ( world) -> ( old_viewproj)
	//	float4x4	m_invview;	m_invview.invert	( Device.mView);
	//	m_previous.mul		( m_saved_viewproj,m_invview);
	//	m_current.set		( Device.mProject)		;
	//	m_saved_viewproj.set( Device.mFullTransform)	;
	//	float	scale		= ps_r2_mblur/2.f;
	//	m_blur_scale.set	( scale,-scale).div( 12.f);
	//}

	// Draw full-screen quad textured with our scene image
	//if ( !_menu_pp)
	//{
		// Compute params
		float4x4		m_v2w( m_context->get_inv_v());

		//CEnvDescriptorMixer& envdesc= *g_pGamePersistent->Environment().CurrentEnv		;

		//const float minamb = 0.001f;

		//float4	ambclr = {std::max( envdesc.ambient.x*2,minamb), std::max( envdesc.ambient.y*2,minamb), std::max( envdesc.ambient.z*2,minamb),	0};
		//ambclr.mul( ps_r2_sun_lumscale_amb);
		//float4	ambclr( 0.04f, 0.04f, 0.04f, 0);
		////float4	envclr = {envdesc.hemi_color.x*2+math::epsilon_5, envdesc.hemi_color.y*2+math::epsilon_5, envdesc.hemi_color.z*2+math::epsilon_5, envdesc.weight};
		//float4	envclr( 0.46055603f, 0.38911888f, 0.35198119f, 1);

		float4	ambclr( environment::ref().get_amb_color());
		float4	envclr( 2*environment::ref().get_env_color());
		float4	fogclr( environment::ref().get_fog_color());
		float4	sunclr( color(lights_db::ref().get_sun()->color));
		float4	sundir( lights_db::ref().get_sun()->direction, 0);

		//envclr.x *= 2*ps_r2_sun_lumscale_hemi; 
		//envclr.y *= 2*ps_r2_sun_lumscale_hemi; 
		//envclr.z *= 2*ps_r2_sun_lumscale_hemi;

		//float4	fogclr = {envdesc.fog_color.x, envdesc.fog_color.y, envdesc.fog_color.z, 0};
		//float4 fogclr( 0.26033184f, 0.27797237f, 0.38404618f, 0);

		//float4	sunclr,sundir;

		//float		fSSAONoise = 2.0f;
		//fSSAONoise *= tan( deg2rad( 67.5f));
		//fSSAONoise /= tan( deg2rad( Device.fFOV));

		//float		fSSAOKernelSize = 150.0f;
		//fSSAOKernelSize *= tan( deg2rad( 67.5f));
		//fSSAOKernelSize /= tan( deg2rad( Device.fFOV));

		// sun-params
		//{
		//	light*		fuckingsun		= ( light*)RImplementation.Lights.sun_adapted._get()	;
		//	Fvector		L_dir,L_clr;	float L_spec;
		//	L_clr.set					( fuckingsun->color.r,fuckingsun->color.g,fuckingsun->color.b);
		//	L_spec						= u_diffuse2s	( L_clr);
		//	Device.mView.transform_dir	( L_dir,fuckingsun->direction);
		//	L_dir.normalize				();

		//	sunclr.set				( L_clr.x,L_clr.y,L_clr.z,L_spec);
		//	sundir.set				( L_dir.x,L_dir.y,L_dir.z,0);
		//}

		//float3	sun_dir( m_mat_view.transform_direction( ( float3( 0.47652841f, -0.80543172f, -0.35242066f))/*sun->direction*/));
		//float3	sun_clr( 0.90578169f, 0.83912235f, 0.69403785f);//( sun->color.r, sun->color.g, sun->color.b);
		//float3	sun_dir( m_mat_view.transform_direction( /*( float3( 0.47652841f, -0.80543172f, -0.35242066f))*/sun->direction));
		//float3	sun_clr/*( 0.90578169f, 0.83912235f, 0.69403785f);*/( sun->color.r, sun->color.g, sun->color.b);
		//float	sun_spec = u_diffuse2s( sun_clr);
		//sundir.set( sun_dir.x, sun_dir.y, sun_dir.z, 0);
		//sunclr.set( 0.90578169f, 0.83912235f, 0.69403785f, sun_spec);

		// Fill VB
		float w = float( device::ref().get_width());
		float h = float( device::ref().get_height());
		
		float2	p0,p1;
		u32		Offset = 0;

		p0.set( 0.f/w, 0.f/h);
		p1.set( ( w+0.f)/w, ( h+0.f)/h);

		float	scale_X	= w / float( tex_jitter);
		float	scale_Y	= h / float( tex_jitter);

		vertex_formats::TL* pv	= ( vertex_formats::TL*)be.vertex.lock( 4, sizeof( vertex_formats::TL), Offset);
		pv->set( hclip( math::epsilon_5, w), hclip( h+math::epsilon_5, h), p0.x, p1.y, 0, 0, scale_Y); 
		pv++;
		pv->set( hclip( math::epsilon_5, w), hclip( math::epsilon_5, h), p0.x, p0.y, 0, 0, 0); 
		pv++;
		pv->set( hclip( w+math::epsilon_5, w), hclip( h+math::epsilon_5, h), p1.x, p1.y, 0, scale_X, scale_Y); 
		pv++;
		pv->set( hclip( w+math::epsilon_5, w), hclip( math::epsilon_5, h), p1.x, p0.y, 0, scale_X, 0); 
		pv++;
		be.vertex.unlock();

		//dxEnvDescriptorMixerRender &envdescren = *( dxEnvDescriptorMixerRender*)( &*envdesc.m_pDescriptorMixer);

		// Setup textures
		//IDirect3DBaseTexture9*	e0	= _menu_pp?0:envdescren.sky_r_textures_env[0].second->surface_get();
		//IDirect3DBaseTexture9*	e1	= _menu_pp?0:envdescren.sky_r_textures_env[1].second->surface_get();
		//t_envmap_0->surface_set		( e0);	_RELEASE( e0);
		//t_envmap_1->surface_set		( e1);	_RELEASE( e1);
	
		// Draw
		m_sh_combine->apply();
		m_g_combine->apply();

		be.set_constant( m_L_ambient,	ambclr);
		be.set_constant( m_L_sun_color, sunclr);
//		be.set_constant( m_Ldynamic_dir, sundir);
		be.set_constant( m_env_color,	envclr);
		be.set_constant( m_fog_color,	fogclr);
		be.set_constant( m_m_v2w,	fogclr);
		//be.set_constant( "ssao_params", fSSAONoise, fSSAOKernelSize, 0.0f, 0.0f);

		// --Porting to DX10_
		//be.set_stencil( TRUE, D3D_COMPARISON_LESS_EQUAL, 0x01, 0x01, 0x00);
		be.render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 2*3, 0, Offset);
	//}
}

} // namespace render
} // namespace xray

