////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_CONTEXT_H_INCLUDED
#define SCENE_CONTEXT_H_INCLUDED

#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_rt.h>

namespace xray { 
namespace render_dx10 { 

class scene_context
{ 
public:
	scene_context	();

	void set_w			( const float4x4& m);
	void set_v			( const float4x4& m);
	void set_p			( const float4x4& m);

	void set_w_identity	() { set_w( math::float4x4().identity());}

	void push_set_w		( const float4x4& m) { m_w_stack.push_back( m_w); set_w( m);}
	void push_set_v		( const float4x4& m) { m_v_stack.push_back( m_v); set_v( m);}
	void push_set_p		( const float4x4& m) { m_p_stack.push_back( m_p); set_p( m);}

	void pop_w() { set_w( m_w_stack.back()); m_w_stack.pop_back();}
	void pop_v() { set_v( m_v_stack.back()); m_v_stack.pop_back();}
	void pop_p() { set_p( m_p_stack.back()); m_p_stack.pop_back();}

	const float4x4&	get_w		() const 	{ return m_w;}
	const float4x4&	get_inv_w	() const	{ return m_inv_w;}
	const float4x4&	get_v		() const	{ return m_v;}
	const float4x4&	get_inv_v	() const	{ return m_inv_v;}
	const float4x4&	get_p		() const	{ return m_p;}

	const float4x4&	get_wv		() const	{ return m_wv;}
	const float4x4&	get_vp		() const	{ return m_vp;}
	const float4x4&	get_wvp		() const	{ return m_wvp;}

	void set_view_pos				( float3 const & view_pos);
	void set_view_dir				( float3 const & view_dir);

	float3 const & get_view_pos	() const	{ return m_view_pos.xyz();}
	float3 const & get_view_dir	() const	{ return m_view_dir.xyz();}

	void reset_matrices();

// 	// setup constant callbacks 
// 	void setup_fog_params	( constant* c);
// 
// 	void setup_mat_w		( constant* c);
// 	void setup_mat_v		( constant* c);
// 	void setup_mat_v2w		( constant* c);
// 	void setup_mat_p		( constant* c);
// 	void setup_mat_wv		( constant* c);
// 	void setup_mat_vp		( constant* c);
// 	void setup_mat_wvp		( constant* c);	
// 	void setup_eye_position	( constant* c);
	//\ setup constant callbacks 


public:

	ref_rt	m_rt_position;
	ref_rt	m_rt_normal;
	ref_rt	m_rt_color;
	ref_rt	m_rt_accumulator;
	ref_rt	m_rt_color_null;
	ref_rt	m_rt_smap;

	ref_rt	m_rt_generic_0;
	ref_rt	m_rt_generic_1;

	ref_texture			m_t_generic_0;
	ref_texture			m_t_position;
	ref_texture 		m_t_normal;
	ref_texture 		m_t_color;
	ref_texture			m_t_accumulator;

	ref_geometry	m_g_quad_uv;
	ref_geometry	m_g_quad_2uv;

	ref_buffer		m_quad_ib;

	ref_effect	m_sh_accum_mask;

	u32	m_light_marker_id;

	// --Porting to DX10_
// 	ID3DRenderTargetView*	m_base_rt;
// 	ID3DDepthStencilView*	m_base_zb;

private:

	fixed_vector<float4x4, 16>	m_w_stack;
	fixed_vector<float4x4, 16>	m_v_stack;
	fixed_vector<float4x4, 16>	m_p_stack;

	float4x4b	m_w;		// Basic	- world
	float4x4b	m_inv_w;	// Derived	- world2local, cached
	float4x4b	m_v;		// Basic	- view
	float4x4b	m_inv_v;	// Derived	- view2world
	float4x4b	m_p;		// Basic	- projection
	float4x4b	m_wv;		// Derived	- world2view
	float4x4b	m_vp;		// Derived	- view2projection
	float4x4b	m_wvp;		// Derived	- world2view2projection

	float4		m_fog_params;
	float4		m_screen_resolution;

	// Used float4 for constant binding.
	float4		m_view_pos;
	float4		m_view_dir;

	constant_host const *	m_c_w;
	constant_host const *	m_c_invw;
	constant_host const *	m_c_v;
	constant_host const *	m_c_p;
	constant_host const *	m_c_wv;
	constant_host const *	m_c_vp;
	constant_host const *	m_c_wvp;
	constant_host const *	m_c_v2w;

}; // class scene_context

} // namespace render
} // namespace xray


#endif // #ifndef SCENE_CONTEXT_H_INCLUDED