////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_emitter_instance.h"
#include "particle_actions.h"
#include "particle_world.h"
#include "particle_emitter.h"
#include "particle_system.h"
#include <xray/particle/render_particle_emitter_instance.h>
#include <xray/particle/engine.h>
#include <xray/timing_functions.h>
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/facade/material_effects_instance_cook_data.h>

namespace xray {
namespace particle {

static float calc_duration(float duration, float duration_variance)
{
	float const result = linear_interpolation( duration - duration_variance, duration + duration_variance, random_float(0.0f, 1.0f) );
	return result;
}

static void prepare_render_emitter_instance(particle_emitter_instance* em_instance)
{
	if (!em_instance->get_data_type_action())
		return;
	
	enum_particle_data_type datatype = em_instance->get_data_type_action()->get_data_type();
	bool use_subuv = false;
	if (datatype==particle_data_type_billboard)
	{
		particle_action_billboard* billboard = static_cast_checked<particle_action_billboard*>(em_instance->get_data_type_action());
		use_subuv				 = billboard->m_use_sub_uv;
	}
	
	em_instance->update_render_buffers(datatype, use_subuv);
}

particle_emitter_instance::particle_emitter_instance(
		particle_world& particle_world,
		particle_emitter& emitter,
		engine& engine,
		bool const is_child_emitter_instance
	) :
	m_particle_world( particle_world ),
	m_emitter(emitter),
	m_engine(engine),
	m_render_instance(0),
	m_next(0),
	m_num_live_particles(0),
	m_num_created_particles(0),
	m_waiting_for_end(false),
	m_emitter_time(0.0f),
	m_current_loop(0),
	m_delayed(false), // true
	m_delay_time(0.0f),
	m_time_to_create_new_one(0.0f),
	m_max_num_particles( emitter.m_max_num_particles ),
	m_current_max_num_particles( emitter.m_max_num_particles ),
	m_create_rate(2), // emitter.m_create_rate
	m_current_create_rate(2),
	m_current_calc_num_max_particles(0),
//	m_min_lifetime_numerator(10000.0f),
	m_is_child_emitter_instance(is_child_emitter_instance),
	m_num_particles_to_create(0),
	m_data_type_action(0),
	m_subimage_index(0.0f),
	m_visible(true),
	m_particle_added(false),
	m_current_duration( calc_duration(emitter.m_duration, emitter.m_duration_variance) ),
	m_subuv_pos_uv( math::uint2(0,0) ),
	m_subuv_size_uv( math::float2(1,1) ),
	m_instance_color( math::float4(1.0f,1.0f,1.0f,1.0f) ),
	m_transform( math::float4x4().identity() ),
	m_billboard_parameters(0),
	m_beamtrail_parameters(0),
	m_cook_data_to_delete(NULL),
	m_aabbox((math::create_zero_aabb()))
{
//	if ( math::abs(emitter.m_duration) <= math::epsilon_3 )
//		m_emitter_time_numerator		= 0.0f;
//	else
//		m_emitter_time_numerator		= 1.0f / m_current_duration;
	
	particle_action* action				= emitter.m_actions;
	while ( action ) {
#pragma message( XRAY_TODO("Dima to IronNick: do not use dynamic_casts, use virtual function here instead") )
		particle_action_data_type* datatype = dynamic_cast<particle_action_data_type*>(action);
		if (!datatype || !datatype->get_visibility())
		{
			action						= action->m_next;
			continue;
		}
		
		m_data_type_action				= datatype;
		action							= action->m_next;
		break;
	}
	
	if (m_data_type_action && m_data_type_action->get_data_type() == particle_data_type_billboard)
		m_billboard_parameters	= &static_cast_checked<particle_action_billboard*>(m_data_type_action)->m_billboard_parameters;
	
	if (m_data_type_action && m_data_type_action->get_data_type() == particle_data_type_trail)
		m_beamtrail_parameters	= &static_cast_checked<particle_action_trail*>(m_data_type_action)->m_beamtrail_parameters;
	else if (m_data_type_action && m_data_type_action->get_data_type() == particle_data_type_beam)
		m_beamtrail_parameters	= &static_cast_checked<particle_action_beam*>(m_data_type_action)->m_beamtrail_parameters;
	
	m_world_space						= m_emitter.get_world_space();
	
	create_render_particle_emitter_instance	( engine );
	
	prepare_render_emitter_instance		( this );
	
	if (m_data_type_action && 
		(m_data_type_action->get_data_type()==particle_data_type_mesh || 
		m_data_type_action->get_data_type()==particle_data_type_decal)
		)
	{
		return;
	}
	
	pcstr const material_name			= emitter.m_material_name[0] ? emitter.m_material_name : "default_particle";
	load_material						( material_name );
}
	
particle_emitter_instance::~particle_emitter_instance()
{
	remove_particles();
	if (m_render_instance)
		m_engine.destroy(m_render_instance);
}

u32 particle_emitter_instance::get_current_max_particles() const
{
	return m_current_calc_num_max_particles;
}

void particle_emitter_instance::create_render_particle_emitter_instance	( engine& engine )
{
	R_ASSERT							( !m_render_instance );
	m_render_instance					=
		engine.create_render_emitter_instance(
			*this,
			m_particle_list,
			m_billboard_parameters,
			m_beamtrail_parameters,
			m_billboard_parameters ? m_billboard_parameters->locked_axis : particle_locked_axis_x,
			m_billboard_parameters ? m_billboard_parameters->screen_alignment : particle_screen_alignment_rectangle,
			m_world_space ? math::float4x4().identity() : m_transform,
			m_instance_color
		);
}

float particle_emitter_instance::get_max_particle_lifetime() const
{
	return m_emitter.m_particle_lifetime_curve.get_max_value();
}

void particle_emitter_instance::set_visible( bool visible )
{
	m_visible = visible;
}

bool particle_emitter_instance::get_visible( ) const
{
	return m_visible && m_emitter.m_visibility;
}

bool particle_emitter_instance::is_finished() const
{
	// Never finish.
	if (m_emitter.m_num_loops==0)
		return false;
	
	return m_current_loop == m_emitter.m_num_loops && !m_waiting_for_end;
}

void particle_emitter_instance::play_child( particle_event* evt, xray::math::float4x4 const& transform )
{
	bool found_emitter = false;
	for (u32 lod_index=0; lod_index<1; lod_index++)
	{
		for (u32 i=0; i<m_emitter.m_particle_system->m_lods[lod_index].m_num_emitters; i++)
		{
			particle_emitter* emitter = &m_emitter.m_particle_system->m_lods[lod_index].m_emitters_array[i];
			if (emitter->m_event!=evt)
				continue;
			
			found_emitter = true;
		}
	}
	
	if (!found_emitter)
		return;
	
	for (u32 lod_index=0; lod_index<1; lod_index++)
	{
		for (u32 i=0; i<m_emitter.m_particle_system->m_lods[lod_index].m_num_emitters; i++)
		{
			particle_emitter* emitter = &m_emitter.m_particle_system->m_lods[lod_index].m_emitters_array[i];
			
			if (emitter->m_event!=evt || !emitter->m_event->get_visibility())
				continue;
			
			particle_emitter_instance* instance = m_particle_world.create_emitter_instance( *emitter, true );
			instance->set_transform(transform);
			m_particle_system_instance->add_emitter_instance(lod_index, instance);
		}
	}
}

void particle_emitter_instance::process_event(enum_particle_event const& event_type, xray::float3 const& location)
{
	particle_action* action = m_emitter.m_actions;
	while (action)
	{
#pragma message( XRAY_TODO("Dima to IronNick: do not use dynamic_casts, use virtual function here instead") )
		particle_event* evt = dynamic_cast<particle_event*>(action);
		if (!evt)
		{
			action = action->m_next;
			continue;
		}
		if (evt->get_event_type()==event_type && evt->get_visibility())
		{
			float4x4 transf = float4x4().identity();
			
			if (evt->get_inherit_position())
				transf = xray::math::create_translation(location);
			//particle_system_instance_ptr psi_ptr = m_emitter.m_particle_system->play_child(m_particle_world, evt, transf);
			play_child(evt, transf);
		}
		action = action->m_next;
	}
}

void particle_emitter_instance::shrink_particles(float time_delta, float limit_over_total, u32 num_need_particles)
{
	XRAY_UNREFERENCED_PARAMETERS(num_need_particles, time_delta);
	
	m_current_max_num_particles		= u32( m_current_calc_num_max_particles * limit_over_total );
	m_current_create_rate			= m_create_rate * limit_over_total;
}

u32 particle_emitter_instance::remove_dead_particles()
{
	u32 num_removed = 0;
	base_particle* P = m_particle_list.front();
	while(P)
	{
		if (P->is_dead())
		{
			process_event(event_on_death,P->position);
			
			m_particle_list.erase(P);
			
			base_particle* to_del = P;
			
			P = m_particle_list.get_next_of_object(P);
			
			m_particle_world.deallocate_particle(to_del);
			
			m_num_live_particles--;
			num_removed++;
		}
		else 
			P = m_particle_list.get_next_of_object(P);
	}
	
	return num_removed;
}

void particle_emitter_instance::remove_particles(u32 num)
{
	u32 index = 0;
	
	base_particle* P = m_particle_list.front();
	while(P)
	{
		if (index == num)
			break;
		
		index++;
		
		m_particle_list.erase(P);
		
		base_particle* to_del = P;
		
		P = m_particle_list.get_next_of_object(P);
		
		m_num_live_particles--;
		m_particle_world.deallocate_particle(to_del);
	}
	
	if (num==(u32)-1)
		m_particle_list.clear();
}

void particle_emitter_instance::remove_overflowing_particles()
{
	u32 num_need = m_num_live_particles + m_num_particles_to_create;
	
	if (num_need > m_current_max_num_particles)
	{
		remove_particles( num_need - m_current_max_num_particles + 1 );
	}
}

u32 particle_emitter_instance::calc_num_max_particles( float time_delta )
{
	XRAY_UNREFERENCED_PARAMETERS(time_delta);
	
	m_create_rate = m_emitter.m_particle_spawn_rate_curve.evaluate(m_emitter_time, 1.0f, range_time_type, u32(this));
	
	u32 num_max_particles_by_burst_list = 0;
	for (u32 i=0; i<m_emitter.m_num_burst_entries; i++)
		num_max_particles_by_burst_list += math::abs(m_emitter.m_burst_entries[i].count);
		
	m_current_calc_num_max_particles = 2 + num_max_particles_by_burst_list + math::floor(m_create_rate * get_max_particle_lifetime());
	
	return m_current_calc_num_max_particles;
}

u32 particle_emitter_instance::calc_num_new_particles( float time_delta, bool allow_to_create_new )
{
	m_num_particles_to_create = 0;
	
	if (!allow_to_create_new || m_delayed || m_num_live_particles>=m_max_num_particles)
		return 0;
	
	if (m_emitter.m_num_loops!=0)
	{
		if (m_waiting_for_end)
			return 0;
	}
	
	// Check burst list first. Burst list already sorted by desc.
	for (u32 i=0; i<m_emitter.m_num_burst_entries; i++)
	{
		burst_entry& one_burst = m_emitter.m_burst_entries[i];
		
		//if (one_burst.count > 0 && one_burst.time<=m_emitter_time)
		if (m_emitter_time<=one_burst.time && one_burst.time < m_emitter_time + time_delta)
		{
			m_num_particles_to_create = u32(xray::math::max( random_float(
				float(one_burst.count-one_burst.count_variance), 
				float(one_burst.count+one_burst.count_variance)), 0.0f));
			
			//one_burst.count *= -1;
			break;
		}
	}
	
	// Then rated particles.
	u32 num_new_particles = 0;
	
	float f_num_new				= m_time_to_create_new_one + time_delta * m_current_create_rate;
	num_new_particles			= xray::math::floor(f_num_new);
	m_time_to_create_new_one	= f_num_new - num_new_particles;
	m_num_particles_to_create	+= num_new_particles;
	
	return num_new_particles;
}

void particle_emitter_instance::append_particles( float time_delta)
{
	for (u32 i=0; i<m_num_particles_to_create; i++)
	{
		if (m_num_live_particles + 1 > m_max_num_particles)
			break;
		
		base_particle* new_particle = m_particle_world.allocate_particle();
		
		if (!new_particle)
			continue;
		
		m_particle_list.push_back(new_particle);
		
		new_particle->m_seed	= u32(new_particle) + static_cast<u32>(xray::timing::get_QPC());
		new_particle->duration  = m_emitter.m_particle_lifetime_curve.evaluate(m_emitter_time, 1.0f, range_time_type, new_particle->seed());
		
		particle_action* modifier = m_emitter.m_actions;
		while(modifier)
		{
			if (modifier->get_visibility())
				modifier->init(this,new_particle,time_delta);
			modifier = modifier->m_next;
		}
		
		if (m_emitter.m_world_space)
			new_particle->position = m_transform.transform_position(new_particle->position);
		
		process_event(event_on_birth,new_particle->position);
		
		m_num_live_particles++;
		m_num_created_particles++;
	}
}

void particle_emitter_instance::recalc_duration()
{
	m_current_duration = calc_duration(m_emitter.m_duration, m_emitter.m_duration_variance);
}

bool particle_emitter_instance::is_emitter_finished() const
{
	return (m_emitter.m_duration > 0.001f && m_emitter_time > m_current_duration);
}

float particle_emitter_instance::get_linear_emitter_time() const
{
	if (m_emitter.m_duration > 0.001f)
	{
		return m_emitter_time / m_emitter.m_duration;
	}
	else
	{
		return m_emitter_time;
	}
}

void particle_emitter_instance::tick(float time_delta, bool /*create_new_particles*/, float alpha)
{
	// TODO: remove it!
	if (!m_data_type_action)
		return;

	m_instance_color.w	= alpha;
	
	m_aabbox.zero();
	
	if (m_delayed)
	{
		m_delay_time += time_delta;
		if (m_delay_time>m_emitter.m_delay)
			m_delayed = false;
		else
			return;
	}
	
	m_emitter_time += time_delta;// * m_emitter_time_numerator;
	
	if (is_emitter_finished())
	{
		if (m_emitter.m_num_loops!=0)
		{
			m_current_loop++;
			xray::math::clamp<u32>(m_current_loop, 0, m_emitter.m_num_loops);
			
			recalc_duration();
		}
		recalc_duration();
		m_emitter_time = 0.0f;
		for (u32 i=0; i<m_emitter.m_num_burst_entries; i++)
		{
			burst_entry& one_burst = m_emitter.m_burst_entries[i];
			if (one_burst.count < 0)
				one_burst.count *= -1;
		}
	}
	
	if (m_emitter.m_num_loops!=0)
	{
		if (m_current_loop==m_emitter.m_num_loops)
		{
			m_waiting_for_end = true;
		}
	}
	
	if (m_waiting_for_end && m_num_live_particles==0)
		m_waiting_for_end = false;
	
	append_particles( time_delta );
	
	m_aabbox.move(m_transform.lines[3].xyz());
	
	base_particle* P = m_particle_list.front();
	while(P)
	{
		P->rotation_rate = P->start_rotation_rate;
		P->size			 = P->start_size;
		P->velocity		 = P->start_velocity;
		
		P->old_position = P->position;
		particle_action* modifier = m_emitter.m_actions;
		while(modifier)
		{
			if (modifier->get_visibility())
				modifier->update(this,P,time_delta);
			modifier = modifier->m_next;
		}
		P->lifetime		= P->lifetime + time_delta;
		
		P->position		+= P->velocity * time_delta;
		
		m_aabbox.modify(P->position);
		
		//LOG_INFO("%.3f, %.3f, %.3f", P->position.x, P->position.y, P->position.z);
		
		float3 ratation_rate	= P->rotation_rate * time_delta * xray::math::pi_x2;
		
		P->rotation		= P->rotation  + ratation_rate.x;
		P->rotationY	= P->rotationY + ratation_rate.y;
		P->rotationZ	= P->rotationZ + ratation_rate.z;
		
		if (P->is_dead())
		{
			process_event(event_on_death,P->position);
			
			m_particle_list.erase(P);
			base_particle* to_del = P;
			P = m_particle_list.get_next_of_object(P);
			m_particle_world.deallocate_particle(to_del);
			m_num_live_particles--;
		}
		else
		{
			P = m_particle_list.get_next_of_object(P);
		}
	}
	
	if (!m_emitter.m_world_space)
	{
		m_aabbox.modify(m_transform);
	}
	
	m_render_instance->set_aabb(m_aabbox);
}

void particle_emitter_instance::load_material(pcstr material_name)
{
	//m_particle_system_instance_ptr = m_particle_system_instance;
	/*
	query_resource(
		material_name,
		resources::material_class,
		boost::bind( &particle_emitter_instance::on_material_loaded, this, _1 ),
		&::xray::memory::g_mt_allocator
	);
	*/
	
	resources::user_data_variant	user_data;
	xray::render::enum_vertex_input_type	vertex_input_type = xray::render::null_vertex_input_type;
	
	if (m_billboard_parameters)
	{
		vertex_input_type = xray::render::particle_vertex_input_type;
		
		if (get_data_type_action())
		{
			enum_particle_data_type datatype = get_data_type_action()->get_data_type();
			bool use_subuv = false;
			if (datatype==particle_data_type_billboard)
			{
				particle_action_billboard* billboard = static_cast_checked<particle_action_billboard*>(get_data_type_action());
				use_subuv				 = billboard->m_use_sub_uv;
			}
			if (use_subuv)
			{
				vertex_input_type = xray::render::particle_subuv_vertex_input_type; 
			}
		}
	}
	else if (m_beamtrail_parameters)
	{
		vertex_input_type = xray::render::particle_beamtrail_vertex_input_type;
	}
	
	m_cook_data_to_delete			=
		XRAY_NEW_IMPL(
			&::xray::memory::g_mt_allocator, xray::render::material_effects_instance_cook_data)(
			vertex_input_type, 
			NULL,
			false
		);
	
	user_data.set					(m_cook_data_to_delete);
	
	resources::query_resource		(
		material_name,
		xray::resources::material_effects_instance_class,
		boost::bind(&particle_emitter_instance::on_material_loaded, this, _1),
		&::xray::memory::g_mt_allocator,
		&user_data
	);
}

float particle_emitter_instance::get_particle_system_time	() const 
{ 
	return m_particle_system_instance->get_particle_system_time(); 
}

void particle_emitter_instance::on_material_loaded(xray::resources::queries_result& result)
{
	//R_ASSERT								( result.is_successful() );
	if (result.is_successful())
		change_material							( result[0].get_unmanaged_resource() );
	
	if (m_cook_data_to_delete)
		XRAY_DELETE_IMPL(&::xray::memory::g_mt_allocator, m_cook_data_to_delete);
	//m_particle_system_instance_ptr = 0;
}

void particle_emitter_instance::update_render_buffers	( enum_particle_data_type const& data_type, bool use_subuv )
{
	m_render_instance->update_render_buffers	(
		data_type,
		use_subuv,
		m_max_num_particles,
		m_beamtrail_parameters ? m_beamtrail_parameters->num_sheets : 0//get_num_sheets()
	);
}

void particle_emitter_instance::set_transform	( xray::math::float4x4 const& transform )
{
	m_transform							= transform;
	m_render_instance->set_transform	( m_world_space ? math::float4x4().identity() : m_transform );
}

void particle_emitter_instance::change_material	( resources::unmanaged_resource_ptr const& material )
{
	m_material							= material;
	m_render_instance->change_material	( m_material );
}

} // namespace particle
} // namespace xray
