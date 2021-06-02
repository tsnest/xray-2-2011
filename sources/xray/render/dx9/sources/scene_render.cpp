////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (color) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_render.h"
#include <xray/render/common/sources/blender_combine.h>
#include <xray/render/common/sources/environment.h>
#include "model_manager.h"
#include "visual.h"
#include "stage_gbuffer.h"
#include "stage_pre_lighting.h"
#include "stage_sun.h"
#include "stage_sun_materials.h"
#include "stage_skybox.h"
#include "stage_combine.h"
#include "stage_lights.h"
#include "stage_screen_image.h"
#include "scene_context.h"


namespace xray {
namespace render {


scene_render::scene_render(bool const render_test_scene): stage(&m_context)
{
	resource_manager::get_ref().register_const_setup("fog_params", const_setup_cb(&m_context, &scene_context::setup_fog_params));

	m_context.reset_matrices();

	//if (0)
	resource_manager::get_ref().register_const_setup("m_W", const_setup_cb(&m_context, &scene_context::setup_mat_w));
	resource_manager::get_ref().register_const_setup("m_V", const_setup_cb(&m_context, &scene_context::setup_mat_v));
	resource_manager::get_ref().register_const_setup("m_P", const_setup_cb(&m_context, &scene_context::setup_mat_p));
	//resource_manager::get_ref().register_const_setup("m_invW", const_setup_cb(this, &scene_render::setup_mat_inv_w));
	resource_manager::get_ref().register_const_setup("m_V2W", const_setup_cb(&m_context, &scene_context::setup_mat_v2w));
	resource_manager::get_ref().register_const_setup("m_WV", const_setup_cb(&m_context, &scene_context::setup_mat_wv));
	resource_manager::get_ref().register_const_setup("m_VP", const_setup_cb(&m_context, &scene_context::setup_mat_vp));
	resource_manager::get_ref().register_const_setup("m_WVP", const_setup_cb(&m_context, &scene_context::setup_mat_wvp));
	resource_manager::get_ref().register_const_setup("eye_position", const_setup_cb(&m_context, &scene_context::setup_eye_position));

	u32 w = hw_wrapper::get_ref().get_width();
	u32 h = hw_wrapper::get_ref().get_height();

	m_context.m_rt_position		= resource_manager::get_ref().create_rt(r2_rt_p, w, h, D3DFMT_A16B16G16R16F);
	m_context.m_rt_normal		= resource_manager::get_ref().create_rt(r2_rt_n, w, h, D3DFMT_A16B16G16R16F);
	m_context.m_rt_color		= resource_manager::get_ref().create_rt(r2_rt_albedo, w, h, D3DFMT_A8R8G8B8);//take into account mixed_depth!!!!!!!!
	m_context.m_rt_accumulator	= resource_manager::get_ref().create_rt(r2_rt_accum, w, h, D3DFMT_A16B16G16R16F);

	// generic(LDR) RTs
	m_context.m_rt_generic_0	= resource_manager::get_ref().create_rt(r2_rt_generic0, w, h, D3DFMT_A8R8G8B8);
	m_context.m_rt_generic_1	= resource_manager::get_ref().create_rt(r2_rt_generic1, w, h, D3DFMT_A8R8G8B8);

	// Textures to render targets
	m_context.m_t_position		= resource_manager::get_ref().create_texture(r2_rt_p);
	m_context.m_t_normal		= resource_manager::get_ref().create_texture(r2_rt_n);
	m_context.m_t_color			= resource_manager::get_ref().create_texture(r2_rt_albedo);
	m_context.m_t_accumulator	= resource_manager::get_ref().create_texture(r2_rt_accum);
	m_context.m_t_generic_0		= resource_manager::get_ref().create_texture(r2_rt_generic0);

	m_device = hw_wrapper::get_ref().device();
	m_device->GetRenderTarget(0, &m_context.m_base_rt);
	m_device->GetDepthStencilSurface(&m_context.m_base_zb);

	u32	smap_size = hw_wrapper::get_ref().o.smap_size;
	
	D3DFORMAT	null_fmt = hw_wrapper::get_ref().o.null_rt_format;

	//PC DX9 requires fake color surface for rendering hw smaps
	//if we can we use "null" surface format which actually does not allocate memory
	m_context.m_rt_color_null = resource_manager::get_ref().create_rt(r2_rt_smap_surf, smap_size, smap_size, null_fmt);

#pragma message(XRAY_TODO("uncomment when you are going to implement shadow maps"))	
//	D3DFORMAT	smap_fmt = hw_wrapper::get_ref().o.hw_smap_format;
//m_context.m_smap = resource_manager::get_ref().create_rt(r2_rt_smap_depth, smap_size, smap_size, smap_fmt);

	backend& be = backend::get_ref();

	create_quad_ib();
	m_context.m_g_quad_uv  = shader_manager::get_ref().create_geometry(FVF::F_TL, be.vertex.get_buffer(), m_context.m_quad_ib);
	m_context.m_g_quad_2uv = shader_manager::get_ref().create_geometry(FVF::F_TL2uv, be.vertex.get_buffer(), m_context.m_quad_ib);

	if ( render_test_scene ) {
		blender_light_mask		m_b_accum_mask;
		m_context.m_sh_accum_mask   = shader_manager::get_ref().create_shader(&m_b_accum_mask);//,   "r2\\accum_mask");
	}

	//m_t_envmap_0 = resource_manager::get_ref().create_texture("sky/sky_9_cube#small");
	//m_t_envmap_1 = resource_manager::get_ref().create_texture("sky/sky_19_cube#small");

	//s_combine_volumetric.create			("combine_volumetric");
	//s_combine_dbg_0.create				("effects\\screen_set",		r2_RT_smap_surf		);	
	//s_combine_dbg_1.create				("effects\\screen_set",		r2_RT_luminance_t8	);
	//s_combine_dbg_Accumulator.create	("effects\\screen_set",		r2_RT_accum			);
	//g_combine_VP.create					(dwDecl,		RCache.Vertex.Buffer(), RCache.QuadIB);
	//g_combine.create					(FVF::F_TL,		RCache.Vertex.Buffer(), RCache.QuadIB);
	//g_combine_2UV.create				(FVF::F_TL2uv,	RCache.Vertex.Buffer(), RCache.QuadIB);

	//u32 fvf_aa_blur				= D3DFVF_XYZRHW|D3DFVF_TEX4|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1)|D3DFVF_TEXCOORDSIZE2(2)|D3DFVF_TEXCOORDSIZE2(3);
	//g_aa_blur.create			(fvf_aa_blur,	RCache.Vertex.Buffer(), RCache.QuadIB);

	//u32 fvf_aa_AA				= D3DFVF_XYZRHW|D3DFVF_TEX7|D3DFVF_TEXCOORDSIZE2(0)|D3DFVF_TEXCOORDSIZE2(1)|D3DFVF_TEXCOORDSIZE2(2)|D3DFVF_TEXCOORDSIZE2(3)|D3DFVF_TEXCOORDSIZE2(4)|D3DFVF_TEXCOORDSIZE4(5)|D3DFVF_TEXCOORDSIZE4(6);
	//g_aa_AA.create				(fvf_aa_AA,		RCache.Vertex.Buffer(), RCache.QuadIB);
	//t_envmap_0.create(r2_T_envs0);
	//t_envmap_1.create(r2_T_envs1);

	if ( identity(true) || render_test_scene ) {

		create_material_texture();
	//	rt_smap_ZB					= NULL;

	//	if (RImplementation.o.advancedpp)
	//		s_accum_direct_volumetric.create("accum_volumetric_sun");

		m_gstage				= NEW(stage_gbuffer)		(&m_context);
		m_pre_lighting_stage	= NEW(stage_pre_lighting)	(&m_context);
		m_sun_stage				= NEW(stage_sun)			(&m_context);
		m_sun_stage_materials	= NEW(stage_sun_materials)	(&m_context);
		m_skybox_stage			= NEW(stage_skybox)			(&m_context);
		m_combine_stage			= NEW(stage_combine)		(&m_context);
		m_lighting_stage		= NEW(stage_lights)			(&m_context);
		m_present_stage			= NEW(stage_screen_image)	(&m_context);

	}
	else {
		m_gstage				= 0;
		m_pre_lighting_stage	= 0;
		m_sun_stage				= 0;
		m_sun_stage_materials	= 0;
		m_skybox_stage			= 0;
		m_combine_stage			= 0;
		m_lighting_stage		= 0;
		m_present_stage			= 0;
	}
}

scene_render::~scene_render()
{
	//model_manager::destroy_instance(m_test_visual);
	safe_release(m_context.m_base_rt);
	safe_release(m_context.m_base_zb);

	safe_release(m_context.m_quad_ib);

	DELETE(m_gstage);
	DELETE(m_pre_lighting_stage);
	DELETE(m_sun_stage);
	DELETE(m_sun_stage_materials);
	DELETE(m_skybox_stage);
	DELETE(m_combine_stage);
	DELETE(m_lighting_stage);
	DELETE(m_present_stage);
}

void scene_render::create_material_texture()
{
	ID3DTexture3D*	t_material_surf = 0;

	R_CHK(D3DXCreateVolumeTexture(hw_wrapper::get_ref().device(), tex_material_ldotn, tex_material_ldoth, 
								  4, 1, 0, D3DFMT_A8L8,D3DPOOL_MANAGED, &t_material_surf));
	m_t_material = resource_manager::get_ref().create_texture(r2_material);
	m_t_material->set_surface(t_material_surf);

	// Fill it (addr: x=dot(L,N),y=dot(L,H))
	D3DLOCKED_BOX	R = {0};
	
	R_CHK(t_material_surf->LockBox(0, &R, 0, 0));
	for (u32 slice=0; slice<4; ++slice)
	{
		for (u32 y=0; y<tex_material_ldoth; ++y)
		{
			for (u32 x=0; x<tex_material_ldotn; ++x)
			{
				u16*	p = (u16*) (LPBYTE (R.pBits) + slice*R.SlicePitch + y*R.RowPitch + x*2);
				float	fd,fs;
				float	ld = float(x)/float(tex_material_ldotn-1);
				float	ls = float(y)/float(tex_material_ldoth-1) /*+ math::epsilon_7*/;
				
				ls *= powf(ld,1/32.f);
				
				switch (slice)
				{
				case 0:
					{ // looks like OrenNayar
						fd	= powf(ld, 0.75f);		// 0.75
						fs	= 0.5f*powf(ls, 16.0f);
					}
					break;
				case 1:
					{// looks like Blinn
						fd	= powf(ld,0.90f);		// 0.90
						fs	= powf(ls,24.f);
					}
					break;
				case 2:
					{ // looks like Phong
						fd	= ld;					// 1.0
						fs  = powf(ls*1.01f, 128.f);
					}
					break;
				case 3:
					{ // looks like Metal
						float	s0	=	abs	(1-abs(0.05f*sin(33.f*ld)+ld-ls));
						float	s1	=	abs	(1-abs(0.05f*cos(33.f*ld*ls)+ld-ls));
						float	s2	=	abs	(1-abs(ld-ls));
						fd  = ld;				// 1.0
						fs  = powf(std::max(std::max(s0,s1),s2), 24.f);
						fs *= powf(ld,1/7.f);
					}
					break;
				default:
					fd	= fs = 0;
				}

				s32	_d = (s32)floor(fd*255.5f);
				math::clamp(_d, 0, 255);
				s32	_s = (s32)floor(fs*255.5f); 
				math::clamp(_s, 0, 255);

				if ((y==tex_material_ldoth-1) && (x==tex_material_ldotn-1))
				{
					_d = 255; _s=255;
				}

				*p = u16(_s*256 + _d);
			}
		}
	}
	R_CHK(t_material_surf->UnlockBox(0));
	safe_release(t_material_surf);
}

void scene_render::create_quad_ib()
{
	const u32 quad_count = 4*1024;
	const u32 idx_count  = quad_count*2*3;
	
	u16	*indices = 0;
	u32	usage = D3DUSAGE_WRITEONLY;

	if (hw_wrapper::get_ref().get_caps().geometry.software)
	{
		ASSERT(0);
		usage |= D3DUSAGE_SOFTWAREPROCESSING;
	}
	
	R_CHK(hw_wrapper::get_ref().device()->CreateIndexBuffer(
								idx_count*2, usage, D3DFMT_INDEX16,
								D3DPOOL_MANAGED, &m_context.m_quad_ib, NULL));
	R_CHK(m_context.m_quad_ib->Lock(0, 0, (void**)&indices, 0));

	int		vertex_id = 0;
	int		idx= 0;
	for (int i=0; i<quad_count; ++i)
	{
		indices[idx++]=u16(vertex_id+0);
		indices[idx++]=u16(vertex_id+1);
		indices[idx++]=u16(vertex_id+2);

		indices[idx++]=u16(vertex_id+3);
		indices[idx++]=u16(vertex_id+2);
		indices[idx++]=u16(vertex_id+1);

		vertex_id+=4;
	}

	R_CHK(m_context.m_quad_ib->Unlock());
}



void scene_render::render(const float4x4& view_mat, const float4x4& proj_mat)
{
	//PIX_EVENT( scene_render );
	//////////////////////////////////////////////////////////////////////////
	// Setup frame
	XRAY_UNREFERENCED_PARAMETERS	( view_mat, proj_mat );
	m_context.m_light_marker_id = 1;

	model_manager::get_ref().update(m_context.get_view_pos(), m_context.get_wvp());
	backend::get_ref().invalidate();

	hw_wrapper::get_ref().device()->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_LESSEQUAL	);

