////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage_lights.h"
#include <xray/render/common/sources/lights_db.h>
#include <xray/render/common/sources/du_sphere.h>
#include <xray/render/common/sources/blender_light_spot.h>
#include <xray/render/common/sources/shader_manager.h>
#include <xray/render/dx9/sources/model_manager.h>
#include <xray/render/common/sources/blender_light_mask.h>

namespace xray {
namespace render {

stage_lights::stage_lights(scene_context* context): stage(context)
{
	blender_light_spot	b_spot;
	m_sh_accum_point = shader_manager::get_ref().create_shader(&b_spot, "r2\\accum_point_s");

	u32	dwUsage				= D3DUSAGE_WRITEONLY;

	// vertices
	{
		u32		vCount		= DU_SPHERE_NUMVERTEX;
		u32		vSize		= 3*4;
		R_CHK(hw_wrapper::get_ref().device()->CreateVertexBuffer(
			vCount*vSize,
			dwUsage,
			0,
			D3DPOOL_MANAGED,
			&m_accum_point_vb,
			0));
		BYTE*	pData				= 0;
		
		R_CHK(m_accum_point_vb->Lock(0,0,(void**)&pData,0));
		CopyMemory(pData,du_sphere_vertices,vCount*vSize);
		m_accum_point_vb->Unlock	();
	}

	// indices_type
	{
		u32		iCount		= DU_SPHERE_NUMFACES*3;

		BYTE*	pData = 0;
		R_CHK(hw_wrapper::get_ref().device()->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_accum_point_ib,0));
		R_CHK(m_accum_point_ib->Lock(0,0,(void**)&pData,0));
		CopyMemory(pData,du_sphere_faces,iCount*2);
		m_accum_point_ib->Unlock	();
	}

