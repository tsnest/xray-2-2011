////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "stage.h"

namespace xray {
namespace render{

stage::stage(scene_context* context): m_context(context)
{
}


void stage::set_default_rt()
{
	backend& be = backend::get_ref();
	be.set_rt(target_rt0, m_context->m_base_rt);
	be.set_rt(target_rt1, 0);
	be.set_rt(target_rt2, 0);
	be.set_rt(target_zb, m_context->m_base_zb);
}

void stage::set_rt(ref_rt _1, ref_rt _2, ref_rt _3, ID3DSurface* zb)
{
	backend& be = backend::get_ref();
	if (_1) _1->apply(target_rt0); else be.set_rt(target_rt0, 0);
	if (_2) _2->apply(target_rt1); else be.set_rt(target_rt1, 0);
	if (_3) _3->apply(target_rt2); else be.set_rt(target_rt2, 0);
	be.set_rt(target_zb, zb);
}

void stage::rm_near()
{
	D3DVIEWPORT9 VP = {0};//	{0,0,T->get_width(),T->get_height(),0,0.02f };
	R_CHK(hw_wrapper::get_ref().device()->GetViewport(&VP));
	VP.MinZ = 0;
	VP.MaxZ = 0.02f;
	R_CHK(hw_wrapper::get_ref().device()->SetViewport(&VP));
}

void stage::rm_normal()
{
	D3DVIEWPORT9 VP = {0};//	{0,0,T->get_width(),T->get_height(),0,1.f };
	R_CHK(hw_wrapper::get_ref().device()->GetViewport(&VP));
	VP.MinZ = 0;
	VP.MaxZ = 1.0f;
	R_CHK(hw_wrapper::get_ref().device()->SetViewport(&VP));
}

void stage::rm_far()
{
	D3DVIEWPORT9 VP = {0};//	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	R_CHK(hw_wrapper::get_ref().device()->GetViewport(&VP));
	VP.MinZ = 0.99999f;
	VP.MaxZ = 1.0f;
	R_CHK(hw_wrapper::get_ref().device()->SetViewport(&VP));
}

//!!!!!!!!!!!!!!!!!!!!!FIX THIS!!!!!!!!!!!!!!!!!!!!!!
void stage::u_compute_texgen_jitter(float4x4& m_Texgen_J)
{
	// place into	0..1 space
	float4x4			m_TexelAdjust = float4x4(
		float4(0.5f,	0.0f,	0.0f,	0.0f),
		float4(0.0f,	-0.5f,	0.0f,	0.0f),
		float4(0.0f,	0.0f,	1.0f,	0.0f),
		float4(0.5f,	0.5f,	0.0f,	1.0f)
	);
	m_Texgen_J = math::mul4x4(m_context->get_wvp()/*m_full_xform*/, m_TexelAdjust);

	// rescale - tile it
	float	scale_X			= float(hw_wrapper::get_ref().get_width())	/ float(tex_jitter);
	float	scale_Y			= float(hw_wrapper::get_ref().get_height()) / float(tex_jitter);
	float	offset			= (.5f / float(tex_jitter));
	m_TexelAdjust = create_scale(float3(scale_X, scale_Y, 1.f));
	m_TexelAdjust = math::mul4x4(math::create_translation(float3(offset, offset, 0)), m_TexelAdjust);
	m_Texgen_J = math::mul4x4(m_TexelAdjust, m_Texgen_J);
}

void stage::u_compute_texgen_screen(float4x4& m_Texgen)
{
	float	_w						= float(hw_wrapper::get_ref().get_width());
	float	_h						= float(hw_wrapper::get_ref().get_height());
	float	o_w						= (.5f / _w);
	float	o_h						= (.5f / _h);
	float4x4	m_TexelAdjust		= float4x4(
		float4(0.5f,		0.0f,			0.0f,	0.0f),
		float4(0.0f,		-0.5f,			0.0f,	0.0f),
		float4(0.0f,		0.0f,			1.0f,	0.0f),
		float4(0.5f + o_w,	0.5f + o_h,		0.0f,	1.0f)
	);
	m_Texgen = math::mul4x4(m_context->get_wvp()/*m_full_xform*/, m_TexelAdjust);
}

} // namespace render
} // namespace xray
