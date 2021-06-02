////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/res_effect.h>
#include "manager_common_inline.h"
#include <xray/render/core/custom_config.h>
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/effect_descriptor.h>
#include <xray/tasks_system.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/effect_cook.h>

namespace xray {
namespace render {

void effect_loader::on_effect_ready( resources::queries_result& data )
{
	if(!query_rejected)
	{
		if (data[0].is_successful())
			*effect_ptr		= static_cast_resource_ptr<ref_effect>(data[0].get_unmanaged_resource());
		else
			*effect_ptr	= 0;
	}else
	{
		//LOG_INFO("query rejected");
	}

	effect_loader* this_ptr	= this;
	DELETE					( this_ptr );
}

////////////////////////////////////////////////////////////////////////
struct effect_manager_call_destructor_predicate 
{

	template <typename T>
	inline void	operator()	( T* resource ) const
	{
		effect_manager::call_resource_destructor( resource );
	}
}; // struct effect_manager_call_destructor_predicate
////////////////////////////////////////////////////////////////////////



effect_manager::effect_manager(): 
	m_loading_incomplete(false),
	force_sync(false),
#ifdef MASTER_GOLD
	m_shader_cache_info(memory::g_mt_allocator)
#else // #ifdef MASTER_GOLD
	m_shader_cache_info(::xray::debug::g_mt_allocator)
#endif // #ifdef MASTER_GOLD
{
	static effect_cook effect_cooker;
	register_cook(&effect_cooker);
}

effect_manager::~effect_manager()
{
	map_effect_descriptors_it it  = m_effect_descriptors.begin(),
							  end = m_effect_descriptors.end();

	for( ; it != end; ++it)
		DELETE( it->second);

		it  = m_effect_descriptors_by_texture.begin();
		end = m_effect_descriptors_by_texture.end();

	for( ; it != end; ++it)
		DELETE( it->second);
	
	//unregister_cook(resources::render_effect_class);
}

res_pass* effect_manager::create_pass( const res_pass& pass)
{
	for( u32 it=0; it<m_passes.size(); it++)
		if( m_passes[it]->equal( pass))
			return m_passes[it];

	res_pass*	new_pass = NEW( res_pass)( pass.m_vs, pass.m_gs, pass.m_ps, pass.m_state);
	new_pass->mark_registered();

	m_passes.push_back			( new_pass);
	return m_passes.back();
}

void effect_manager::delete_pass( res_pass const* pass)
{
	if( !pass->is_registered() )
		return;

	if( reclaim( m_passes, pass))
	{
		DELETE( pass, effect_manager_call_destructor_predicate());
		return;
	}

	LOG_ERROR( "!ERROR: Failed to find compiled pass.");
}

effect_descriptor* effect_manager::get_effect_descriptor_by_name(pcstr name)
{
	map_effect_descriptors_it it = m_effect_descriptors.find(name);
	
	if( it != m_effect_descriptors.end())
		return it->second;
	
	if (!strings::equal(name, "none"))
		R_ASSERT(0, "%s disctiptor not found!", name);
	
	return 0;
}

effect_descriptor* effect_manager::find_effect( LPCSTR name, LPCSTR texture /* "texture" for testing only */ )
{
	// First find by texture name. This is temporary; for testing only.
	map_effect_descriptors_it	it = m_effect_descriptors_by_texture.find( texture);

	if( it != m_effect_descriptors_by_texture.end())
		return it->second;
	
	it = m_effect_descriptors.find( name);
	
	if( it != m_effect_descriptors.end())
		return it->second;
	
	return 0;
}

void effect_manager::add_effect(effect_descriptor* in_descriptor, 
								custom_config_ptr const& in_config, 
								res_effect* in_effect)
{
	effect_holder_struct	holder;
	holder.effect			= in_effect;
	holder.config			= in_config;
	holder.descriptor		= in_descriptor;
	
	m_effects.push_back(holder);
}

void effect_manager::remove_effect(res_effect* in_effect)
{
	effects_vector_type::iterator begin_it	= m_effects.begin();
	effects_vector_type::iterator end_it	= m_effects.end();
	effects_vector_type::iterator it		= begin_it;
	
	for (; it != end_it; ++it)
	{
		if (it->effect == in_effect)
		{
			m_effects.erase(it);
		}
	}
}

ref_effect effect_manager::create_effect( LPCSTR descriptor_name, xray::configs::binary_config_value const& in_binary_config, bool force_recompile)
{
	XRAY_UNREFERENCED_PARAMETER(force_recompile);

	if( effect_descriptor* b = find_effect( descriptor_name, ""))
	{
		u32 crc = 0;
		custom_config_ptr config	= render::create_custom_config(in_binary_config, crc, true);
		ref_effect new_effect		= create_new_effect(*b, config, crc);
		return new_effect;
	}
	ASSERT("effect desctriptor not found!");
	return 0;
}

ref_effect effect_manager::create_effect( LPCSTR descriptor_name, custom_config_value const& in_binary_config, bool force_recompile)
{
	XRAY_UNREFERENCED_PARAMETER(force_recompile);

	if( effect_descriptor* b = find_effect( descriptor_name, ""))
	{
		u32 crc = 0;
		custom_config_ptr config	= render::create_custom_config(in_binary_config, crc, true);
		ref_effect new_effect		= create_new_effect(*b, config, crc);
		return new_effect;
	}
	ASSERT("effect desctriptor not found!");
	return 0;
}

ref_effect effect_manager::create_effect( LPCSTR descriptor_name, render::effect_options_descriptor const& desc)
{
	if( effect_descriptor* b = find_effect( descriptor_name, ""))
	{
		u32 crc = desc.get_crc();
		
		custom_config_ptr config	= render::create_custom_config(desc, crc, true);
		ref_effect new_effect		= create_new_effect(*b, config, crc);
		return new_effect;
	}
	ASSERT("effect desctriptor not found!");
	return 0;
}

void effect_manager::on_effect_created(ref_effect* out_effect_ptr, resources::queries_result& data)
{
	if (data[0].is_successful())
	{
		*out_effect_ptr		= static_cast_resource_ptr<ref_effect>(data[0].get_unmanaged_resource());
	}
	else
		*out_effect_ptr		= 0;
}

void effect_manager::on_async_effect_created(resources::queries_result& data, 
											 ref_effect* out_effect_ptr,
											 effect_descriptor* descriptor)
{
	(void)&descriptor;
	
//	if (std::find(m_effects_deleted_in_pending.begin(), m_effects_deleted_in_pending.end(), out_effect_ptr) != m_effects_deleted_in_pending.end())
//		return;
	
	if (data[0].is_successful())
		*out_effect_ptr		= static_cast_resource_ptr<ref_effect>(data[0].get_unmanaged_resource());
	else
		*out_effect_ptr		= 0;
}

void effect_manager::create_new_effect(effect_loader* loader, 
									   effect_descriptor* descriptor, 
									   custom_config_ptr const& config, 
									   u32 crc)
{
	resources::user_data_variant				user_data_variant;
	
	effect_compile_data* cook_data				= NEW(effect_compile_data)(descriptor, config, crc);
	user_data_variant.set						(cook_data);
	
	resources::query_create_resource			(
		"",
		xray::const_buffer("", 1),
		resources::render_effect_class,
		boost::bind(&effect_loader::on_effect_ready, loader, _1 ),
		g_allocator,
		&user_data_variant
	);
}

void effect_manager::create_new_effect(ref_effect* out_effect, 
									   effect_descriptor* descriptor, 
									   custom_config_ptr const& config, 
									   u32 crc)
{
	resources::user_data_variant				user_data_variant;
	
	effect_compile_data* cook_data				= NEW(effect_compile_data)(descriptor, config, crc);
	user_data_variant.set						(cook_data);
	
	resources::query_create_resource			(
		"",
		xray::const_buffer("", 1),
		resources::render_effect_class,
		boost::bind(&effect_manager::on_async_effect_created, this, _1, out_effect, descriptor),
		g_allocator,
		&user_data_variant
	);
}

ref_effect effect_manager::create_new_effect( effect_descriptor& descriptor, 
											 render::custom_config_ptr const& ptr, 
											 u32 crc)
{
	resources::user_data_variant				user_data_variant;
	
	effect_compile_data* cook_data				= NEW(effect_compile_data)(&descriptor, ptr, crc);
	user_data_variant.set						(cook_data);
	
	resources::user_data_variant const*			user_data_variants[] = {&user_data_variant};
	
	resources::creation_request requests[]		= { resources::creation_request("", xray::const_buffer("", 1), resources::render_effect_class) };
	
	ref_effect new_effect_ptr;
	
	resources::query_create_resources_and_wait	(
		requests, 
		array_size(requests), 
		boost::bind(&effect_manager::on_effect_created, this, &new_effect_ptr, _1),
		g_allocator,
		user_data_variants
	);
	
	return new_effect_ptr;
}

void effect_manager::on_effects_recompiled(effects_to_recompile_type* effects_to_recompile, resources::queries_result& data)
{
	u32 request_index						= 0;
	for (effects_to_recompile_type::iterator it = effects_to_recompile->begin(); it != effects_to_recompile->end(); ++it, ++request_index)
	{
		if (data[request_index].is_successful())
		{
			
			effects_to_recompile;
			
			ref_effect created_temp_effect	= static_cast_resource_ptr<ref_effect>(data[request_index].get_unmanaged_resource());
			// TODO: add some special copy function
			it->effect->m_techniques		= created_temp_effect->m_techniques;
			it->effect->m_cur_technique		= created_temp_effect->m_cur_technique;
#ifndef MASTER_GOLD
			threading::simple_lock_raii raii(it->effect->used_shaders_lock);
			it->effect->used_shaders		= created_temp_effect->used_shaders;
#endif // #ifndef MASTER_GOLD
		}
	}
}

ref_effect	effect_manager::create_effect( LPCSTR shader, LPCSTR texture_name)
{
	if( effect_descriptor* b = find_effect( shader, texture_name))
	{
		return create_effect( b, texture_name );
	}
	ASSERT("effect desctriptor not found!");
	return 0;
}
 
ref_effect	effect_manager::create_effect( effect_descriptor* desc, LPCSTR texture_name)
{
	byte temp_buffer[Kb];
	effect_options_descriptor options_desc(temp_buffer, sizeof(temp_buffer));
	
	if (texture_name)
		options_desc["texture0"] = texture_name;
	
	u32 crc = 0;
	return create_new_effect(*desc, create_custom_config(options_desc, crc, true), crc);
}

void effect_manager::delete_effect( res_effect const* effect)
{
	if( !effect->is_registered() )
		return;
	
	for (effects_vector_type::iterator it=m_effects.begin(); it!=m_effects.end(); ++it)
	{
		//if (it->second!=effect)
		if (it->effect!=effect)
			continue;
		
		m_effects.erase(it);
		
		DELETE( effect, effect_manager_call_destructor_predicate());
		
		return;
	}
	
	// TODO: remove m_shaders, remove shader from m_effects
	if( reclaim( m_shaders, effect))
	{
		DELETE( effect, effect_manager_call_destructor_predicate());
		return;
	}
	
	LOG_ERROR( "!ERROR: Failed to find complete shader");
}

res_shader_technique* effect_manager::create_effect_technique( const res_shader_technique& element)
{
	if( element.m_passes.empty())
		return 0;

	// Search equal in shaders array
	for( u32 it = 0; it < m_techniques.size(); ++it)
		if( element.equal( m_techniques[it]))
			return m_techniques[it];

	// Create _new_ entry
	res_shader_technique*	new_technique = NEW( res_shader_technique); //( element);

	new_technique->m_flags = element.m_flags;
	new_technique->m_passes = element.m_passes;

	new_technique->mark_registered();
	m_techniques.push_back( new_technique);
	return new_technique;
}

void effect_manager::delete_effect_technique( res_shader_technique const* technique)
{
	if( !technique->is_registered() )
		return;

	if( reclaim( m_techniques, technique))
	{
		DELETE( technique, effect_manager_call_destructor_predicate());
		return;
	}

	//LOG_ERROR( "!ERROR: Failed to find complete shader");
}

void effect_manager::recompile_shaders( render::vector<fs_new::virtual_path_string> const& in_shader_names)
{
#ifndef MASTER_GOLD
	effects_to_recompile_type					effects_to_recompile(xray::debug::g_mt_allocator);
	
	for (effects_vector_type::iterator it = m_effects.begin(); it != m_effects.end(); ++it)
	{
		threading::simple_lock_raii	raii(it->effect->used_shaders_lock);
		for (render::vector<fs_new::virtual_path_string>::const_iterator name_it = in_shader_names.begin(); name_it != in_shader_names.end(); ++name_it)
		{
			//for(xray::vectora<fs_new::virtual_path_string>::iterator	used_it = it->second->used_shaders.begin(); used_it != it->second->used_shaders.end(); ++used_it)
			//LOG_INFO(used_it->c_str());
			xray::vectora<fs_new::virtual_path_string>::iterator found = std::find(it->effect->used_shaders.begin(), it->effect->used_shaders.end(), *name_it);
			if (found != it->effect->used_shaders.end())
				effects_to_recompile.push_back( effect_to_recompile_struct( it->effect, it->descriptor, it->config, 0 ) );
		}
	}

	if (!effects_to_recompile.size())
		return;
	
	u32 const						num_requests			= effects_to_recompile.size();
	resources::user_data_variant**	user_data_variants_ptr	= (resources::user_data_variant**)ALLOCA(sizeof(resources::user_data_variant*) * num_requests);
	resources::user_data_variant*	user_data_variants		= (resources::user_data_variant*)ALLOCA(sizeof(resources::user_data_variant) * num_requests);
	resources::creation_request*	requests				= (resources::creation_request*)ALLOCA(sizeof(resources::creation_request) * num_requests);
	
	u32 request_index = 0;
	for (effects_to_recompile_type::iterator it = effects_to_recompile.begin(); it != effects_to_recompile.end(); ++it)
	{
		resources::user_data_variant* variant	= new(&user_data_variants[request_index])resources::user_data_variant;
		variant->set							(NEW(effect_compile_data)(it->descriptor, it->config, it->crc, false));
		user_data_variants_ptr[request_index]	= variant;
		
		new(&requests[request_index])			resources::creation_request(
			"", 
			xray::const_buffer("", 1), 
			resources::render_effect_class
		);
		request_index++;
	}
	LOG_INFO									("Shaders reloading started.");
	timing::timer								timer;
	timer.start									();
	// Must be query_create_resources_and_wait!
	resources::query_create_resources_and_wait	(
		requests, 
		num_requests, 
		boost::bind(&effect_manager::on_effects_recompiled, this, &effects_to_recompile, _1),
		&xray::debug::g_mt_allocator,
		(resources::user_data_variant const**)user_data_variants_ptr
	);
	LOG_INFO									("Shaders reloading finished (%.3f seconds)", timer.get_elapsed_sec());
//	editor_message_out("Shaders reloading finished.");
#endif // #ifndef MASTER_GOLD
}

void effect_manager::register_effect_desctiptor( char const * name, effect_descriptor * dectriptor)
{
 	m_effect_descriptors.insert( utils::mk_pair(name, dectriptor));
}

void effect_manager::delete_pending_effect(ref_effect* effect)
{
	if (std::find(m_effects_deleted_in_pending.begin(), m_effects_deleted_in_pending.end(), effect) == m_effects_deleted_in_pending.end())
		m_effects_deleted_in_pending.push_back(effect);
}

} // namespace render 
} // namespace xray 
