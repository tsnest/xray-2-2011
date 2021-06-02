////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_skybox.h"
#include <xray/render/core/effect_descriptor.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/backend.h>
#include <xray/render/core/pix_event_wrapper.h>
#include "renderer_context.h"
#include "renderer_context_targets.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/options.h>
#include "statistics.h"
#include "scene.h"
#include "lights_db.h"
#include "light.h"

namespace xray {
namespace render {

class effect_skybox : public effect_descriptor
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config)
	{
		XRAY_UNREFERENCED_PARAMETER	( config);

		compiler.begin_technique( /*sky box*/)
			.begin_pass( "sky2", NULL, "sky2", shader_configuration(), NULL)
				.set_depth( false, false)
				.set_stencil( true, 0x00, 0xff, 0x00, D3D_COMPARISON_EQUAL)
				.set_texture ( "t_skybox", "test/sky_19_cube")
			.end_pass()
		.end_technique();
	}
};

#pragma pack( push,1)
struct v_skybox				{
	float3	p;
	u32			color;
	float3	uv	[2];

	void		set			( const float3& _p, u32 _c, const float3& _tc)
	{
		p					= _p;
		color				= _c;
		uv[0]				= _tc;
		uv[1]				= _tc;
	}
};

//const	u32 v_skybox_fvf	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3( 0) | D3DFVF_TEXCOORDSIZE3( 1);
const D3D_INPUT_ELEMENT_DESC v_skybox_fvf[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR",		0, DXGI_FORMAT_R8G8B8A8_UNORM,		0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 16, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32B32_FLOAT,		0, 28, 	D3D_INPUT_PER_VERTEX_DATA, 0}
};

struct v_clouds				{
	float3	p;
	u32			color;
	u32			intensity;
	void		set			( const float3& _p, u32 _c, u32 _i)
	{
		p					= _p;
		color				= _c;
		intensity			= _i;
	}
};
//const	u32 v_clouds_fvf	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
#pragma pack( pop)

stage_skybox::stage_skybox( renderer_context* context ): stage( context)
{
	effect_manager::ref().create_effect<effect_skybox>(&m_sh_sky);
	m_g_skybox				= resource_manager::ref().create_geometry( v_skybox_fvf, sizeof(v_skybox), backend::ref().vertex.buffer(), backend::ref().index.buffer());
	//m_c_sun_direction		= backend::ref().register_constant_host( "sun_direction", rc_float );
	m_enabled				= options::ref().m_enabled_sky_box_stage;
}

bool stage_skybox::is_effects_ready() const
{
	return m_sh_sky.c_ptr() != NULL;
}

stage_skybox::~stage_skybox()
{
}

//////////////////////////////////////////////////////////////////////////
// half box def
static	float	hbox_verts[24][3]	=
{
	{-1.f,	-1.f,	-1.f}, {-1.f,	-1.f,	-1.f},	// down
	{ 1.f,	-1.f,	-1.f}, { 1.f,	-1.f,	-1.f},	// down
	{-1.f,	-1.f,	 1.f}, {-1.f,	-1.f,	 1.f},	// down
	{ 1.f,	-1.f,	 1.f}, { 1.f,	-1.f,	 1.f},	// down
	{-1.f,	 1.f,	-1.f}, {-1.f,	 1.f,	-1.f},
	{ 1.f,	 1.f,	-1.f}, { 1.f,	 1.f,	-1.f},
	{-1.f,	 1.f,	 1.f}, {-1.f,	 1.f,	 1.f},
	{ 1.f,	 1.f,	 1.f}, { 1.f,	 1.f,	 1.f},
	{-1.f,	 0.f,	-1.f}, {-1.f,	-1.f,	-1.f},	// half
	{ 1.f,	 0.f,	-1.f}, { 1.f,	-1.f,	-1.f},	// half
	{ 1.f,	 0.f,	 1.f}, { 1.f,	-1.f,	 1.f},	// half
	{-1.f,	 0.f,	 1.f}, {-1.f,	-1.f,	 1.f}	// half
};
static	u16			hbox_faces[20*3]	=
{
	0,	 2,	 3,
	3,	 1,	 0,
	4,	 5,	 7,
	7,	 6,	 4,
	0,	 1,	 9,
	9,	 8,	 0,
	8,	 9,	 5,
	5,	 4,	 8,
	1,	 3,	10,
	10,	 9,	 1,
	9,	10,	 7,
	7,	 5,	 9,
	3,	 2,	11,
	11,	10,	 3,
	10,	11,	 6,
	6,	 7,	10,
	2,	 0,	 8,
	8,	11,	 2,
	11,	 8,	 4,
	4,	 6,	11
};

