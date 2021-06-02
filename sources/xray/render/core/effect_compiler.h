////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_EFFECT_COMPILER_H_INCLUDED
#define XRAY_RENDER_CORE_EFFECT_COMPILER_H_INCLUDED

#include <xray/render/core/state_descriptor.h>
#include <xray/render/core/sampler_state_descriptor.h>
#include <xray/render/core/xs_descriptor.h>
#include <xray/render/core/shader_constant_bindings.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/shader_configuration.h>

namespace xray {
namespace render {

class sampler_state_descriptor;

class res_texture;
class resource_intrusive_base;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class shader_constant_binding;
class res_effect;

template < typename ShaderData >
class res_xs_hw;

struct ps_data;
typedef res_xs_hw<ps_data>	res_ps_hw;
typedef intrusive_ptr< res_ps_hw, resource_intrusive_base, threading::single_threading_policy>	ref_ps_hw;

struct gs_data;
typedef res_xs_hw<gs_data>	res_gs_hw;
typedef intrusive_ptr<res_gs_hw, resource_intrusive_base, threading::single_threading_policy>	ref_gs_hw;

struct vs_data;
typedef res_xs_hw<vs_data>	res_vs_hw;
typedef intrusive_ptr<res_vs_hw, resource_intrusive_base, threading::single_threading_policy>	ref_vs_hw;

struct shader_include_getter;

class effect_compiler: public boost::noncopyable
{
public:

	void			 set_mapping		();

	effect_compiler& begin_pass			(pcstr vs_name, 
										 pcstr gs_name, 
										 pcstr ps_name, 
										 shader_configuration const& shader_config, 
										 shader_include_getter* include_getter);
	effect_compiler& end_pass			();
	
	effect_compiler& begin_technique	();
	void			 end_technique		();
	
	effect_compiler& set_depth			(bool enable, bool write_enable, D3D_COMPARISON_FUNC cmp_func = D3D_COMPARISON_LESS_EQUAL);
	effect_compiler& set_stencil		(BOOL enable, u32 ref=0x00, u8 mask=0x00, u8 writemask=0x00, D3D_COMPARISON_FUNC func = D3D_COMPARISON_ALWAYS, D3D_STENCIL_OP fail=D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP pass=D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP zfail=D3D_STENCIL_OP_KEEP);
	effect_compiler& set_stencil_ref	(u32 ref);
	effect_compiler& set_alpha_blend	(BOOL is_blend_enabled /* Temporary !!!*/, 
										 D3D_BLEND src_blend = D3D_BLEND_ONE, 
										 D3D_BLEND dest_blend = D3D_BLEND_ZERO, 
										 D3D_BLEND_OP blend_op = D3D_BLEND_OP_ADD, 
										 D3D_BLEND src_alpha_blend = D3D_BLEND_ONE,
										 D3D_BLEND dest_alpha_blend = D3D_BLEND_ZERO,
										 D3D_BLEND_OP blend_alpha_op = D3D_BLEND_OP_ADD);
	effect_compiler& set_cull_mode		(D3D_CULL_MODE mode);
	effect_compiler& set_alpha_to_coverage (BOOL is_enabled );
	effect_compiler& color_write_enable	(D3D_COLOR_WRITE_ENABLE mode = D3D_COLOR_WRITE_ENABLE_ALL);
	effect_compiler& set_fill_mode		(D3D_FILL_MODE fill_mode);

	// Sampler functions
	effect_compiler& def_sampler		(char const * hlsl_name, sampler_state_descriptor & sampler_desc);
	effect_compiler& def_sampler		(char const * hlsl_name, D3D_TEXTURE_ADDRESS_MODE address = D3D_TEXTURE_ADDRESS_WRAP, D3D_FILTER filter = D3D_FILTER_MIN_MAG_MIP_LINEAR);

	sampler_state_descriptor & begin_sampler	( char const * name); 
	effect_compiler			& end_sampler	(); 

	// Texture functions
	effect_compiler& set_texture		(char const * hlsl_name, char const *	phisical_name, ref_texture* out_texture = 0);
	effect_compiler& set_texture		(char const * hlsl_name, shared_string phisical_name)  { return set_texture( hlsl_name, phisical_name.c_str());}
	effect_compiler& set_texture		(char const * hlsl_name, res_texture * texture);

	
// 	effect_compiler& def_sampler_clf	(char const * name, shared_string texture);
// 	effect_compiler& def_sampler_rtf	(char const * name, shared_string texture);
// 	effect_compiler& def_sampler_clw	(char const * name, shared_string texture);

	// This overwrites global binding.
	effect_compiler& bind_constant		(shader_constant_binding const& binding);

	template <typename T>
	effect_compiler& bind_constant		(shared_string hlsl_name, T const * source)					{ return bind_constant( shader_constant_binding( hlsl_name, source)); }

 	template <typename T>
 	effect_compiler& set_constant		(shared_string hlsl_name, T const & source)					{ return bind_constant( shader_constant_binding( hlsl_name, effect_constant_storage::ref().store_constant(source))); }
	
public:
	effect_compiler						(res_effect& effect, bool shaders_cache_mode = false, binary_shader_sources_type* in_shader_sources = 0);
	~effect_compiler					();
	
	binary_shader_sources_type*			m_shader_sources;
	
	struct shader_cache_info
	{
		fs_new::virtual_path_string			vertex_shader_name;
		fs_new::virtual_path_string			pixel_shader_name;
		fs_new::virtual_path_string			geometry_shader_name;
		shader_configuration	configuration;
	};
	
	typedef xray::vectora<shader_cache_info>	shader_cache_info_vector;
	
	shader_cache_info_vector const get_cached_shaders_info() const { return m_shader_cache_info; }
	
private:
	
	shader_cache_info_vector			m_shader_cache_info;
	
	u32	effect_compiler::get_sampler	(char const * name);

	void set_samp_texture				(u32 samp, char const * name);
	void set_samp_address				(u32 samp, u32	address);
	void set_samp_filter_min			(u32 samp, u32 filter);
	void set_samp_filter_mip			(u32 samp, u32 filter);
	void set_samp_filter_mag			(u32 samp, u32 filter);
	void set_samp_filter				(u32 samp, D3D_FILTER filter);
	
	
private:
	ref_ps_hw							m_ps_hw;
	ref_gs_hw							m_gs_hw;
	ref_vs_hw							m_vs_hw;
	state_descriptor					m_state_descriptor;
	sampler_state_descriptor			m_sampler_state_descriptor;
	vs_descriptor						m_vs_descriptor;
	gs_descriptor						m_gs_descriptor;
	ps_descriptor						m_ps_descriptor;
	pcstr								m_curr_sampler_name;
	shader_constant_bindings			m_bindings;
	res_shader_technique				m_sh_technique;
	res_effect&							m_compilation_target;
	u32									m_technique_idx;
	u32									m_pass_idx;
	bool								m_shaders_cache_mode;
}; // class effect_compiler

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_CORE_EFFECT_COMPILER_H_INCLUDED
