////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "vision_sensor.h"
#include "ai_world.h"
#include <xray/ai/npc.h>
#include <xray/ai/collision_object.h>
#include "sensed_object_types.h"
#include "sensed_object.h"
#include <xray/ai/npc_statistics.h>
#include "behaviour.h"

namespace xray {
namespace ai {
namespace sensors {

vision_sensor::vision_sensor	( npc& npc, ai_world& world, brain_unit& brain ) :
	active_sensor_base			( npc, world, brain ),
	m_last_tick					( world.get_current_time_in_ms() ),
	m_subscription				( boost::bind( &vision_sensor::on_object_destruction, return_this(), _1 ) )
{
	m_world.destruction_manager().subscribe		( m_subscription );
}

vision_sensor::~vision_sensor	( )
{
	m_world.destruction_manager().unsubscribe	( m_subscription );

	while ( sensed_visual_object* visual_object = m_visible_objects.pop_front() )
		DELETE					( visual_object );
}

struct find_visual_object_predicate : private boost::noncopyable
{
	inline find_visual_object_predicate		( game_object const& ai_game_object ) :
		visual_object						( ai_game_object )
	{
	}
	
	inline bool	operator ( )				( sensed_visual_object* const current_object ) const
	{
		return current_object->object		== &visual_object;
	}

	game_object const&						visual_object;
};

sensed_visual_object* vision_sensor::find_visual_object_in_list	( 
		game_object const* const ai_game_object,
		visible_objects& objects_list
	)
{
	find_visual_object_predicate check_predicate( *ai_game_object );
	return objects_list.find_if					( check_predicate );
}

void vision_sensor::check_frustum			( )
{
	math::float4x4 const projection			= math::create_perspective_projection(
												math::deg2rad( m_parameters.vertical_fov ),
												m_parameters.aspect_ratio,
												m_parameters.near_plane_distance,
												m_parameters.far_plane_distance
											);
	math::frustum const view_frustum		( mul4x4( m_npc.get_eyes_matrix(), projection ) );
	update_frustum_callback_type const callback( boost::bind( &vision_sensor::update_frustum_objects, this, _1 ) );
	m_world.get_visible_objects				( view_frustum, callback );
}

struct cleaning_not_in_frustum_predicate : private boost::noncopyable
{
	inline cleaning_not_in_frustum_predicate( vision_sensor::visible_objects& objects ) :
		objects_to_clean					( objects )
	{
	}
	
	bool operator	()						( sensed_visual_object* visual_object ) const
	{
		if ( visual_object->is_in_frustum )
			return							false;

		DELETE								( visual_object );	
		return								true;
	}

	vision_sensor::visible_objects&			objects_to_clean;
};

void vision_sensor::delete_not_in_frustum 	( )
{
	m_visible_objects.remove_if				( cleaning_not_in_frustum_predicate( m_visible_objects ) );
}

void vision_sensor::add_new_visible_object	( game_object const* const ai_game_object )
{
	sensed_visual_object* visual_object		= NEW( sensed_visual_object );
	visual_object->object					= ai_game_object;
	visual_object->is_in_frustum			= true;
	visual_object->newly_added_to_frustum	= true;
	m_visible_objects.push_back				( visual_object );
}

bool vision_sensor::check_if_in_blind_zones		( game_object const* const ai_game_object ) const
{
	float3 const& object_position				= ai_game_object->local_to_cell( m_npc.get_eyes_position() ).transform_position( ai_game_object->get_collision_object()->get_origin() );
	float const peripheral_view_angle			= math::acos( m_npc.get_eyes_direction() | normalize( object_position - m_npc.get_eyes_position() ) );
	float const horizontal_fov_d2				= math::atan( 1.f / m_parameters.aspect_ratio * math::tan( m_parameters.vertical_fov / 2.f ) );
	float const angles_ratio					= math::clamp_r( peripheral_view_angle / horizontal_fov_d2, 0.f, 1.f );
	float const side_plane						= m_parameters.min_indirect_view_factor * m_parameters.far_plane_distance;
	float const blind_zone_bound				= m_parameters.far_plane_distance + angles_ratio * ( side_plane - m_parameters.far_plane_distance );
	return ( object_position - m_npc.get_eyes_position() ).length() > blind_zone_bound;
}

struct update_visibility_parameters
{
	npc const*	object_to_ignore;
	float3		npc_position;
	float		time_quant;
	float		velocity_factor;
	float		luminosity_factor;
	float		decrease_factor;
	float		far_plane_distance;
	float		near_plane_distance;
	float		max_visibility_value;
	float		visibility_threshold;
	float		transparency_threshold;
	u32			last_update_time;
};

struct update_visibility_predicate : private boost::noncopyable
{
	inline update_visibility_predicate			( ai_world& ai_world, update_visibility_parameters& visibility_parameters )
		:	world								( ai_world ),
			parameters							( visibility_parameters )
	{
	}
	