bool stage_skybox::is_enabled() const
{
	if (stage::is_enabled())
	{
		return true;
	}
	else
	{
		math::color clear_color = math::color(0.2f,0.2f,0.2f,1.0f);
		backend::ref().set_render_targets( &*m_context->m_targets->m_rt_generic_0, &*m_context->m_targets->m_rt_generic_1, 0, 0);
		backend::ref().clear_render_targets(clear_color,clear_color,clear_color,clear_color);
		return false;
	}
}

void stage_skybox::execute()
{
	PIX_EVENT( stage_skybox);
	
	if (!is_effects_ready())
		return;
	
	BEGIN_CPUGPU_TIMER(statistics::ref().sky_stat_group.execute_time);
	
	if (!is_enabled())
	{
		execute_disabled();
		return;
	}
	
	backend::ref().set_render_targets( &*m_context->m_targets->m_rt_generic_0, &*m_context->m_targets->m_rt_generic_1, 0, 0);
	backend::ref().reset_depth_stencil_target();

	render_sky();
	//	Render clouds before combine without Z-test
	//	to avoid siluets. However, it's a bit slower process.
	//render_clouds();
	
	END_CPUGPU_TIMER;
	
	m_context->set_w					( float4x4().identity() );

	backend::ref().reset_render_targets();
	backend::ref().reset_depth_stencil_target();
}

void stage_skybox::render_sky()
{
	// clouds_sh.create		( "clouds","null");
	//. this is the bug-fix for the case when the sky is broken
	//. for some unknown reason the geoms happen to be invalid sometimes
	//. if vTune show this in profile, please add simple cache ( move-to-forward last found) 
	//. to the following functions:
	//.		CResourceManager::_CreateDecl
	//.		CResourceManager::CreateGeom

	//if( env.bNeed_re_create_env)
	//{
	//	sh_2sky.create			( &m_b_skybox,"skybox_2t");
	//	sh_2geom.create			( v_skybox_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
	//	clouds_sh.create		( "clouds","null");
	//	clouds_geom.create		( v_clouds_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
	//	env.bNeed_re_create_env		= FALSE;
	//}
	rm_far();

	//dxEnvDescriptorMixerRender &mixRen = *( dxEnvDescriptorMixerRender*)&*env.CurrentEnv->m_pDescriptorMixer;

	// draw sky box
	float4x4	mat_sky;
	mat_sky = math::create_rotation_y/*rotateY*/( /*env.CurrentEnv->sky_rotation*/0);
	mat_sky = math::mul4x3( math::create_translation/*translate_over*/( m_context->get_view_pos()), mat_sky);

	u32	i_offset, v_offset;
	u32	C = math::color_rgba( 226u, 226u, 226u, 215u/*iFloor( env.CurrentEnv->sky_color.x*255.f), iFloor( env.CurrentEnv->sky_color.y*255.f), iFloor( env.CurrentEnv->sky_color.z*255.f), iFloor( env.CurrentEnv->weight*255.f)*/);

	// Fill index buffer
	u16*	pib	= backend::ref().index.lock( 20*3, i_offset);
	CopyMemory( pib, hbox_faces, 20*3*sizeof(u16));
	backend::ref().index.unlock();

	// Fill vertex buffer
	v_skybox* pv = ( v_skybox*)backend::ref().vertex.lock( 12, sizeof(v_skybox)/*m_g_skybox->get_stride()*/, v_offset);
	for ( u32 v=0; v<12; v++)
		pv[v].set( float3( hbox_verts[v*2][0], hbox_verts[v*2][1], hbox_verts[v*2][2]), C, float3( hbox_verts[v*2+1][0], hbox_verts[v*2+1][1], hbox_verts[v*2+1][2]));
	backend::ref().vertex.unlock();
	
	// Render
	m_context->set_w( mat_sky);
	
	if (m_context->scene()->get_sky_material().c_ptr())
	{
		material_effects& me = m_context->scene()->get_sky_material()->get_material_effects();
		
		if (!me.m_effects[forward_render_stage].c_ptr())
			m_sh_sky->apply();
		else
			me.m_effects[forward_render_stage]->apply( 1 );
	}
	else
	{
		m_sh_sky->apply();
	}
	
	//light*	sun = &*m_context->scene()->lights().get_sun().c_ptr();
	
	//if (sun)
	//	backend::ref().set_ps_constant(m_c_sun_direction, sun->direction);
	//else
	//	backend::ref().set_ps_constant(m_c_sun_direction, float3(0.0f, 0.0f, 0.0f));
	
	m_g_skybox->apply();
	
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 20*3, i_offset, v_offset);

	// Sun
	rm_normal();

	m_context->set_w_identity();
}

