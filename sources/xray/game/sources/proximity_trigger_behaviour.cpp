////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "proximity_trigger_behaviour.h"
#include "object_proximity_trigger.h"
#include "object_scene.h"
#include "game_world.h"
#include "hud.h"

#ifndef MASTER_GOLD
#	include "game.h"
#	include "proximity_trigger_input_handler.h"
#	include <xray/console_command.h>
#	include <xray/collision/api.h>
#	include <xray/render/facade/debug_renderer.h>
#endif //#ifndef MASTER_GOLD


typedef collision::geometry				geometry;
typedef collision::geometry_instance	geometry_instance;
typedef collision::primitive			primitive;

namespace stalker2{

////////////////			I N I T I A L I Z E				////////////////

proximity_trigger_behaviour::proximity_trigger_behaviour ( object_scene_job* owner, configs::binary_config_value const& data, pcstr name )
	:super				( owner, data, name ),
	m_proximity_trigger	( NULL ),
	m_testee			( NULL )
{
	pcstr testees_source = data["testees_source"];

	if( xray::strings::compare( testees_source, "HUD" ) == 0 )
	{
		m_testee = &owner->get_game_world( ).get_hud( )->get_caracter_capsule( );
	}

#ifndef MASTER_GOLD
	static xray::console_commands::cc_bool		is_testee_volume_enabled_cc	( "proximity_trigger_testee_volume_enabled", m_is_testee_volume_enabled, true, console_commands::command_type_user_specific );

	if( !m_is_input_handler_enabled_catched )
	{
		m_input_handler			= NEW( proximity_trigger_input_handler )( *this );
		static xray::console_commands::cc_bool		is_input_handler_enabled_cc	( "proximity_trigger_input_handler_enabled", m_is_input_handler_enabled, true, console_commands::command_type_user_specific );
		is_input_handler_enabled_cc.subscribe_on_change	( boost::bind( &proximity_trigger_behaviour::input_handler_enabled_changed, this, _1 ) );
		if( m_is_input_handler_enabled )
			input_handler_enabled_changed( "" );

		m_is_input_handler_enabled_catched = true;
	}

	m_testee_primitive.data_		= float3( 1, 1, 1 );
	m_testee_primitive.type			= collision::primitive_capsule;
	m_last_testee_primitive_type	= collision::primitive_capsule;
#endif //#ifndef MASTER_GOLD
}

#ifndef MASTER_GOLD
bool proximity_trigger_behaviour::m_is_testee_volume_enabled = false;
bool proximity_trigger_behaviour::m_is_input_handler_enabled_catched = false;
#endif //#ifndef MASTER_GOLD

proximity_trigger_behaviour::~proximity_trigger_behaviour ( )
{
#ifndef MASTER_GOLD
	if( m_is_input_handler_enabled )
		m_input_handler->remove_handler	( );

	DELETE( m_input_handler );
#endif //#ifndef MASTER_GOLD

	m_proximity_trigger	 = NULL;
}

////////////////		P U B L I C   M E T H O D S			////////////////


void proximity_trigger_behaviour::attach_to_object ( object_controlled* o )
{
	super::attach_to_object		( o );
	
	m_proximity_trigger						= static_cast_checked<object_proximity_trigger*>( o );

#ifndef MASTER_GOLD
	if( m_is_input_handler_enabled )
		m_input_handler->add_handler	( );
#endif //#ifndef MASTER_GOLD
}

void proximity_trigger_behaviour::detach_from_object ( object_controlled* o )
{
	super::detach_from_object	( o );

#ifndef MASTER_GOLD
	if( m_is_input_handler_enabled )
		m_input_handler->remove_handler	( );

	m_is_input_handler_enabled_catched = false;
#endif //#ifndef MASTER_GOLD
}

#ifndef MASTER_GOLD
struct primitive_callback: collision::enumerate_primitives_callback
{
	primitive m_primitive;

