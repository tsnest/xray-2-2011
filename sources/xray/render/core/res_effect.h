////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_EFFECT_H_INCLUDED
#define RES_EFFECT_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/res_state.h>
#include <xray/render/core/res_xs.h>
#include <xray/render/core/res_input_layout.h>
#include <xray/resources_intrusive_base.h>
#include <xray/resources_resource_ptr.h>
#include <xray/render/core/shader_configuration.h>

namespace xray {
namespace render {

template < typename shader_data >
class res_xs;

struct vs_data;
typedef res_xs< vs_data >		res_vs;
typedef xray::intrusive_ptr< res_vs, resource_intrusive_base, threading::single_threading_policy >	ref_vs;

struct gs_data;
typedef res_xs< gs_data >		res_gs;
typedef xray::intrusive_ptr< res_gs, resource_intrusive_base, threading::single_threading_policy >	ref_gs;

struct ps_data;
typedef res_xs< ps_data >		res_ps;
typedef xray::intrusive_ptr< res_ps, resource_intrusive_base, threading::single_threading_policy >	ref_ps;

class res_state;
typedef xray::intrusive_ptr<res_state, resource_intrusive_base, threading::single_threading_policy>	ref_state;

class res_declaration;

class res_input_layout;
typedef xray::intrusive_ptr<res_input_layout, resource_intrusive_base, threading::single_threading_policy>			input_layout_ptr;




//////////////////////////////////////////////////////////////////////////
class res_pass : public resource_intrusive_base {
	friend class effect_manager;
	friend class resource_intrusive_base;

public:
	res_pass( const ref_vs& vs, const ref_gs& gs, const ref_ps& ps, const ref_state& state):
	  m_state(state), 
		  m_vs(vs),
		  m_gs(gs), 
		  m_ps(ps),
		  m_registered(false)
	  {
	  }

	inline void mark_registered() { R_ASSERT( !m_registered ); m_registered = true; }
	inline bool is_registered() const { return m_registered; }
	~res_pass	();
	void destroy_impl	() const;

	void init_layout( res_declaration const & decl);

	bool equal(const res_pass& other) const;
	bool equal(const res_pass* other) const {return equal(*other);}

	void apply() const;

	// Need review...
	inline u64 get_priority() const;

	res_vs*	get_vs() const { return &*m_vs;}
	res_ps*	get_ps() const { return &*m_ps;}
	res_gs*	get_gs() const { return &*m_gs;}
	
private:
	ref_state			m_state;			//state;		// Generic state, like Z-Buffering, samplers, etc
	ref_vs				m_vs;				
	ref_gs				m_gs;			
	ref_ps				m_ps;				

	input_layout_ptr	m_input_layout;	// May be NULL
	bool				m_registered;
};

typedef intrusive_ptr<res_pass, resource_intrusive_base, threading::single_threading_policy>	ref_pass;

//////////////////////////////////////////////////////////////////////////
// shader_technique
class res_shader_technique : public resource_intrusive_base {
public:
	struct sflags
	{
		u32	priority		: 2; //iPriority
		u32	strict_b2f		: 1; //bStrictB2F
		u32	has_emissive	: 1; //bEmissive
		u32	has_distort		: 1; //bDistort
		u32	has_wmark		: 1; //bWmark
	};


	friend class effect_manager;
	res_shader_technique() :
		  m_registered(false)
	  {
	  }
	~res_shader_technique	();
	void destroy_impl() const;

public:

	inline void mark_registered() { R_ASSERT( !m_registered ); m_registered = true; }
	inline bool is_registered() const { return m_registered; }

	void init_layouts( res_declaration const & decl);

	bool equal(const res_shader_technique& other) const;
	bool equal(const res_shader_technique* other) const {return equal(*other);}

	res_pass const* get_pass( u32 i) const { return &*m_passes[i];}

public:
	static const int SHADER_PASSES_MAX = 2;
	typedef fixed_vector<ref_pass,SHADER_PASSES_MAX> passes_array_type;
	
	sflags				m_flags;
	passes_array_type	m_passes;
	bool				m_registered;
};

typedef intrusive_ptr<res_shader_technique, resource_intrusive_base, threading::single_threading_policy>	ref_shader_technique;

//////////////////////////////////////////////////////////////////////////
// effect
class res_effect : public resources::unmanaged_resource
{
	//resource_intrusive_base {
	friend class effect_manager;
	friend class effect_cook;
	friend class effect_compiler;
	friend class resource_intrusive_base;
	friend struct shader_binary_source_cook;

	virtual ~res_effect();
	void destroy_impl() const;

public:
	res_effect() :
#ifndef MASTER_GOLD
			used_shaders(xray::debug::g_mt_allocator),
#endif // #ifndef MASTER_GOLD
			m_registered(false)
	  {
	  }

	void get_max_used_texture_dimension(u32& out_width, u32& out_height);
	u32 get_total_pixel_shader_instruction_count() const;

	inline void mark_registered() { /*R_ASSERT( !m_registered ); m_registered = true;*/ }
	inline bool is_registered() const { return true/*m_registered*/; }
	
	void init_layouts( res_declaration const & decl);

	bool equal(const res_effect& other) const;
	bool equal(const res_effect* other) const {return equal(*other);}
	
	void apply (u32 technique_id = 0, u32 pass_id = 0) {select_technique(technique_id); apply_pass(pass_id);}

	void select_technique(u32 technique_id) {ASSERT(technique_id<m_techniques.size()); m_cur_technique = technique_id;}
	void apply_pass(u32 pass_id);