void stage_skybox::render_clouds()
{
	//::Render->rmFar				();

	//float4x4						mXFORM, mScale;
	//mScale.scale				( 10,0.4f,10);
	//mXFORM.rotateY				( env.CurrentEnv->sky_rotation);
	//mXFORM.mulB_43				( mScale);
	//mXFORM.translate_over		( Device.vCameraPosition);

	//Fvector wd0,wd1;
	//float4 wind_dir;
	//wd0.setHP					( PI_DIV_4,0);
	//wd1.setHP					( PI_DIV_4+PI_DIV_8,0);
	//wind_dir.set				( wd0.x,wd0.z,wd1.x,wd1.z).mul( 0.5f).add( 0.5f).mul( 255.f);
	//u32		i_offset,v_offset;
	//u32		C0					= utils::color_rgba( iFloor( wind_dir.x),iFloor( wind_dir.y),iFloor( wind_dir.w),iFloor( wind_dir.z));
	//u32		C1					= utils::color_rgba( iFloor( env.CurrentEnv->clouds_color.x*255.f),iFloor( env.CurrentEnv->clouds_color.y*255.f),iFloor( env.CurrentEnv->clouds_color.z*255.f),iFloor( env.CurrentEnv->clouds_color.w*255.f));

	//// Fill index buffer
	//u16*	pib					= RCache.Index.Lock	( env.CloudsIndices.size(),i_offset);
	//CopyMemory					( pib,&env.CloudsIndices.front(),env.CloudsIndices.size()*sizeof( u16));
	//RCache.Index.Unlock			( env.CloudsIndices.size());

	//// Fill vertex buffer
	//v_clouds* pv				= ( v_clouds*)	RCache.Vertex.Lock	( env.CloudsVerts.size(),clouds_geom.stride(),v_offset);
	//for ( FvectorIt it=env.CloudsVerts.begin(); it!=env.CloudsVerts.end(); it++,pv++)
	//	pv->set					( *it,C0,C1);
	//RCache.Vertex.Unlock		( env.CloudsVerts.size(),clouds_geom.stride());

	//// Render
	//RCache.set_xform_world		( mXFORM);
	//RCache.set_Geometry			( clouds_geom);
	//RCache.set_Shader			( clouds_sh);
	//dxEnvDescriptorMixerRender	&mixRen = *( dxEnvDescriptorMixerRender*)&*env.CurrentEnv->m_pDescriptorMixer;
	//RCache.set_Textures			( &mixRen.clouds_r_textures);
	//RCache.Render				( D3DPT_TRIANGLELIST,v_offset,0,env.CloudsVerts.size(),i_offset,env.CloudsIndices.size()/3);

	//::Render->rmNormal			();
}


} // namespace render
} // namespace xray