	void operator()								( sensed_visual_object* visual_object )
	{
		float ray_energy						= 1.f;
		float3 const& point_in_world_frame		= visual_object->object->local_to_cell( parameters.npc_position ).transform_position( visual_object->local_point );

		bool const ray_query_succeeded			= world.ray_query(
													visual_object->object->get_collision_object(),
													parameters.object_to_ignore->cast_game_object()->get_collision_object(),
													parameters.npc_position,
													normalize( point_in_world_frame - parameters.npc_position ),
													parameters.far_plane_distance,
													parameters.transparency_threshold,
													ray_energy
												);
		if ( parameters.object_to_ignore->debug_draw_allowed() )
			world.draw_ray						( parameters.npc_position, point_in_world_frame, ray_query_succeeded );
		
		u32 const time_delta					= ( world.get_current_time_in_ms() - parameters.last_update_time ) / 1000;
		visual_object->distance					= ray_query_succeeded ? ( point_in_world_frame - parameters.npc_position ).length() : ( visual_object->object->get_collision_object()->get_origin() - parameters.npc_position ).length();
		visual_object->own_position				= parameters.npc_position;
		visual_object->was_visible_last_time	= ray_query_succeeded;
		if ( !ray_query_succeeded )
		{
			visual_object->visibility_value		-= parameters.decrease_factor;
			return;
		}
		
		float const calculated_luminosity		= exp( visual_object->object->get_luminosity() * parameters.luminosity_factor );
		float const visibility_delta			= time_delta / parameters.time_quant * calculated_luminosity *
												( 1.f + parameters.velocity_factor * visual_object->object->get_velocity() ) *
												( parameters.far_plane_distance - visual_object->distance ) / ( parameters.far_plane_distance - parameters.near_plane_distance );
		float visibility_value					= visual_object->visibility_value + visibility_delta * ray_energy;

		if ( visibility_value > parameters.max_visibility_value )
			visibility_value					= parameters.max_visibility_value;

		visual_object->was_updated				= visual_object->visibility_value != visibility_value;
		visual_object->visibility_value			= visual_object->newly_added_to_frustum ? parameters.visibility_threshold : visibility_value;
		visual_object->newly_added_to_frustum	= false;
	}

	ai_world&									world;
	update_visibility_parameters&				parameters;	
};

void vision_sensor::update_visibility_value		( )
{
	update_visibility_parameters				visibility_parameters;
	visibility_parameters.object_to_ignore		= &m_npc;
	visibility_parameters.npc_position			= m_npc.get_eyes_position();
	visibility_parameters.last_update_time		= m_last_tick;
	visibility_parameters.time_quant			= m_parameters.time_quant;
	visibility_parameters.velocity_factor		= m_parameters.velocity_factor;
	visibility_parameters.luminosity_factor		= m_parameters.luminosity_factor;
	visibility_parameters.decrease_factor		= m_parameters.decrease_factor;
	visibility_parameters.far_plane_distance	= m_parameters.far_plane_distance;
	visibility_parameters.near_plane_distance	= m_parameters.near_plane_distance;
	visibility_parameters.max_visibility_value	= m_parameters.max_visibility;
	visibility_parameters.visibility_threshold	= m_parameters.visibility_threshold;
	visibility_parameters.transparency_threshold = m_parameters.transparency_threshold;

	update_visibility_predicate					visibility_predicate( m_world, visibility_parameters );
	m_visible_objects.for_each					( visibility_predicate );
}

void vision_sensor::update_visible_objects	( )
{
	reset_frustum_status					( );
	check_frustum							( );
	delete_not_in_frustum					( );
	trace_objects_in_frustum				( );
	update_visibility_value					( );
	remove_invisible_objects				( );
}

void vision_sensor::tick			( )
{
	if ( !m_parameters.enabled )
		return;

	u32 const current_time			= m_world.get_current_time_in_ms();
	
	if ( current_time - m_last_tick < 1000 )
		return;

	update_visible_objects			( );
	update_perceptors				( );

#ifdef DEBUG
	if ( m_npc.debug_draw_allowed() )
		m_world.draw_frustum			(
			math::deg2rad( m_parameters.vertical_fov ),
			m_parameters.far_plane_distance,
			m_parameters.aspect_ratio,
			m_npc.get_eyes_position(),
			m_npc.get_eyes_direction(),
			m_npc.get_color()
		);
#endif // #ifdef DEBUG
}

struct update_perceptors_predicate : private boost::noncopyable
{
	inline update_perceptors_predicate	( vision_sensor const& sensor, float const threashold ) :
		vision							( sensor ),
		visibility_threashold			( threashold )
	{
	}
	