	m_g_accum_point = shader_manager::get_ref().create_geometry(D3DFVF_XYZ, m_accum_point_vb, m_accum_point_ib);
}

stage_lights::~stage_lights()
{
//#ifdef DEBUG
//	_SHOW_REF("g_accum_point_ib",m_g_accum_point_ib);
//#endif // DEBUG
	safe_release(m_accum_point_ib);
//#ifdef DEBUG
//	_SHOW_REF("g_accum_point_vb",m_g_accum_point_vb);
//#endif // DEBUG
	safe_release(m_accum_point_vb);}

void stage_lights::execute( )
{
	PIX_EVENT( stage_lights );

	set_rt	( m_context->m_rt_accumulator, 0, 0, m_context->m_base_zb);

/////////////// Editor lights rendering //////////////////////////////////

	if (model_manager::get_ref().get_draw_editor())
	{
		lights_db::editor_lights&	e_lights = lights_db::get_ref().get_editors_lights();

		lights_db::editor_lights::iterator	e_it  = e_lights.begin(),
											e_end = e_lights.end();

		for (; e_it!=e_end; ++e_it)
			accum_point(&*(e_it->light));

	}
	if (model_manager::get_ref().get_draw_game())
	{
		lights_db::editor_lights&	e_lights = lights_db::get_ref().get_game_lights();

		lights_db::editor_lights::iterator	e_it  = e_lights.begin(),
											e_end = e_lights.end();

		for (; e_it!=e_end; ++e_it)
			accum_point(&*(e_it->light));

	}


	m_context->set_w_identity();
}

void stage_lights::accum_point(light* l)
{
	increment_light_marker();

	backend&	be = backend::get_ref();
	//phase_accumulator				();
	//RImplementation.stats.l_visible	++;

	//ref_shader		shader			= L->s_point;
	//if (!shader)	shader			= m_sh_accum_point;
	
	ref_shader	shader = m_sh_accum_point;

	//float4x4 Pold;//=Fidentity;
	//Pold.identity();
	//float4x4 FTold = m_context->m_full_xform;//=Fidentity;

	//if (L->flags.bHudMode)
	//{
	//	extern ENGINE_API float		psHUD_FOV;
	//	Pold				= Device.mProject;
	//	FTold				= Device.mFullTransform;
	//	Device.mProject.build_projection(
	//		deg2rad(psHUD_FOV*Device.fFOV /* *Device.fASPECT*/ ), 
	//		Device.fASPECT, VIEWPORT_NEAR, 
	//		g_pGamePersistent->Environment().CurrentEnv->far_plane);

	//	Device.mFullTransform.mul	(Device.mProject, Device.mView);
	//	be.set_xform_project	(Device.mProject);
	//	RImplementation.rmNear		();
	//}

	// Common
	float3		L_pos;
	float		L_spec;
	float		L_R					= l->range*0.95f;
	float3		L_clr;				L_clr.set		(l->color.r,l->color.g,l->color.b);
	L_spec	=	utils::u_diffuse2s	(L_clr);
	L_pos = m_context->get_v().transform_position(l->position);

	// Xforms
	l->xform_calc();
	m_context->set_w(l->m_xform);
	
	//be.set_xform_world			(l->m_xform);
	//be.set_xform_view			(Device.mView);
	//be.set_xform_project		(Device.mProject);
	//enable_scissor					(L);
	//enable_dbt_bounds				(L);

	// *****************************	Mask by stencil		*************************************
	// *** similar to "Carmack's reverse", but assumes convex, non intersecting objects,
	// *** thus can cope without stencil clear with 127 lights
	// *** in practice, 'cause we "clear" it back to 0x1 it usually allows us to > 200 lights :)

	be.set_z(TRUE);
	m_context->m_sh_accum_mask->apply(blender_light_mask::tech_mask_point);			// masker
	be.set_color_write_enable(FALSE);

	// backfaces: if (stencil>=1 && zfail)	stencil = light_id
	be.set_cull_mode(D3DCULL_CW);
	be.set_stencil(TRUE, D3DCMP_LESSEQUAL, m_context->m_light_marker_id, 0x01, 0xff, D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
	draw_sphere(l);

	//// frontfaces: if (stencil>=light_id && zpass)	stencil = 0x1
	be.set_cull_mode(D3DCULL_CCW);

								//       | 
								// these V need to be changed to a constant or variable 
	be.set_stencil(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0xff,D3DSTENCILOP_KEEP, D3DSTENCILOP_REPLACE, D3DSTENCILOP_KEEP);
	draw_sphere(l);

	//// nv-stencil recompression
	//if (RImplementation.o.nvstencil)		u_stencil_optimize();

	// *****************************	Minimize overdraw	*************************************
	// Select shader (front or back-faces), *** back, if intersect near plane
	be.set_color_write_enable();
	be.set_cull_mode(D3DCULL_CW);		// back
	/*
	if (bIntersect)	be.set_CullMode		(CULL_CW);		// back
	else			be.set_CullMode		(CULL_CCW);		// front
	*/

	// 2D texgens 
	float4x4			m_texgen;			u_compute_texgen_screen	(m_texgen	);

	// Draw volume with projective texgen
	{
		// Select shader
		u32		_id					= 0;
		//if (L->flags.bShadow)		{
		//	bool	bFullSize			= (L->X.S.size == u32(RImplementation.o.smapsize));
		//	if (L->X.S.transluent)	_id	= SE_L_TRANSLUENT;
		//	else if		(bFullSize)	_id	= SE_L_FULLSIZE;
		//	else					_id	= SE_L_NORMAL;
		//} else {
		//	_id						= SE_L_UNSHADOWED;
		//	//m_Shadow				= m_Lmap;
		//}

		_id = blender_light_spot::tech_unshadowed/*SE_L_NORMAL*/;

		float4x4	ident;
		ident.identity();
		//get_w().set_world_matrix(ident);
		shader->apply(_id);

		// Constants
		be.set_c					("Ldynamic_pos",	L_pos.x,L_pos.y,L_pos.z,1/(L_R*L_R));
		be.set_c					("Ldynamic_color",	L_clr.x,L_clr.y,L_clr.z,L_spec);
		//!!!!!!!!!!!! FIX THIS !!!!!!!!!!!!!!!!!!!! trashes dt_scaler!!! is it even working?????
		be.set_c("m_texgen", m_texgen);

//		// Fetch4 : enable
//		if (RImplementation.o.HW_smap_FETCH4)	{
//			//. we hacked the shader to force smap on S0
//#			define FOURCC_GET4  MAKEFOURCC('G','E','T','4') 
//			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET4 );
//		}

		// Render if (stencil >= light_id && z-pass)
		be.set_stencil(TRUE,D3DCMP_LESSEQUAL,m_context->m_light_marker_id,0xff,0x00,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP,D3DSTENCILOP_KEEP);
		draw_sphere(l);

//		// Fetch4 : disable
//		if (RImplementation.o.HW_smap_FETCH4)	{
//			//. we hacked the shader to force smap on S0
//#			define FOURCC_GET1  MAKEFOURCC('G','E','T','1') 
//			HW.pDevice->SetSamplerState	( 0, D3DSAMP_MIPMAPLODBIAS, FOURCC_GET1 );
//		}
	}

	//// blend-copy
	//if (!RImplementation.o.fp16_blend)	{
	//	u_setrt						(rt_Accumulator,NULL,NULL,HW.pBaseZB);
	//	be.set_Element			(s_accum_mask->E[SE_MASK_ACCUM_VOL]	);
	//	be.set_c				("m_texgen",		m_Texgen);
	//	draw_volume					(L);
	//}

	//R_CHK(HW.pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE));

	//dwLightMarkerID					+=	2;	// keep lowest bit always setted up

	//u_DBT_disable				();



	//if (L->flags.bHudMode)
	//{
	//	RImplementation.rmNormal					();
	//	// Restore projection
	//	Device.mProject				= Pold;
	//	Device.mFullTransform		= FTold;
	//	be.set_xform_project	(Device.mProject);
	//}

	//m_context->m_full_xform = FTold;
}

void stage_lights::draw_sphere(light* )
{
	m_g_accum_point->apply();
	backend::get_ref().render(D3DPT_TRIANGLELIST, 0, 0, DU_SPHERE_NUMVERTEX, 0, DU_SPHERE_NUMFACES);
}

void stage_lights::reset_light_marker( bool bResetStencil)
{
	backend&	be = backend::get_ref();
	m_context->m_light_marker_id = stencil_light_marker_start;
	
	if (bResetStencil)
	{
		u32		offset;
		
		float	_w	= float(hw_wrapper::get_ref().get_width());
		float	_h	= float(hw_wrapper::get_ref().get_width());
		u32		C	= utils::color_rgba(255,255,255,255);
		float	eps	= math::epsilon_3;
		
		FVF::TL* pv	= (FVF::TL*) be.vertex.lock(4, m_context->m_g_quad_uv->get_stride(), offset);
		pv->set(eps,			float(_h+eps),	eps,	1.f, C, 0, 0);	pv++;
		pv->set(eps,			eps,			eps,	1.f, C, 0, 0);	pv++;
		pv->set(float(_w+eps),	float(_h+eps),	eps,	1.f, C, 0, 0);	pv++;
		pv->set(float(_w+eps),	eps,			eps,	1.f, C, 0, 0);	pv++;
		be.vertex.unlock(4, m_context->m_g_quad_uv->get_stride());
		
		be.set_cull_mode(D3DCULL_NONE);
		be.set_color_write_enable(FALSE);
		//	Clear everything except last bit
		be.set_stencil(TRUE, D3DCMP_ALWAYS, 0x00, 0x00, 0xFE, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO);
		//RCache.set_Stencil	(TRUE,D3DCMP_ALWAYS,dwLightMarkerID,0x00,0xFF, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO, D3DSTENCILOP_ZERO);
		m_context->m_sh_accum_mask->apply(blender_light_mask::tech_mask_stencil);
		m_context->m_g_quad_uv->apply();
		be.render(D3DPT_TRIANGLELIST,offset,0,4,0,2);

/*
		u32		Offset;
		float	_w					= float(Device.dwWidth);
		float	_h					= float(Device.dwHeight);
		u32		C					= utils::color_rgba	(255,255,255,255);
		float	eps					= 0;
		float	_dw					= 0.5f;
		float	_dh					= 0.5f;
		FVF::TL* pv					= (FVF::TL*) RCache.Vertex.Lock	(4,g_combine->vb_stride,Offset);
		pv->set						(-_dw,		_h-_dh,		eps,	1.f, C, 0, 0);	pv++;
		pv->set						(-_dw,		-_dh,		eps,	1.f, C, 0, 0);	pv++;
		pv->set						(_w-_dw,	_h-_dh,		eps,	1.f, C, 0, 0);	pv++;
		pv->set						(_w-_dw,	-_dh,		eps,	1.f, C, 0, 0);	pv++;
		RCache.Vertex.Unlock		(4,g_combine->vb_stride);
		RCache.set_Element			(s_occq->E[2]	);
		RCache.set_Geometry			(g_combine		);
		RCache.Render				(D3DPT_TRIANGLELIST,Offset,0,4,0,2);
*/
	}
}

void stage_lights::increment_light_marker()
{
#pragma message(XRAY_TODO("literal \"2\" need to be a constant depending on bits given for light marking."))

	m_context->m_light_marker_id += 2;

	//if (dwLightMarkerID>10)
	if (m_context->m_light_marker_id>255)
		reset_light_marker(true);
}

} // namespace render
} // namespace xray
