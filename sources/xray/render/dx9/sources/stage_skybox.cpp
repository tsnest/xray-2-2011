////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_skybox.h"
#include <xray/render/common/sources/environment.h>
#include <xray/render/common/sources/shader_manager.h>

namespace xray {
namespace render{

class blender_skybox : public blender
{
public:
	virtual void compile(blender_compiler& compiler, const blender_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETER	( options );
		shader_defines_list	defines;
		make_defines(defines);

		compiler.begin_technique(/*sky box*/)
			.begin_pass("sky2", "sky2", defines)
				.set_z_test(FALSE)
				.def_sampler_clf("s_sky0", "sky/sky_9_cube")
				.def_sampler_clf("s_sky1", "sky/sky_19_cube")
				//.def_sampler_rtf("s_tonemap", "$user$tonemap")
			.end_pass()
		.end_technique();
	}
};

#pragma pack(push,1)
struct v_skybox				{
	float3	p;
	u32			color;
	float3	uv	[2];

	void		set			(const float3& _p, u32 _c, const float3& _tc)
	{
		p					= _p;
		color				= _c;
		uv[0]				= _tc;
		uv[1]				= _tc;
	}
};
const	u32 v_skybox_fvf	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(0) | D3DFVF_TEXCOORDSIZE3(1);

struct v_clouds				{
	float3	p;
	u32			color;
	u32			intensity;
	void		set			(const float3& _p, u32 _c, u32 _i)
	{
		p					= _p;
		color				= _c;
		intensity			= _i;
	}
};
const	u32 v_clouds_fvf	= D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_SPECULAR;
#pragma pack(pop)

stage_skybox::stage_skybox(scene_context* context): stage(context)
{
	blender_skybox	b_skybox;

	backend& be = backend::get_ref();

	m_sh_sky	=	shader_manager::get_ref().create_shader(&b_skybox, "skybox_2t");
	m_g_skybox	=	shader_manager::get_ref().create_geometry(v_skybox_fvf, be.vertex.get_buffer(), be.index.get_buffer());
}

stage_skybox::~stage_skybox()
{
}

//////////////////////////////////////////////////////////////////////////
// half box def
static	float	hbox_verts[24][3]	=
{
	{-1.f,	-1.f,	-1.f}, {-1.f,	-1.01f,	-1.f},	// down
	{ 1.f,	-1.f,	-1.f}, { 1.f,	-1.01f,	-1.f},	// down
	{-1.f,	-1.f,	 1.f}, {-1.f,	-1.01f,	 1.f},	// down
	{ 1.f,	-1.f,	 1.f}, { 1.f,	-1.01f,	 1.f},	// down
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

void stage_skybox::execute( )
{
	PIX_EVENT( stage_skybox );

	set_rt( m_context->m_rt_generic_0, m_context->m_rt_generic_1, 0, m_context->m_base_zb);

	backend::get_ref().set_color_write_enable();
	backend::get_ref().set_z(false);
	backend::get_ref().set_cull_mode(D3DCULL_CCW);
	render_sky();
	//	Render clouds before combine without Z-test
	//	to avoid siluets. However, it's a bit slower process.
	render_clouds();

	backend::get_ref().set_z(true);
	backend::get_ref().set_stencil(FALSE);
}

void stage_skybox::render_sky()
{
	backend& be = backend::get_ref();

	// clouds_sh.create		("clouds","null");
	//. this is the bug-fix for the case when the sky is broken
	//. for some unknown reason the geoms happen to be invalid sometimes
	//. if vTune show this in profile, please add simple cache (move-to-forward last found) 
	//. to the following functions:
	//.		CResourceManager::_CreateDecl
	//.		CResourceManager::CreateGeom

	//if(env.bNeed_re_create_env)
	//{
	//	sh_2sky.create			(&m_b_skybox,"skybox_2t");
	//	sh_2geom.create			(v_skybox_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
	//	clouds_sh.create		("clouds","null");
	//	clouds_geom.create		(v_clouds_fvf,RCache.Vertex.Buffer(), RCache.Index.Buffer());
	//	env.bNeed_re_create_env		= FALSE;
	//}
	rm_far();

	//dxEnvDescriptorMixerRender &mixRen = *(dxEnvDescriptorMixerRender*)&*env.CurrentEnv->m_pDescriptorMixer;

	// draw sky box
	float4x4	mat_sky;
	mat_sky = math::create_rotation_y/*rotateY*/(/*env.CurrentEnv->sky_rotation*/0);
	mat_sky = math::mul4x3(math::create_translation/*translate_over*/(m_context->get_view_pos()), mat_sky);

	u32	i_offset, v_offset;
	u32	C = utils::color_rgba(226, 226, 226, 215/*iFloor(env.CurrentEnv->sky_color.x*255.f), iFloor(env.CurrentEnv->sky_color.y*255.f), iFloor(env.CurrentEnv->sky_color.z*255.f), iFloor(env.CurrentEnv->weight*255.f)*/);

	// Fill index buffer
	u16*	pib	= be.index.lock(20*3, i_offset);
	CopyMemory(pib, hbox_faces, 20*3*2);
	be.index.unlock(20*3);

	// Fill vertex buffer
	v_skybox* pv = (v_skybox*)be.vertex.lock(12, m_g_skybox->get_stride(), v_offset);
	for (u32 v=0; v<12; v++)
		pv[v].set(float3(hbox_verts[v*2][0], hbox_verts[v*2][1], hbox_verts[v*2][2]), C, float3(hbox_verts[v*2+1][0], hbox_verts[v*2+1][1], hbox_verts[v*2+1][2]));
	be.vertex.unlock(12, m_g_skybox->get_stride());

	// Render
	m_context->set_w(mat_sky);

	m_g_skybox->apply();
	m_sh_sky->apply();

	backend::get_ref().set_stencil(FALSE);
	be.render(D3DPT_TRIANGLELIST, v_offset, 0, 12, i_offset, 20);

	// Sun
	rm_normal();

	m_context->set_w_identity();
}

void stage_skybox::render_clouds()
{
	//::Render->rmFar				();

	//float4x4						mXFORM, mScale;
	//mScale.scale				(10,0.4f,10);
	//mXFORM.rotateY				(env.CurrentEnv->sky_rotation);
	//mXFORM.mulB_43				(mScale);
	//mXFORM.translate_over		(Device.vCameraPosition);

	//Fvector wd0,wd1;
	//float4 wind_dir;
	//wd0.setHP					(PI_DIV_4,0);
	//wd1.setHP					(PI_DIV_4+PI_DIV_8,0);
	//wind_dir.set				(wd0.x,wd0.z,wd1.x,wd1.z).mul(0.5f).add(0.5f).mul(255.f);
	//u32		i_offset,v_offset;
	//u32		C0					= utils::color_rgba(iFloor(wind_dir.x),iFloor(wind_dir.y),iFloor(wind_dir.w),iFloor(wind_dir.z));
	//u32		C1					= utils::color_rgba(iFloor(env.CurrentEnv->clouds_color.x*255.f),iFloor(env.CurrentEnv->clouds_color.y*255.f),iFloor(env.CurrentEnv->clouds_color.z*255.f),iFloor(env.CurrentEnv->clouds_color.w*255.f));

	//// Fill index buffer
	//u16*	pib					= RCache.Index.Lock	(env.CloudsIndices.size(),i_offset);
	//CopyMemory					(pib,&env.CloudsIndices.front(),env.CloudsIndices.size()*sizeof(u16));
	//RCache.Index.Unlock			(env.CloudsIndices.size());

	//// Fill vertex buffer
	//v_clouds* pv				= (v_clouds*)	RCache.Vertex.Lock	(env.CloudsVerts.size(),clouds_geom.stride(),v_offset);
	//for (FvectorIt it=env.CloudsVerts.begin(); it!=env.CloudsVerts.end(); it++,pv++)
	//	pv->set					(*it,C0,C1);
	//RCache.Vertex.Unlock		(env.CloudsVerts.size(),clouds_geom.stride());

	//// Render
	//RCache.set_xform_world		(mXFORM);
	//RCache.set_Geometry			(clouds_geom);
	//RCache.set_Shader			(clouds_sh);
	//dxEnvDescriptorMixerRender	&mixRen = *(dxEnvDescriptorMixerRender*)&*env.CurrentEnv->m_pDescriptorMixer;
	//RCache.set_Textures			(&mixRen.clouds_r_textures);
	//RCache.Render				(D3DPT_TRIANGLELIST,v_offset,0,env.CloudsVerts.size(),i_offset,env.CloudsIndices.size()/3);

	//::Render->rmNormal			();
}

void stage_skybox::rm_near()
{
	D3DVIEWPORT9 VP = {0};//	{0,0,T->get_width(),T->get_height(),0,0.02f };
	R_CHK(hw_wrapper::get_ref().device()->GetViewport(&VP));
	VP.MinZ = 0;
	VP.MaxZ = 0.02f;
	R_CHK(hw_wrapper::get_ref().device()->SetViewport(&VP));
}

void stage_skybox::rm_normal()
{
	D3DVIEWPORT9 VP = {0};//	{0,0,T->get_width(),T->get_height(),0,1.f };
	R_CHK(hw_wrapper::get_ref().device()->GetViewport(&VP));
	VP.MinZ = 0;
	VP.MaxZ = 1.0f;
	R_CHK(hw_wrapper::get_ref().device()->SetViewport(&VP));
}

void stage_skybox::rm_far()
{
	D3DVIEWPORT9 VP = {0};//	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	R_CHK(hw_wrapper::get_ref().device()->GetViewport(&VP));
	VP.MinZ = 0.99999f;
	VP.MaxZ = 1.0f;
	R_CHK(hw_wrapper::get_ref().device()->SetViewport(&VP));
}

} // namespace render
} // namespace xray

