////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SHADER_MANAGER_H_INCLUDED
#define SHADER_MANAGER_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/shader_configuration.h>
#include <xray/render/core/res_effect.h>

namespace xray {
namespace render {

typedef fixed_string<128>	effect_decriptor_name;

//struct custom_config;
typedef xray::intrusive_ptr < custom_config, custom_config, threading::simple_lock > custom_config_ptr;



class effect_descriptor;
class res_pass;
class res_shader_technique;

struct effect_options_descriptor;
class custom_config_value;

template < typename shader_data >
class res_xs_hw;

struct effect_loader
{
	effect_loader	( ref_effect* effect ):effect_ptr(effect), query_rejected(false){}
	ref_effect*		effect_ptr;
	bool			query_rejected;
	void			on_effect_ready	( resources::queries_result& data );
};

class effect_manager: public quasi_singleton<effect_manager>
{
public:
		effect_manager	();
		~effect_manager	();
	
		struct effect_holder_struct
		{
			effect_descriptor*	descriptor;
			custom_config_ptr	config;
			res_effect*			effect;
		};
		
		void					add_effect(effect_descriptor* in_descriptor, 
										   custom_config_ptr const& in_config, 
										   res_effect* in_effect);
		
		void					remove_effect(res_effect* in_effect);
		
		res_pass*				create_pass	( res_pass const& prototype);
		void					delete_pass	( res_pass const* pass);
		
		res_shader_technique*	create_effect_technique	( const res_shader_technique& element);
		void					delete_effect_technique	( res_shader_technique const* element);
		
		// NEW
		template<class effect_descriptor_class> 
		ref_effect				create_effect();
		
		template<class effect_descriptor_class> 
		ref_effect				create_effect(effect_options_descriptor const& desc);

		template<class effect_descriptor_class> 
		void					create_effect(ref_effect* out_effect, effect_options_descriptor const& desc);

		template<class effect_descriptor_class> 
		void					create_effect(ref_effect* out_effect);

		template<class effect_descriptor_class> 
		void					create_effect(effect_loader* loader, effect_options_descriptor const& desc);
		
		ref_effect				create_effect( LPCSTR descriptor_name, render::effect_options_descriptor const& desc);
		ref_effect				create_effect( LPCSTR descriptor_name, xray::configs::binary_config_value const& config, bool force_recompile);
		ref_effect				create_effect( LPCSTR descriptor_name, custom_config_value  const& config, bool force_recompile);
private:
		void					on_effect_created(ref_effect* out_effect_ptr, xray::resources::queries_result& data);
		void					on_async_effect_created(resources::queries_result& data, ref_effect* out_effect_ptr, effect_descriptor* descriptor);
		ref_effect				create_new_effect( effect_descriptor& descriptor, custom_config_ptr const& ptr, u32 crc);
		void					create_new_effect( ref_effect* out_effect, effect_descriptor* descriptor, custom_config_ptr const& config, u32 crc);
		void					create_new_effect( effect_loader* loader, effect_descriptor* descriptor, custom_config_ptr const& config, u32 crc);
public: 
		
		// TODO: Refactor this functions.
		ref_effect				create_effect	( LPCSTR descriptor_name, LPCSTR texture_name = 0);
		ref_effect				create_effect	( effect_descriptor* desc, LPCSTR texture_name = 0);
		
		//res_effect*				create_effect	( effect_descriptor* desc, res_texture** textures, u32 count);
		void					delete_effect	( res_effect const* shader);
		
		void					register_effect_desctiptor				( char const * name, effect_descriptor * dectriptor);
		void					recompile_shaders( render::vector<fs_new::virtual_path_string> const& in_shader_names);
		
private:
	typedef render::map<effect_decriptor_name, effect_descriptor*>				map_effect_descriptors;
	typedef	map_effect_descriptors::iterator									map_effect_descriptors_it;
	typedef render::vector<effect_holder_struct>								effects_vector_type;
	typedef render::map<u32, effect_descriptor*>								map_effect_descriptors_by_id;
	
	struct shader_cache_info
	{
		fs_new::virtual_path_string			vertex_shader_name;
		fs_new::virtual_path_string			pixel_shader_name;
		shader_configuration	configuration;
	};

	typedef xray::vectora<shader_cache_info>	shader_cache_info_vector;
	shader_cache_info_vector					m_shader_cache_info;
		
	struct effect_to_recompile_struct
	{
		effect_to_recompile_struct(ref_effect in_effect, effect_descriptor* in_descriptor, custom_config_ptr in_config, u32 in_crc):
			effect		(in_effect),
			descriptor	(in_descriptor),
			config		(in_config),
			crc			(in_crc)
		{}
		ref_effect			effect;
		effect_descriptor*	descriptor;
		custom_config_ptr	config;
		u32					crc;
	}; // struct effect_to_recompile_struct
	
	typedef xray::vectora<effect_to_recompile_struct>	effects_to_recompile_type;
	
	void on_effects_recompiled					(effects_to_recompile_type* effects_to_recompile, resources::queries_result& data);
public:

	template <typename T>
	static void call_resource_destructor( T* resource) { resource->~T();};
	
	effect_descriptor* get_effect_descriptor_by_name(pcstr name);
	
	void delete_pending_effect(ref_effect* effect);
	
	bool force_sync;
	
private:
	effect_descriptor* find_effect( LPCSTR name, LPCSTR texture /* "texture" for testing only */ );
	
	void load_effects	();
	void load_raw_file	( resources::queries_result& data);
	
	effect_descriptor* make_effect( u64 cls);
	
private:
	render::vector<res_pass*>				m_passes;
	//render::vector<res_effect_technique*>	m_effect_techniques;
	
	render::vector<res_effect*>				m_shaders;
	render::vector<res_shader_technique*>	m_techniques;
	
	effects_vector_type						m_effects;
	
	map_effect_descriptors					m_effect_descriptors;
	map_effect_descriptors					m_effect_descriptors_by_texture;
	
	bool									m_loading_incomplete;
	
	// Temporary
	typedef render::vector<ref_effect*>		effects_deleted_in_pending_type;
	effects_deleted_in_pending_type			m_effects_deleted_in_pending;
	
}; // class effect_manager

} // namespace render 
} // namespace xray 

#include "effect_manager_inline.h"

#endif // #ifndef SHADER_MANAGER_H_INCLUDED