	//////////////////////////////////////////////////////////////////////////
	// Rendering stages 

	// G-Stage. The render targets will be cleared.
	m_gstage->execute();

	// Lighting
	m_pre_lighting_stage->execute();

// 	// Sun lighting
//  m_sun_stage->execute();
	m_sun_stage_materials->execute(stage_sun_materials::cascade_near);
	
	// dynamic lights
	m_lighting_stage->execute();

	// Ambient, hemi, skybox, postprocesses ???
	m_skybox_stage->execute();

	// Combine 1
	m_combine_stage->execute();

	// Present the final image to base render target
	math::uint2 wh = hw_wrapper::get_ref().get_size();
	m_present_stage->execute( 0, 0, (float)wh.x, (float)wh.y, m_context.m_t_generic_0);

	// This maybe need to be 
 	hw_wrapper::get_ref().device()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
 	hw_wrapper::get_ref().device()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
 	hw_wrapper::get_ref().device()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

void scene_render::test( pcstr args )
{
	XRAY_UNREFERENCED_PARAMETER	( args );

	resources::request	resources[]	= {
		{ "resources/barrel_close_1.ogf",					xray::resources::raw_data_class },
//		{ "resources/stalker_dolg_1.ogf",			xray::resources::raw_data_class },
	};

	xray::resources::query_resources	(
		resources,
		array_size(resources),
		boost::bind(&scene_render::on_test_visual_loaded, this, _1),
		render::g_allocator
	);
}

class test_blender: public blender
{
public:
	void compile(blender_compiler& compiler, const blender_compilation_options& options)
	{
		XRAY_UNREFERENCED_PARAMETER	( options );

		shader_defines_list	defines;
		make_defines(defines);

		compiler.begin_technique(/*LEVEL*/);
		compiler.begin_pass("test", "test", defines)
				.set_z_test(TRUE)
				.set_alpha_blend(FALSE)
			.end_pass()
		.end_technique();
	}

private:
};

void scene_render::on_test_visual_loaded	( xray::resources::queries_result& data )
{
	ASSERT							( !data.empty( ) );

	resources::pinned_ptr_const<u8>	ogf_data		( data[ 0 ].get_managed_resource( ) );

	memory::chunk_reader		chunk_reader(ogf_data.c_ptr(), ogf_data.size(), memory::chunk_reader::chunk_type_sequential);
	
	u16 type					= model_manager::get_visual_type(chunk_reader);
	m_test_visual				= model_manager::create_instance(type);
	m_test_visual->load			(chunk_reader);
	
	test_blender	tb;
	m_test_visual->m_shader = shader_manager::get_ref().create_shader(&tb);
}



} // namespace render 
} // namespace xray 
