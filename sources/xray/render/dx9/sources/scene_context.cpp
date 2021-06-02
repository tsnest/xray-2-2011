////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_context.h"

namespace xray {
namespace render{

void scene_context::set_w(const float4x4& m)
{
	m_w = m;
	m_inv_w.try_invert(m);
	m_wv = math::mul4x3(m_w, m_v);
	m_wvp = math::mul4x4(m_wv, m_p);

	if (m_c_w) backend::get_ref().set_c(m_c_w, m_w);
	if (m_c_wv) backend::get_ref().set_c(m_c_wv, m_wv);
	if (m_c_wvp) backend::get_ref().set_c(m_c_wvp, m_wvp);


	//m_bInvWValid	= false;
	//if (c_invw)		apply_invw();
}

void scene_context::set_v(const float4x4& m)
{
	m_v = m;
	m_inv_v.try_invert(m);
	m_wv = math::mul4x3(m_w, m_v);
	m_vp = math::mul4x4(m_v, m_p);
	m_wvp = math::mul4x4(m_wv, m_p);

	m_view_pos = float3(m_inv_v.e30, m_inv_v.e31, m_inv_v.e32);
	m_view_dir = float3(m_inv_v.e20, m_inv_v.e21, m_inv_v.e22);

	if (m_c_v) backend::get_ref().set_c(m_c_v, m_v);
	if (m_c_vp) backend::get_ref().set_c(m_c_vp, m_vp);
	if (m_c_wv) backend::get_ref().set_c(m_c_wv, m_wv);
	if (m_c_wvp) backend::get_ref().set_c(m_c_wvp, m_wvp);
}

void scene_context::set_p(const float4x4& m)
{
	m_p = m;
	m_vp = math::mul4x4(m_v, m_p);
	m_wvp = math::mul4x4(m_wv, m_p);

	if (m_c_p) backend::get_ref().set_c(m_c_p, m_p);
	if (m_c_vp) backend::get_ref().set_c(m_c_vp, m_vp);
	if (m_c_wvp) backend::get_ref().set_c(m_c_wvp, m_wvp);
}

void scene_context::reset_matrices()
{
	m_w.identity();
	m_inv_w.identity();
	m_v.identity();
	m_inv_v.identity();
	m_p.identity();
	m_wv.identity();
	m_vp.identity();
	m_wvp.identity();

	m_c_w = 0;
	m_c_invw = 0;
	m_c_v = 0;
	m_c_p = 0;
	m_c_wv = 0;
	m_c_vp = 0;
	m_c_wvp = 0;
}

void scene_context::setup_mat_w(res_constant* c)
{
	m_c_w = c;
	backend::get_ref().set_c(c, m_w);
}

void scene_context::setup_mat_v(res_constant* c)
{
	m_c_v = c;
	backend::get_ref().set_c(c, m_v);
}

void scene_context::setup_mat_v2w(res_constant* c)
{
	m_c_v = c;
	backend::get_ref().set_c(c, m_inv_v);
}

void scene_context::setup_mat_p(res_constant* c)
{
	m_c_p = c;
	backend::get_ref().set_c(c, m_p);
}

void scene_context::setup_mat_wv(res_constant* c)
{
	m_c_wv = c;
	backend::get_ref().set_c(c, m_wv);
}

void scene_context::setup_mat_vp(res_constant* c)
{
	m_c_vp = c;
	backend::get_ref().set_c(c, m_vp);
}

void scene_context::setup_mat_wvp(res_constant* c)
{
	m_c_wvp = c;
	backend::get_ref().set_c(c, m_wvp);
}

void scene_context::setup_fog_params(res_constant* c)
{
	float4	result;
	//if (marker!=Device.dwFrame)
	{
		// Near/Far
		float	n = 600/*g_pGamePersistent->Environment().CurrentEnv->fog_near*/;
		float	f = 1000/*g_pGamePersistent->Environment().CurrentEnv->fog_far*/;
		float	r = 1/(f-n);
		result.set		(-n*r, r, r, r);
	}
	backend::get_ref().set_c(c,result);
}

void scene_context::setup_eye_position(res_constant* c)
{
	backend::get_ref().set_c(c, float4(get_view_pos(), 1));
}

} // namespace render
} // namespace xray
