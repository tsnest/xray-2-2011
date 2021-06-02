////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/scene_context.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/backend.h>

namespace xray {
namespace render_dx10 {

scene_context::scene_context	( ) :
	m_view_pos	( 0.f, 0.f, 0.f, 1.f ),
	m_view_dir	( 0.f, 0.f, 1.f, 0.1f ),
	m_w			( math::float4x4().identity() ),
	m_inv_w		( math::float4x4().identity() ),
	m_v			( math::float4x4().identity() ),
	m_inv_v		( math::float4x4().identity() ),
	m_p			( math::float4x4().identity() ),
	m_wv		( math::float4x4().identity() ),
	m_vp		( math::float4x4().identity() ),
	m_wvp		( math::float4x4().identity() ),
	m_c_w		( NULL),
	m_c_invw	( NULL),
	m_c_v		( NULL),
	m_c_p		( NULL),
	m_c_wv		( NULL),
	m_c_vp		( NULL),
	m_c_wvp		( NULL),
	m_c_v2w		( NULL)
{
	//if (marker!=Device.dwFrame)
	{
		// Near/Far
		float	n = 6000/*g_pGamePersistent->Environment().CurrentEnv->fog_near*/;
		float	f = 10000/*g_pGamePersistent->Environment().CurrentEnv->fog_far*/;
		float	r = 1/(f-n);
		m_fog_params.set		(-n*r, r, r, r);
	}
	//backend::ref().set_constant( c, m_fog_params);

	m_screen_resolution = float4( (float)device::ref().get_width(), (float)device::ref().get_height(), 1.f/device::ref().get_width(), 1.f/device::ref().get_height());

	reset_matrices();
	//////////////////////////////////////////////////////////////////////////
	m_c_w	= resource_manager::ref().register_constant_binding( constant_binding( "m_W",		&m_w));
	m_c_v	= resource_manager::ref().register_constant_binding( constant_binding( "m_V",		&m_v));
	m_c_p	= resource_manager::ref().register_constant_binding( constant_binding( "m_P",		&m_p));
	m_c_v2w = resource_manager::ref().register_constant_binding( constant_binding( "m_V2W",		&m_inv_v));
	m_c_wv	= resource_manager::ref().register_constant_binding( constant_binding( "m_WV",		&m_wv));
	m_c_vp	= resource_manager::ref().register_constant_binding( constant_binding( "m_VP",		&m_vp));
	m_c_wvp	= resource_manager::ref().register_constant_binding( constant_binding( "m_WVP",		&m_wvp));
	
	resource_manager::ref().register_constant_binding( constant_binding( "fog_params",	&m_fog_params));
	resource_manager::ref().register_constant_binding( constant_binding( "screen_res",	&m_screen_resolution));
	resource_manager::ref().register_constant_binding( constant_binding( "eye_position", &m_view_pos));
};


void scene_context::set_w(const float4x4& m)
{
	m_w = m;
	m_inv_w.try_invert(m);
	m_wv.mul4x3(m_w, m_v);
	m_wvp.mul4x4(m_wv, m_p);


	// ??????????????
	if (m_c_w) backend::ref().set_constant( m_c_w, m_w);
	if (m_c_wv) backend::ref().set_constant( m_c_wv, m_wv);
	if (m_c_wvp) backend::ref().set_constant( m_c_wvp, m_wvp);


	//m_bInvWValid	= false;
	//if (c_invw)		apply_invw();
}

void scene_context::set_v(const float4x4& m)
{
	m_v = m;
	m_inv_v.try_invert(m);

	m_view_pos = float4( m_inv_v.get().c.xyz(), 1); // float4(m_inv_v.e30, m_inv_v.e31, m_inv_v.e32, 1);
	m_view_dir = float4( m_inv_v.get().k.xyz(), 0); // float4(m_inv_v.e20, m_inv_v.e21, m_inv_v.e22, 0);

// 	float4x4 tmp_v = m_inv_v;
// 	m_inv_v = transpose(tmp_v);

	m_wv = math::mul4x3(m_w, m_v);
	m_vp = math::mul4x4(m_v, m_p);
	m_wvp = math::mul4x4(m_wv, m_p);


	// ??????????????
	if (m_c_v) backend::ref().set_constant( m_c_v, m_v);
	if (m_c_vp) backend::ref().set_constant( m_c_vp, m_vp);
	if (m_c_wv) backend::ref().set_constant( m_c_wv, m_wv);
	if (m_c_wvp) backend::ref().set_constant( m_c_wvp, m_wvp);
}

void scene_context::set_p(const float4x4& m)
{
	m_p = m;
	m_vp = math::mul4x4(m_v, m_p);
	m_wvp = math::mul4x4(m_wv, m_p);

	// ??????????????
	if (m_c_p) backend::ref().set_constant( m_c_p, m_p);
	if (m_c_vp) backend::ref().set_constant( m_c_vp, m_vp);
	if (m_c_wvp) backend::ref().set_constant( m_c_wvp, m_wvp);
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
}

// void scene_context::setup_mat_w(constant* c)
// {
// 	m_c_w = c;
// 	backend::ref().set_constant( c, m_w);
// }
// 
// void scene_context::setup_mat_v(constant* c)
// {
// 	m_c_v = c;
// 	backend::ref().set_constant( c, m_v);
// }
// 
// void scene_context::setup_mat_v2w(constant* c)
// {
// 	m_c_v = c;
// 	backend::ref().set_constant( c, m_inv_v);
// }
// 
// void scene_context::setup_mat_p(constant* c)
// {
// 	m_c_p = c;
// 	backend::ref().set_constant( c, m_p);
// }
// 
// void scene_context::setup_mat_wv(constant* c)
// {
// 	m_c_wv = c;
// 	backend::ref().set_constant( c, m_wv);
// }
// 
// void scene_context::setup_mat_vp(constant* c)
// {
// 	m_c_vp = c;
// 	backend::ref().set_constant( c, m_vp);
// }
// 
// void scene_context::setup_mat_wvp(constant* c)
// {
// 	m_c_wvp = c;
// 	backend::ref().set_constant( c, m_wvp);
// }

// void scene_context::setup_fog_params(constant* c)
// {
// 	float4	result;
// 	//if (marker!=Device.dwFrame)
// 	{
// 		// Near/Far
// 		float	n = 600/*g_pGamePersistent->Environment().CurrentEnv->fog_near*/;
// 		float	f = 1000/*g_pGamePersistent->Environment().CurrentEnv->fog_far*/;
// 		float	r = 1/(f-n);
// 		result.set		(-n*r, r, r, r);
// 	}
// 	backend::ref().set_constant( c,result);
// }
// 
// void scene_context::setup_eye_position(constant* c)
// {
// 	backend::ref().set_constant( c, float4(get_view_pos(), 1));
// }

} // namespace render
} // namespace xray
