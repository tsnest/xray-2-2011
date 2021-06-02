////////////////////////////////////////////////////////////////////////////
//	Created		: 16.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_CONTEXT_H_INCLUDED
#define SCENE_CONTEXT_H_INCLUDED

namespace xray {
namespace render{

class scene_context
{
public:
	inline	scene_context	( ) :
	m_view_pos	( 0.f, 0.f, 0.f ),
		m_view_dir	( 0.f, 0.f, 1.f ),
		m_w			( math::float4x4().identity() ),
		m_inv_w		( math::float4x4().identity() ),
		m_v			( math::float4x4().identity() ),
		m_inv_v		( math::float4x4().identity() ),
		m_p			( math::float4x4().identity() ),
		m_wv		( math::float4x4().identity() ),
		m_vp		( math::float4x4().identity() ),
		m_wvp		( math::float4x4().identity() )
	{
	};

	void set_w(const float4x4& m);
	void set_v(const float4x4& m);
	void set_p(const float4x4& m);

	void set_w_identity() { set_w(math::float4x4().identity());}

	void push_set_w(const float4x4& m) {m_w_stack.push_back(m_w); set_w(m);}
	void push_set_v(const float4x4& m) {m_v_stack.push_back(m_v); set_v(m);}
	void push_set_p(const float4x4& m) {m_p_stack.push_back(m_p); set_p(m);}

	void pop_w() {set_w(m_w_stack.back()); m_w_stack.pop_back();}
	void pop_v() {set_v(m_v_stack.back()); m_v_stack.pop_back();}
	void pop_p() {set_p(m_p_stack.back()); m_p_stack.pop_back();}

	const float4x4&	get_w	()		{return m_w;}
	const float4x4&	get_inv_w()		{return m_inv_w;}
	const float4x4&	get_v	()		{return m_v;}
	const float4x4&	get_inv_v()		{return m_inv_v;}
	const float4x4&	get_p	()		{return m_p;}

	const float4x4&	get_wv	()		{return m_wv;}
	const float4x4&	get_vp	()		{return m_vp;}
	const float4x4&	get_wvp	()		{return m_wvp;}

	void set_view_pos	( float3 const & view_pos );
	void set_view_dir	( float3 const & view_dir );

	float3 const & get_view_pos	( ) { return m_view_pos;}
	float3 const & get_view_dir	( ) { return m_view_dir;}

	void reset_matrices();

	// setup constant callbacks 
	void setup_fog_params(res_constant* c);

	void setup_mat_w(res_constant* c);
	void setup_mat_v(res_constant* c);
	void setup_mat_v2w(res_constant* c);
	void setup_mat_p(res_constant* c);
	void setup_mat_wv(res_constant* c);
	void setup_mat_vp(res_constant* c);
	void setup_mat_wvp(res_constant* c);
	void setup_eye_position(res_constant* c);
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

	ID3DIndexBuffer*	m_quad_ib;

	ref_shader	m_sh_accum_mask;

	u32	m_light_marker_id;

	ID3DSurface*	m_base_rt;
	ID3DSurface*	m_base_zb;

private:

	float3		m_view_pos;
	float3		m_view_dir;
	

	float4x4	m_w;		// Basic	- world
	float4x4	m_inv_w;	// Derived	- world2local, cached
	float4x4	m_v;		// Basic	- view
	float4x4	m_inv_v;	// Derived	- view2world
	float4x4	m_p;		// Basic	- projection
	float4x4	m_wv;		// Derived	- world2view
	float4x4	m_vp;		// Derived	- view2projection
	float4x4	m_wvp;		// Derived	- world2view2projection

	res_constant*	m_c_w;
	res_constant*	m_c_invw;
	res_constant*	m_c_v;
	res_constant*	m_c_p;
	res_constant*	m_c_wv;
	res_constant*	m_c_vp;
	res_constant*	m_c_wvp;
	res_constant*	m_c_v2w;

	fixed_vector<float4x4, 16>	m_w_stack;
	fixed_vector<float4x4, 16>	m_v_stack;
	fixed_vector<float4x4, 16>	m_p_stack;

}; // class scene_context

} // namespace render
} // namespace xray


#endif // #ifndef SCENE_CONTEXT_H_INCLUDED