	virtual void enumerate( float4x4 const &transform, primitive const &p )
	{
		XRAY_UNREFERENCED_PARAMETER( transform );
		m_primitive.type	= p.type;
		m_primitive.data_	= p.data_;
	}
};
#endif //#ifndef MASTER_GOLD

void proximity_trigger_behaviour::tick ( )
{

#ifndef MASTER_GOLD
	if( m_testee == NULL || m_testee_primitive.type != m_last_testee_primitive_type )
	{
		m_last_testee_primitive_type = m_testee_primitive.type;

		float4x4 matrix;
		matrix.identity( );

		if( m_testee != NULL )
		{
			matrix = m_testee->get_matrix		( );
			collision::delete_geometry_instance	( g_allocator, m_testee );
		}

		switch( m_testee_primitive.type )
		{
			case collision::primitive_box:
				m_testee			= &*collision::new_box_geometry_instance	( g_allocator, math::create_scale(m_testee_primitive.box( ).half_side) * matrix );
				break;

			case collision::primitive_sphere:
				m_testee			= &*collision::new_sphere_geometry_instance	( g_allocator, matrix, m_testee_primitive.sphere( ).radius );
				break;

			case collision::primitive_cylinder:
				m_testee			= &*collision::new_cylinder_geometry_instance	( g_allocator, matrix, m_testee_primitive.cylinder( ).radius, m_testee_primitive.cylinder( ).half_length );
				break;

			case collision::primitive_capsule:
				m_testee			= &*collision::new_capsule_geometry_instance	( g_allocator, matrix, m_testee_primitive.capsule( ).radius, m_testee_primitive.capsule( ).half_length );
				break;

		default:
			NODEFAULT( );
		}
	}
#endif //#ifndef MASTER_GOLD
	proximity_trigger_testee_status testee_status	= m_proximity_trigger->test_object( m_testee );

	if( testee_status == proximity_trigger_testee_status_enter )
	{
		game_event ev("on enter");
		m_owner->get_event_manager( )->emit_event( ev, m_proximity_trigger );
	}
	else if( testee_status == proximity_trigger_testee_status_leave )
	{
		game_event ev("on exit");
		m_owner->get_event_manager( )->emit_event( ev, m_proximity_trigger );
	}

#ifndef MASTER_GOLD

	render::debug::renderer& renderer	= m_proximity_trigger->get_game_world( ).get_game( ).renderer( ).debug( );
	render::scene_ptr const& scene		= m_proximity_trigger->get_game_world( ).get_game( ).get_render_scene( );

	if( m_is_testee_volume_enabled )
	{
		primitive_callback callback;
		m_testee->enumerate_primitives( callback );
		m_testee_primitive.type		= callback.m_primitive.type;
		m_testee_primitive.data_	= callback.m_primitive.data_;

		switch( m_testee_primitive.type )
		{
			case collision::primitive_sphere:
				renderer.draw_sphere_solid	( scene, m_testee->get_matrix( ).c.xyz( ), m_testee_primitive.sphere( ).radius, math::color( 255, 255, 0, 200 ) );
				break;
			case collision::primitive_box:
				renderer.draw_cube_solid	( scene, m_testee->get_matrix( ), m_testee_primitive.box( ).half_side, math::color( 255, 255, 0, 200 ) );
				break;
			case collision::primitive_cylinder:
				renderer.draw_cylinder_solid( scene, m_testee->get_matrix( ), float3( m_testee_primitive.cylinder( ).radius, m_testee_primitive.cylinder( ).half_length, m_testee_primitive.cylinder( ).radius ), math::color( 255, 255, 0, 200 ) );
				break;
			case collision::primitive_capsule:
				renderer.draw_solid_capsule	( scene, m_testee->get_matrix( ), float3( m_testee_primitive.capsule( ).radius, m_testee_primitive.capsule( ).half_length, m_testee_primitive.capsule( ).radius ), math::color( 255, 255, 0, 200 ) );
				break;
		}
	}

#endif //#ifndef MASTER_GOLD
}

////////////////		D E B U G    M E T H O D S			////////////////

#ifndef MASTER_GOLD

void proximity_trigger_behaviour::input_handler_enabled_changed( pcstr args )
{
	XRAY_UNREFERENCED_PARAMETER( args );

	if( proximity_trigger_behaviour::m_proximity_trigger == NULL )
		return;

	if( m_is_input_handler_enabled )
		proximity_trigger_behaviour::m_input_handler->add_handler	( );
	else
		proximity_trigger_behaviour::m_input_handler->remove_handler	( );
}

float4x4 const& proximity_trigger_behaviour::get_testee_transform ( )
{
	return m_testee->get_matrix( ); 
}

void proximity_trigger_behaviour::set_testee_transform ( float4x4 const& matrix )
{
	return m_testee->set_matrix( matrix ); 
}

#endif //#ifndef MASTER_GOLD

} // namespace stalker2