	void operator()						( sensed_visual_object* sensed_object ) const
	{
		if ( sensed_object->was_updated && sensed_object->visibility_value >= visibility_threashold )
			vision.add_fact				( sensed_object ); 
	}

	vision_sensor const&				vision;
	float const							visibility_threashold;
};

void vision_sensor::update_perceptors	( ) const
{		
	m_visible_objects.for_each			( update_perceptors_predicate( *this, m_parameters.visibility_threshold ) ); 
}

void vision_sensor::add_fact			( sensed_visual_object* visual_object ) const
{
	sensed_object visible_object		(
		visual_object->own_position,
		visual_object->local_point,
		c_ptr( visual_object->object ),
		m_world.get_current_time_in_ms(),
		sensed_object_type_visual,
		visual_object->visibility_value / m_parameters.max_visibility
	);
	m_brain_unit.on_seen_object			( visible_object );
}

void vision_sensor::set_parameters		( behaviour const& behaviour_parameters )
{
	m_parameters						= behaviour_parameters.get_vision_sensor_parameters();
}

void vision_sensor::on_object_destruction	( game_object const& destroyed_object )
{
	sensed_visual_object* object_to_be_deleted = find_visual_object_in_list( &destroyed_object, m_visible_objects );
	if ( object_to_be_deleted && m_visible_objects.erase( object_to_be_deleted ) )
		DELETE	( object_to_be_deleted );
}

void vision_sensor::dump_state			( npc_statistics& stats ) const
{
	npc_statistics::sensor_info_type	new_stats_item;
	new_stats_item.caption				= "visible objects:";
	for ( sensed_visual_object* iter = m_visible_objects.front(); iter; iter = m_visible_objects.get_next_of_object( iter ) )
		new_stats_item.content.push_back( iter->object->get_name() );

	if ( m_visible_objects.empty() )
		new_stats_item.content.push_back( "none" );
	
	stats.sensors_state.push_back		( new_stats_item );
}

struct cleansing_visible_objects_predicate : private boost::noncopyable
{
	bool operator()						( sensed_visual_object* visual_object ) const
	{
		bool const can_be_forgotten		= visual_object->visibility_value <= 0;
		if ( !can_be_forgotten )
			return						false;

		DELETE							( visual_object );	
		return							true;
	}
};

void vision_sensor::remove_invisible_objects( )
{
	m_visible_objects.remove_if				( cleansing_visible_objects_predicate() );
}

struct reset_frustum_status_predicate : private boost::noncopyable
{
	void operator()						( sensed_visual_object* visual_object ) const
	{
		visual_object->is_in_frustum	= false;
	}
};

void vision_sensor::reset_frustum_status( )
{
	m_visible_objects.for_each			( reset_frustum_status_predicate() );
}

void vision_sensor::update_frustum_objects	( game_object const& ai_game_object )
{
	bool const is_actually_visible			= !check_if_in_blind_zones( &ai_game_object );
	for ( sensed_visual_object*	it_object = m_visible_objects.front(); it_object; it_object = m_visible_objects.get_next_of_object( it_object ) )
	{
		if ( it_object->object == &ai_game_object && is_actually_visible )
		{
			it_object->is_in_frustum		= true;
			return;
		}
	}
	npc const* const testing_npc			= ai_game_object.cast_npc();
	if ( ( testing_npc && testing_npc != &m_npc || !testing_npc ) && is_actually_visible )
		add_new_visible_object				( &ai_game_object );
}

void vision_sensor::trace_objects_in_frustum	( )
{
	for ( sensed_visual_object*	it_object = m_visible_objects.front(); it_object; it_object = m_visible_objects.get_next_of_object( it_object ) )
		if ( !it_object->was_visible_last_time )
			it_object->local_point				= it_object->object->get_random_surface_point( m_world.get_current_time_in_ms() );
}

} // namespace sensors
} // namespace ai
} // namespace xray