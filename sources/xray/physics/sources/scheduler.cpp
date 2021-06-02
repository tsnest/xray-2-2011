////////////////////////////////////////////////////////////////////////////
//	Created 	: 04.02.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "scheduler.h"
#include "step_object.h"
#include "physics_world.h"
#include "temp_storage_holder.h"
#include "contact_storage.h"
#include "integration_constants.h"

#include <xray/console_command.h>

float s_physics_time_factor = 1.f;
static console_commands::cc_float s_cmd_physics_time_factor( "physics_time_factor", s_physics_time_factor, 0.f, 100.f, true, console_commands::command_type_user_specific );

scheduler::scheduler( memory::base_allocator& allocator, const physics::physics_world& w )
:
m_active_objects( allocator ),
m_world			( w ),
m_time_left		( 0 )
{
	
}

float integration_step = default_integration_step;

//u32 dbg_count  = 0;
void	scheduler::tick( )
{
//	return;
	//...
	// get delta and divide into fixed
	//...
	float time_passed = m_timer.get_elapsed_sec() * s_physics_time_factor;
	m_timer.start();
	
	float time = m_time_left + time_passed;

	u32 steps_count = math::floor( time/integration_step );
	
	if ( steps_count == 0 ) {
		m_time_left = time;
		return;
	}
	
	u32 const max_steps_count_per_tick = 10u;

	u32 step_times = xray::math::min( max_steps_count_per_tick, steps_count );
	for ( u32 i = 0; i < step_times ; ++i )	
			step( integration_step );

	m_time_left = time - float( steps_count ) * integration_step;
	
	/*
	++dbg_count;
	const float time_delta	= 0.001f;
	const u32	sub_steps_num	= 1;//50;	
	if( dbg_count % 1 == 0 )
		for( u32 i = 0; i < sub_steps_num ; ++i )	
			step( time_delta );

	*/
	//::render(  world().get_temp_storage_holder().contacts(), world().render_world() );
}

void	scheduler::activate	( step_object *o )
{
	ASSERT( o );
	m_active_objects.push_back( o );
}

bool scheduler::deactivate( step_object *o )
{
	ASSERT( o );
	vectora<step_object*>::iterator r = std::find( m_active_objects.begin(), m_active_objects.end(), o );
	if ( r != m_active_objects.end() ) {
		m_active_objects.erase( r );
		return true;
	}
	return false;
}

void scheduler::step( float time_delta )
{
	m_step_counter.step();
	vectora<step_object*>::reverse_iterator			ri = m_active_objects.rbegin();
	vectora<step_object*>::reverse_iterator			re = m_active_objects.rend();
	
	for( ;ri != re; ++ri )
	{
		if( !(*ri)->step( *this, time_delta ) )
		{
			vectora<step_object*>::iterator		ei = m_active_objects.end() - 1;
			std::swap( *ei, *ri );
			m_active_objects.erase( ei );
		}

	}

}