	res_shader_technique const * get_technique( u32 i) const		{ return &*m_techniques[i];}

private:
	enum {
		SHADER_TECHNIQUES_MAX = 12,
	};

	typedef fixed_vector<ref_shader_technique, SHADER_TECHNIQUES_MAX> techniques_array_type;

	u32						m_cur_technique;
	techniques_array_type	m_techniques;
	bool					m_registered;
	
#ifndef MASTER_GOLD
	xray::vectora<fs_new::virtual_path_string>	used_shaders;
	threading::simple_lock						used_shaders_lock;
#endif // #ifndef MASTER_GOLD
	
	//R1-0=norm_lod0(det), 1=norm_lod1(normal), 2=L_point, 3=L_spot, 4=L_for_models,	
	//R2-0=deffer, 1=norm_lod1(normal), 2=psm, 3=ssm, 4=dsm
};

//typedef intrusive_ptr<res_effect, resource_intrusive_base, threading::single_threading_policy> ref_effect;

struct custom_config;
typedef xray::intrusive_ptr < custom_config, custom_config, threading::simple_lock > custom_config_ptr;

class effect_descriptor;

struct effect_compile_data
{
	effect_compile_data(effect_descriptor* in_descriptor, custom_config_ptr in_config, u32 in_crc, bool in_add_to_array = true):
		descriptor(in_descriptor),
		config(in_config),
		crc(in_crc),
		add_to_array(in_add_to_array)
	{}
	effect_descriptor*	descriptor;
	custom_config_ptr	config;
	u32					crc;
	bool				add_to_array;
}; // struct effect_compile_data

typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

enum enum_shader_type;

struct binary_shader_cook_data
{
	binary_shader_cook_data(shader_configuration			in_configuration,
							fs_new::virtual_path_string		in_shader_name,
							enum_shader_type				in_shader_type,
							bool							in_is_need_check_time,
							res_effect*						in_effect_resource):
		configuration(in_configuration),
		shader_name(in_shader_name),
		shader_type(in_shader_type),
		is_need_check_time(in_is_need_check_time),
		effect_resource(in_effect_resource)
	{}
	res_effect*				effect_resource;
	shader_configuration	configuration;
	fs_new::virtual_path_string			shader_name;
	enum_shader_type		shader_type;
	bool					is_need_check_time;
}; // struct binary_shader_cook_data

struct binary_shader_source: public resources::unmanaged_resource
{
	binary_shader_source():
		shader_byte_code(0),
		error_code(0)
	{}
	resources::managed_resource_ptr shader_source;
	ID3D10Blob*						shader_byte_code;
	ID3D10Blob*						error_code;
	shader_configuration			configuration;
	fs_new::virtual_path_string		shader_name;
	enum_shader_type				shader_type;
}; // struct binary_shader_source

typedef	xray::resources::resource_ptr < binary_shader_source, xray::resources::unmanaged_intrusive_base > binary_shader_source_ptr;




	
struct binary_shader_key_type
{
	binary_shader_key_type(pcstr in_shader_name, enum_shader_type in_type, shader_configuration in_configuration):
		shader_name(in_shader_name), type(in_type), configuration(in_configuration)
	{}
	friend bool operator < (binary_shader_key_type const& left, binary_shader_key_type const& right)
	{
		return	left.shader_name < right.shader_name ||
				(left.shader_name == right.shader_name && 
					(left.configuration.configuration < right.configuration.configuration || 
						(left.configuration.configuration == right.configuration.configuration && left.type < right.type)));
	}
	friend bool operator == (binary_shader_key_type const& left, binary_shader_key_type const& right)
	{
		return	left.shader_name == right.shader_name && 
				left.configuration.configuration == right.configuration.configuration &&
				left.type == right.type;
	}
	
	shader_configuration		configuration;
	fs_new::virtual_path_string	shader_name;
	enum_shader_type			type;
};

typedef associative_vector<binary_shader_key_type, binary_shader_source_ptr, render::vector> binary_shader_sources_type;

//#define		SE_R2_NORMAL_HQ		0	// high quality/detail
//#define		SE_R2_NORMAL_LQ		1	// low quality
//#define		SE_R2_SHADOW		2	// shadow generation
//	E[3] - can use for night vision but need to extend SE_R1. Will need 
//	Extra shader element.
//	E[4] - distortion or self illumination(self emission). 
//	E[4] Can use for lightmap capturing.

//#pragma pack(pop)


//u64 res_pass::get_priority() const
//{
//#ifdef DEBUG
//	const int vs_id_bit_width = 7;
//#endif // #ifdef DEBUG
//	const int ps_id_bit_width = 7;
//	const int state_id_bit_width = 7;
////	const int tex_list_id_bit_width = 12;
//
//	u64 qword = 0;
//	u64 vs_id = m_vs->get_id();
//	ASSERT(vs_id < (1<<vs_id_bit_width));
//	qword = vs_id;
//
//	qword <<= ps_id_bit_width;
//	u64 ps_id = m_ps->get_id();
//	ASSERT(ps_id < (1<<ps_id_bit_width));
//	qword |= ps_id;
//
//	qword <<= state_id_bit_width;
//	u64 state_id = m_state->get_id();
//	ASSERT(state_id < (1<<state_id_bit_width));
//	qword |= state_id;
//
//// 	qword <<= tex_list_id_bit_width;
//// 	u64 tlist_id = m_tex_list->get_id();
//// 	ASSERT(tlist_id < (1<<tex_list_id_bit_width));
//// 	qword |= tlist_id;
//
//	return qword;
//}


} // namespace render 
} // namespace xray 


#endif // #ifndef RES_EFFECT_H_INCLUDED
