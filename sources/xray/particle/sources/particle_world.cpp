////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_world.h"
#include "particle_system_instance_cook.h"
#include "particle_system_cook.h"

#include "help_functions.h"
#include "particle_entity_types.h"
#include "particle_actions.h"
#include "particle_system.h"

#include "particle_system_instance.h"
#include "particle_emitter.h"
#include "particle_entity_types.h"

#ifndef MASTER_GOLD
#	include "preview_particle_system.h"
#	include "preview_particle_system_instance.h"
#	include "preview_particle_system_cooker.h"
#endif // #ifndef MASTER_GOLD

#include <xray/particle/base_particle.h>
#include "particle_beam_emitter_instance.h"
#include <xray/resources_unmanaged_allocator.h>
#include <xray/resources.h>
#include <xray/console_command.h>
#include <xray/console_command_processor.h>
#include <xray/particle/engine.h>
#include "particle_actions.h"

static u32 s_particle_simulation_speed_value = 100; // in percents
static xray::console_commands::cc_u32 s_particle_simulation_speed("particle_simulation_speed", 
															s_particle_simulation_speed_value, 
															0, 
															100, 
															true, 
															xray::console_commands::command_type_engine_internal);


namespace xray {
namespace particle {


#ifndef MASTER_GOLD
static xray::uninitialized_reference<preview_particle_system_cooker>	s_preview_particle_system_cook_object;
#endif // #ifndef MASTER_GOLD

void register_preview_particle_system_cooker()
{
#ifndef MASTER_GOLD
	XRAY_CONSTRUCT_REFERENCE	( s_preview_particle_system_cook_object, preview_particle_system_cooker ) ( );
	resources::register_cook	( s_preview_particle_system_cook_object.c_ptr() );
#endif // #ifndef MASTER_GOLD
}

void unregister_preview_particle_system_cooker()
{
#ifndef MASTER_GOLD
	resources::unregister_cook	( resources::preview_particle_system_class );
	XRAY_DESTROY_REFERENCE		( s_preview_particle_system_cook_object );
#endif // #ifndef MASTER_GOLD
}


particle_world::particle_world	( particle::engine& engine ) :
	m_num_particles ( 0 ),
	m_max_particles ( 50000 ),	// sizeof(base_particle) == 204 bytes ..., (204 * 50000) == 9.7 Mb
	m_engine		( engine )
{
}

particle_world::~particle_world	( )
{
 	particle_system_instance_impl* instance = m_ticked_instances_list.front();
 	while(instance)
 	{
 		particle_system_instance_impl* next = m_ticked_instances_list.get_next_of_object(instance);
		
 		remove_particle_system_instance(instance);
		
 		instance = next;
 	}
}

base_particle* particle_world::allocate_particle( )
{
	if ( m_num_particles + 1 > m_max_particles )
	{
		return 0;
	}
	
	base_particle* particle		= XRAY_NEW_IMPL( m_allocator, base_particle )();
	CURE_ASSERT					( particle, return 0 );
	particle->set_defaults		( );
	++m_num_particles;
	
	return particle;
}

void particle_world::deallocate_particle( base_particle* P )
{
	--m_num_particles;
	XRAY_DELETE_IMPL( m_allocator, P );
}

void particle_world::play(particle_system_instance_ptr instance, xray::math::float4x4 const& transform, bool use_transform, bool always_loop)
{
	particle_system_instance_impl* impl = static_cast_checked<particle_system_instance_impl*>(instance.c_ptr());
	
	if (use_transform)
		impl->play_impl(transform);
	else
		impl->play_impl();
	
	impl->m_always_looping = always_loop;
	
	add_particle_system_instance(impl);
}

void particle_world::stop(particle_system_instance_ptr instance, float time)
{
	particle_system_instance_impl* impl = static_cast_checked<particle_system_instance_impl*>(instance.c_ptr());
	impl->stop_impl(time);
}

void particle_world::set_transform_to_particle_instance(particle_system_instance* instance, xray::math::float4x4 const& transform)
{
	particle_system_instance_impl* impl = static_cast_checked<particle_system_instance_impl*>(instance);
	impl->set_transform(transform);
}


void particle_world::set_max_particles(u32 num_max_particles)
{
	m_max_particles  = num_max_particles;
}

void particle_world::check_lods(xray::math::float3 const& view_location)
{
 	particle_system_instance_impl* instance = m_ticked_instances_list.front();
 	while(instance)
 	{
 		instance->check_lods(view_location);
 		instance = m_ticked_instances_list.get_next_of_object(instance);
 	}
}

u32 particle_world::calc_num_new_particles( float time_delta )
{
	u32 total_summ = 0;
	
 	particle_system_instance_impl* instance = m_ticked_instances_list.front();
 	while(instance)
 	{
 		total_summ += instance->calc_num_new_particles(time_delta);
 		instance = m_ticked_instances_list.get_next_of_object(instance);
 	}
	return total_summ;
}

u32 particle_world::calc_num_max_particles(float time_delta)
{
	u32 total_summ = 0;

	particle_system_instance_impl* instance = m_ticked_instances_list.front();
	while(instance)
	{
		total_summ += instance->calc_num_max_particles(time_delta);
		instance = m_ticked_instances_list.get_next_of_object(instance);
	}
	return total_summ;
}

u32 particle_world::remove_dead_particles()
{
	u32 num_removed = 0;

	particle_system_instance_impl* instance = m_ticked_instances_list.front();
	while(instance)
	{
		num_removed += instance->remove_dead_particles();
		instance = m_ticked_instances_list.get_next_of_object(instance);
	}
	return num_removed;
}

void particle_world::remove_overflowing_particles()
{
	particle_system_instance_impl* instance = m_ticked_instances_list.front();
	while(instance)
	{
		instance->remove_overflowing_particles();
		instance = m_ticked_instances_list.get_next_of_object(instance);
	}
}


void particle_world::shrink_particles( float time_delta, float limit_over_total, u32 num_need_particles)
{
	particle_system_instance_impl* instance = m_ticked_instances_list.front();
	while(instance)
	{
		instance->shrink_particles(time_delta, limit_over_total, num_need_particles);
		instance = m_ticked_instances_list.get_next_of_object(instance);
	}
}

void particle_world::tick( float time_delta, xray::math::float4x4 const& view_matrix )
{
	time_delta *= s_particle_simulation_speed_value / 100.0f;
	
	float4x4 inv_view_matrix;
	inv_view_matrix.try_invert(view_matrix);
	
	check_lods( inv_view_matrix.lines[3].xyz() );
	
	u32 num_need_new	= calc_num_new_particles(time_delta);
	
	u32	num_particles	= calc_num_max_particles(time_delta);
	
	float total			= float(num_particles + num_need_new);
	
	float k = float(m_max_particles) / total;
	xray::math::clamp( k, 0.001f, 1.0f);
	
	shrink_particles( time_delta, k, num_need_new );
	
	remove_overflowing_particles();
	
	particle_system_instance_impl* instance = m_ticked_instances_list.front();
	while(instance)
	{
		bool is_finished = false;
		
		if (instance->get_ticked())
		{
			is_finished = instance->tick( time_delta );
		}
		
		particle_system_instance_impl* next = m_ticked_instances_list.get_next_of_object(instance);
		if (is_finished && !instance->m_child_played)
		{
			remove(instance);
		}
		
		instance = next;
	}
	ASSERT(m_num_particles<=m_max_particles);
}

void particle_world::add_particle_system_instance(particle_system_instance* instance)
{
	//ps_instance_entry* new_entry = MT_NEW(ps_instance_entry)();
	//m_ticked_instances_list2.push_back( new_entry );
	particle_system_instance_impl* impl = static_cast_checked<particle_system_instance_impl*>( instance );
	impl->self_ptr = impl;
	m_ticked_instances_list.push_back( impl );
}

void particle_world::remove_particle_system_instance(particle_system_instance* in_instance)
{
	particle_system_instance_impl* inst = m_ticked_instances_list.front();
	
	bool found = false;
	
	while(inst)
	{
		if (inst==in_instance)
		{
			found = true;
			break;
		}
		inst = m_ticked_instances_list.get_next_of_object(inst);
	}
	
	if (!found)
		return;
	
	inst->remove_particles();
	
	m_ticked_instances_list.erase( static_cast_checked<particle_system_instance_impl*>(in_instance) );
	inst->self_ptr = 0;
}

void particle_world::get_render_emitter_instances( particle::particle_system_instance_ptr in_instance, render_particle_emitter_instances_type& out_particle_emitter_instances )
{
	// TODO: duplicated code!
	
	particle_system_instance_impl* instance =  static_cast_checked<particle_system_instance_impl*>(in_instance.c_ptr());
	
	if (!instance->get_ticked() || !instance->get_visible())
	{
		instance = m_ticked_instances_list.get_next_of_object(instance);
		return;
	}
	
	emitter_instance_list_type& emm_list = instance->m_lods[instance->m_current_lod].m_emitter_instance_list;
	
	particle_emitter_instance* em_instance = emm_list.front();
	while(em_instance)
	{
		if (!em_instance->get_visible() || 
			!em_instance->get_data_type_action())
		{
			em_instance = emm_list.get_next_of_object(em_instance);
			continue;
		}
		out_particle_emitter_instances.push_back( &em_instance->render_instance() );
		em_instance = emm_list.get_next_of_object(em_instance);
	}
	if (instance->m_old_lod!=instance->m_current_lod)
	{
		em_instance = instance->m_lods[instance->m_old_lod].m_emitter_instance_list.front();
		while(em_instance)
		{
			if (!em_instance->get_visible() || !em_instance->get_data_type_action())
			{
				em_instance = emm_list.get_next_of_object(em_instance);
				continue;
			}
			out_particle_emitter_instances.push_back( &em_instance->render_instance() );
			em_instance = emm_list.get_next_of_object(em_instance);
		}
	}
}

void particle_world::get_render_emitter_instances( float4x4 const& view_proj_matrix, render_particle_emitter_instances_type& out_render_emitter_instances)
{
	math::frustum view_frustum(view_proj_matrix);
	
	particle_system_instance_impl* instance = m_ticked_instances_list.front();
	
	while(instance)
	{
		if (!instance->get_ticked() || !instance->get_visible())
		{
			instance = m_ticked_instances_list.get_next_of_object(instance);
			continue;
		}
		
		emitter_instance_list_type& emm_list = instance->m_lods[instance->m_current_lod].m_emitter_instance_list;
		
		particle_emitter_instance* em_instance = emm_list.front();
		while(em_instance)
		{
			if (!em_instance->get_visible() || 
				!em_instance->get_data_type_action() || 
				view_frustum.test_inexact(em_instance->m_aabbox)==math::intersection_outside)
			{
				em_instance = emm_list.get_next_of_object(em_instance);
				continue;
			}
			out_render_emitter_instances.push_back( &em_instance->render_instance() );
			em_instance = emm_list.get_next_of_object(em_instance);
		}
		if (instance->m_old_lod!=instance->m_current_lod)
		{
			em_instance = instance->m_lods[instance->m_old_lod].m_emitter_instance_list.front();
			while(em_instance)
			{
				if (!em_instance->get_visible() || !em_instance->get_data_type_action())
				{
					em_instance = emm_list.get_next_of_object(em_instance);
					continue;
				}
				out_render_emitter_instances.push_back( &em_instance->render_instance() );
				em_instance = emm_list.get_next_of_object(em_instance);
			}
		}
		instance = m_ticked_instances_list.get_next_of_object(instance);
	}
}

particle_emitter_instance* particle_world::create_emitter_instance	( particle_emitter& emitter, bool is_child_emitter_instance )
{
	particle_action_data_type* action	= emitter.get_data_type_action();
	if ( action && (action->get_data_type() == particle_data_type_beam) )
		return							MT_NEW ( particle_beam_emitter_instance ) ( *this, emitter, m_engine, is_child_emitter_instance );

	return								MT_NEW ( particle_emitter_instance ) ( *this, emitter, m_engine, is_child_emitter_instance );
}

void particle_world::change_material	( particle_emitter_instance& instance, resources::unmanaged_resource_ptr const& material )
{
	instance.change_material			( material );
}

// particle_system_instance_ptr particle_world::play			( particle_system_ptr in_particle_system, math::float4x4 const& transform )
// {
// 	particle_system_instance_ptr out_ptr = ((xray::particle::particle_system*)in_particle_system.c_ptr())->play(*this, transform);
// 	
// 	xray::particle::particle_system_instance_impl* inst = (xray::particle::particle_system_instance_impl*)out_ptr.c_ptr();
// 	inst->m_always_looping = true;/*always_looping*/;
// 	
// 	return out_ptr;
// }
// 
// void particle_world::stop									( particle_system_ptr particle_system )
// {
// 	//((particle_system_instance_impl*)in_instance.c_ptr())->stop_impl( 0.0f );
// }

void particle_world::remove									( particle_system_instance_ptr in_particle_system_instance )
{
	remove_particle_system_instance((particle_system_instance*)in_particle_system_instance.c_ptr());
}

void particle_world::set_transform							( particle_system_instance_ptr particle_system_instance, math::float4x4 const& transform )
{
	particle_system_instance_impl* instance =  static_cast_checked<particle_system_instance_impl*>(particle_system_instance.c_ptr());
	instance->set_transform					(transform);
}

void particle_world::set_visible							( particle_system_instance_ptr particle_system_instance, bool is_visible )
{
	particle_system_instance_impl* instance =  static_cast_checked<particle_system_instance_impl*>(particle_system_instance.c_ptr());
	instance->set_visible					(is_visible);
}

void particle_world::set_paused								( particle_system_instance_ptr particle_system_instance, bool is_paused )
{
	particle_system_instance_impl* instance =  static_cast_checked<particle_system_instance_impl*>(particle_system_instance.c_ptr());
	instance->set_pause						(is_paused);
}


#ifndef MASTER_GOLD

particle_system* particle_world::add						( configs::lua_config_value const& init_values, math::float4x4 const& transform )
{
	preview_particle_system* new_ps = MT_NEW(preview_particle_system)(*this);
	
	new_ps->set_creation_source(
		resource_base::creation_source_created_by_user, 
		"",
		xray::resources::memory_usage_type(xray::resources::nocache_memory, sizeof(preview_particle_system))
		);
	
	new_ps->set_deleter_object(&*s_preview_particle_system_cook_object, threading::current_thread_id());
	
	new_ps->m_num_lods = 1;
	new_ps->m_lods = MT_ALLOC(particle_system_lod, new_ps->m_num_lods);
	
	new_ps->m_lods[0].m_emitters_array = 0;
	new_ps->m_lods[0].m_num_emitters = 0;
	new_ps->m_lods[0].m_parent = new_ps;
	new_ps->m_lods[0].m_total_length = 1.0f;
	new_ps->m_lods[0].m_visibility = true;
	
	new_ps->load_lod_from_config(new_ps->m_lods[0], init_values["initialize_config"], true);
	
	new_ps->play_preview(transform, true);
	//interlocked_exchange_pointer(reinterpret_cast<atomic_ptr_type&>(*ps_out_ptr), static_cast<atomic_ptr_value_type>(new_ps));
	return (particle_system*)new_ps;
}

void particle_world::remove									( particle_system** in_particle_system )
{
	preview_particle_system* preview_ps = *(preview_particle_system**)in_particle_system;
	preview_ps->stop_preview();
	
	// TODO!
	//scene->particle_world()->remove_all_child_played_particle_system_instances_if_ticked();
	
	//if (*preview_ps)
	//	MT_DELETE(*preview_ps);
}

void particle_world::update_preview							( particle_system** in_particle_system, xray::configs::lua_config_value config_value )
{
	preview_particle_system* preview_ps = (preview_particle_system*)*in_particle_system;
	preview_ps->update_from_config		(config_value);
}

void particle_world::set_transform							( particle_system** in_particle_system, math::float4x4 const& transform )
{
	preview_particle_system* preview_ps = (preview_particle_system*)*in_particle_system;
	preview_ps->get_preview_instance()->set_transform(transform);
}

void particle_world::set_looped								( particle_system** in_particle_system, bool is_looped )
{
	preview_particle_system* preview_ps = (preview_particle_system*)*in_particle_system;
	preview_ps->get_preview_instance()->m_always_looping = is_looped;
}

void particle_world::restart								( particle_system** in_particle_system )
{
	preview_particle_system* preview_ps = *(preview_particle_system**)in_particle_system;
	preview_ps->get_preview_instance()->updated_from_config(preview_ps->m_lods, preview_ps->m_num_lods);
}

void particle_world::show									( particle_system** in_particle_system )
{
	preview_particle_system* preview_ps = *(preview_particle_system**)in_particle_system;
	
	particle_system_instance_impl* instance = m_ticked_instances_list.front();
	while(instance)
	{
		particle_system_instance_impl* next = m_ticked_instances_list.get_next_of_object(instance);
		
		instance->set_ticked(false);
		
		instance = next;
	}
	
	preview_ps->get_preview_instance()->set_ticked(true);
	preview_ps->get_preview_instance()->m_always_looping = true;
}

u32 particle_world::get_num_preview_particle_system_emitters( particle_system** in_preview_particle_system )
{
	if ( !in_preview_particle_system || !*in_preview_particle_system )
		return 0;
	
	preview_particle_system* preview_ps = *(preview_particle_system**)in_preview_particle_system;
	preview_particle_system_instance* preview_instance = preview_ps->get_preview_instance();
	
	if (!preview_instance)
		return 0;
	
	u32 emitter_index = 0;
	
	particle_emitter_instance* emitter = preview_instance->m_lods[0].m_emitter_instance_list.front();
	while (emitter)
	{
		emitter = preview_instance->m_lods[0].m_emitter_instance_list.get_next_of_object(emitter);
		emitter_index++;
	}
	return emitter_index;
}

void particle_world::gather_stats							( particle_system** in_preview_particle_system, preview_particle_emitter_info* out_info )
{
	preview_particle_system* preview_ps = *(preview_particle_system**)in_preview_particle_system;
	
	preview_particle_system_instance* preview_instance = preview_ps->get_preview_instance();
	
	u32 emitter_index = 0;
	particle_emitter_instance* emitter = preview_instance->m_lods[0].m_emitter_instance_list.front();
	while (emitter)
	{
		//out_info->emitter_name.assignf("%d: ", emitter_index);
		out_info->current_emitter_time		= emitter->m_emitter_time;
		out_info->emitter_duration			= emitter->get_current_duration();
		out_info->num_current_max_particles = emitter->get_current_max_particles();
		out_info->num_live_particles		= emitter->m_num_live_particles;
		out_info->num_fire_events			= 0;
		out_info->enabled					= emitter->get_visible();
		emitter = preview_instance->m_lods[0].m_emitter_instance_list.get_next_of_object(emitter);
		emitter_index++;
		out_info++;
	}
}

#endif // #ifndef MASTER_GOLD

} // namespace particle
} // namespace xray