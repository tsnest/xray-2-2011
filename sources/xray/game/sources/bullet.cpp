////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include "bullet.h"
#include "bullet_manager.h"
#include "game.h"
#include "game_world.h"
#include "collision_object_types.h"

#include <xray/collision/common_types.h>
#include <xray/collision/space_partitioning_tree.h>

namespace stalker2 {

///////////////////////				A D D I T I O N A L   D A T A 				    ///////////////////////////////////////
struct game_material
{
	game_material():
		material_strength_coefficient	( 500.0f ),
		material_resistance				( 50 ),
		bullet_reflection_epsilon		( 0.1f ),
		bullet_reflection_speed_down	( 9.f )
		{}

	float			material_strength_coefficient;
	float			material_resistance;
	float			bullet_reflection_epsilon;
	float			bullet_reflection_speed_down;
};
static game_material m_collided_material;

///////////////////////			F O R W A R D   D E C L A R A T I O N S				///////////////////////////////////////

enum triangle_orientation
{
	triangle_orientation_front_face,
	triangle_orientation_back_face,
};

enum collision_result
{
	collision_result_no_collision,
	collision_result_collide,
	collision_result_pierced,
	collision_result_reflected,
};

static float3 get_triangle_normal ( collision::ray_triangle_result const& result );

/////////////////////////////			   I N I T I A L I Z E				///////////////////////////////////////

bullet::bullet ( float3 const& position, float3 const& velocity, float born_time, float air_resistance ):
	m_position					( position ),
	m_velocity					( velocity ),
	m_start_position			( position ),
	m_start_velocity			( velocity ),

	m_enter_collision_point		( float3( ) ),
	m_inside_collision_object	( NULL ),

	m_born_time					( born_time ),
	m_life_time					( 0 ),
	m_air_resistance			( air_resistance ),
	m_current_resistance		( air_resistance ),
	m_max_distance				( 1000.0f ),
	m_fly_distance				( 0.0f ),

	m_importance				( 0 ),
	m_change_trajectory_count	( 0 )
{
}

bullet::bullet ( bullet const& other )
{
	m_position					= other.m_position;
	m_velocity					= other.m_velocity;
	m_start_position			= other.m_position;
	m_start_velocity			= other.m_velocity;

	m_enter_collision_point		= other.m_enter_collision_point;
	m_inside_collision_object	= other.m_inside_collision_object;

	m_born_time					= other.m_born_time;
	m_air_resistance			= other.m_air_resistance;
	m_current_resistance		= other.m_current_resistance;
	m_max_distance				= other.m_max_distance;
	m_fly_distance				= other.m_fly_distance;

	m_importance				= other.m_importance;
	m_change_trajectory_count	= other.m_change_trajectory_count;
}
bullet::~bullet ( )
{
}

/////////////////////////////		   P U B L I C   M E T H O D S			///////////////////////////////////////

void bullet::tick ( bullet_manager& bullet_manager, float elapsed_game_seconds )
{
	const float3	zero_velocity		= float3( 0, 0, 0 );

	float	low_time					= m_life_time;
	float	high_time					= ( elapsed_game_seconds - m_born_time ) * bullet_manager.get_bullet_time_factor( );

	float3 const& gravity				= bullet_manager.get_gravity( );

	for ( ;; )
	{
		if ( m_velocity.squared_length( ) < 1.f )
		{
			m_start_velocity = zero_velocity;
			return;
		}

		if ( m_change_trajectory_count >= 32 )
		{
			m_start_velocity = zero_velocity;
			return;
		}
		
		if( low_time == high_time )
			return;

		float	time					= pick_next_permissible_time( low_time, high_time, gravity );

		if( low_time == time )
			return;

		collision_result	result	= check_collision( bullet_manager, m_position, low_time, time );
		if ( result == collision_result_collide )
		{
			m_start_velocity = zero_velocity;
			return;
		}

		if ( result == collision_result_pierced || result == collision_result_reflected )
		{
			low_time	= m_life_time;
			high_time	-= time;
			time		= m_life_time;
		}	
		if	( time != 0 )
			if ( !update_bullet_position( time, gravity ) )
			{
				m_start_velocity = zero_velocity;
				return;
			}

		if ( math::is_similar( time, high_time ) )
			return;

		R_ASSERT			( low_time < high_time, "start_low[%f] high_time[%f]", low_time, high_time );
		low_time			= time;
		R_ASSERT			( low_time < high_time, "start_low[%f] high[%f]", low_time, high_time );
	}
}

bool bullet::is_finish_flying ( ) const
{
	const float3 zero_velocity = float3( 0, 0, 0 );

	return m_start_velocity == zero_velocity;
}

float3 const& bullet::get_start_position ( ) const
{
	return m_start_position;
}
float3 const& bullet::get_start_velocity ( ) const
{
	return m_start_velocity;
}

float3 const& bullet::get_position ( ) const
{
	return m_position;
}
float3 const& bullet::get_velocity ( ) const
{
	return m_velocity;
}

/////////////////////////////		   D E B U G   M E T H O D S			///////////////////////////////////////

#ifndef MASTER_GOLD

float3				bullet::evaluate_position				( float time, float& fall_down_time, float3 const& gravity )
{
	float3 position		= compute_trajectory_position( time, gravity );

	float const parabolic_time		= get_parabolic_time( );
	fall_down_time					= time - parabolic_time;

	return position;
}

float				bullet::get_max_time					( float3 const& gravity )
{
	float ret_val			= 0;
	pick_permissible_range	( ret_val, 0, 10, gravity );

	return ret_val;
}

#endif // #ifndef MASTER_GOLD


/////////////////////////////		  P R I V A T E   M E T H O D S			///////////////////////////////////////

float3 bullet::compute_parabolic_velocity ( float time, float3 const& gravity )
{
	float3 const xz_velocity	= float3( m_start_velocity.x, 0.f, m_start_velocity.z );
	// this could be since we could fire in different directions
	// for example, vertically into the ground
	if ( math::is_zero( xz_velocity.squared_length( ) ) )
		return					m_start_velocity + gravity * time;

	return						m_start_velocity * math::max( 0.f, 1.f - m_current_resistance * time ) + ( gravity * time );
}

float3 bullet::compute_trajectory_velocity ( float time, float3 const& gravity )
{
	float const parabolic_time	= get_parabolic_time( );
	float const	fall_down_time	= time - parabolic_time;

	if ( fall_down_time < 0.f )
		return					compute_parabolic_velocity ( time, gravity );

	float3 const& parabolic_vel	= compute_parabolic_velocity( parabolic_time, gravity );
	return						parabolic_vel + gravity * fall_down_time;
}

float3 bullet::compute_parabolic_position ( float time, float3 const& gravity )
{
	float3 const xz_velocity	= float3( m_start_velocity.x, 0.f, m_start_velocity.z );
	if ( math::is_zero( xz_velocity.squared_length( ) ) )
		return				m_start_position + m_start_velocity * time + gravity * ( math::sqr( time ) * .5f );

	float const sqr_t_div_2	= math::sqr( time ) * .5f;
	return m_start_position + m_start_velocity * time 
		+ m_start_velocity * -m_current_resistance * sqr_t_div_2
		+ gravity * sqr_t_div_2;
}

float3 bullet::compute_trajectory_position ( float time, float3 const& gravity )
{
	float const parabolic_time	= get_parabolic_time( );
	float const	fall_down_time	= time - parabolic_time;

	if ( fall_down_time < 0.f )
		return					compute_parabolic_position( time, gravity );

	float3 const& parabolic_pos	= compute_parabolic_position( parabolic_time, gravity );
	float3 const& parabolic_vel	= compute_parabolic_velocity( parabolic_time, gravity );

	return parabolic_pos + parabolic_vel * fall_down_time + gravity * ( math::sqr( fall_down_time ) * .5f );
}

float bullet::get_parabolic_time ( )
{
	static const float air_resistance_epsilon = 1.1f;
	return math::max( 0.f, 1.f / ( m_current_resistance * air_resistance_epsilon ) );
}

float bullet::pick_next_permissible_time ( float const low_time, float high_time, float3 const& gravity )
{
	R_ASSERT	( low_time < high_time, "low_time[%f] high_time[%f]", low_time, high_time );
	float const start_high_time	= high_time;
	if ( pick_permissible_range( high_time, low_time, high_time, gravity ) )
	{
		if ( high_time <= start_high_time )
			return			high_time;

		return				start_high_time;
	}

	float					low			= low_time;
	float					check_time	= high_time;
	float const				epsilon		= .05f;
	while ( !math::is_similar( low, high_time, math::epsilon_5 ) )
	{
		float				distance = compute_max_error( low_time, check_time, gravity );
		
		if ( distance < epsilon )
			low				= check_time;
		else
			high_time		= check_time;

		check_time			= ( low + high_time ) * .5f;
	}
	
	R_ASSERT				( low <= start_high_time, "low[%f], high[%f]", low, start_high_time );
	return					( low );
}

bool bullet::pick_permissible_range ( float& result, float const low_time, float const high_time, float3 const& gravity )
{
//	R_ASSERT				( m_max_distance - m_fly_distance > 0.0f );

	float const	parabolic_time	= get_parabolic_time( );

	if ( low_time > parabolic_time )
	{
		result				= get_check_time_in_vacuum( low_time, high_time, gravity );
		return				true;
	}

	if ( high_time  < parabolic_time )
	{
		result				= get_check_time( low_time, high_time, gravity );
		return				false;
	}

	if ( !math::is_similar( parabolic_time, low_time ) )
	{
		result				= get_check_time( low_time, parabolic_time, gravity );
		return				false;
	}

	result					= get_check_time_in_vacuum( parabolic_time, high_time, gravity );
	return					false;
}

float bullet::get_check_time_in_vacuum ( float start_low, float const high, float3 const& gravity )
{
	float const max_test_distance	= m_max_distance - m_fly_distance;
	float const time_delta	= high - start_low;
	float const time_to_fly	= ( m_start_velocity * time_delta + gravity * ( math::sqr( time_delta ) * .5f ) ).length( );
	if ( time_to_fly <= max_test_distance )
		return				high;

	float const fall_down_velocity_magnitude = compute_trajectory_velocity( start_low, gravity ).length( );
	float const positive_gravity	= -gravity.y;
	float time				=	( math::sqrt(
									math::sqr( fall_down_velocity_magnitude ) + 2.f * max_test_distance * positive_gravity )
									- fall_down_velocity_magnitude )
								/ positive_gravity;
	R_ASSERT				( time >= 0.f );

	R_ASSERT				( high >= start_low );
	float result			= start_low + time;
	math::clamp				( result, start_low, high );
	R_ASSERT				( result <= high, "result[%f], high[%f], start_low[%f], air_resistance[%f]", result, high, start_low, m_current_resistance );

	return					result;
}

inline static float get_max_error_time ( float const t0, float const t1 )
{
	return					( ( t1 + t0 ) * .5f );
	// this is correct even in our case
	// y(t) = V0y*t - V0y*ar*t^2/2 - g*t^2/2
	// x(t) = V0x*t - V0x*ar*t^2/2
}

float bullet::get_check_time ( float const start_low, float high, float3 const& gravity )
{
	float const max_test_distance	= m_max_distance - m_fly_distance;
	float3 const start		= compute_trajectory_position( start_low, gravity );
#ifndef MASTER_GOLD
	float const start_high	= high;
#endif // #ifndef MASTER_GOLD
	float					low = start_low;
	float					check_time = high;
	while ( !math::is_similar( low, high ) ) {
		float3 const intermediate	= compute_trajectory_position( get_max_error_time( start_low, check_time ), gravity );
		float3 const target			= compute_trajectory_position( check_time, gravity );
		float const distance		= ( intermediate - start ).length( ) + ( intermediate - target ).length( );
		if ( distance < max_test_distance )
			low				= check_time;
		else
			high			= check_time;

		check_time			= ( low + high ) * .5f;
	}

	R_ASSERT				( low <= start_high );
	return					low;
}

float bullet::compute_max_error ( float const low, float const high, float3 const& gravity )
{
	float					max_error_time = get_max_error_time( low, high );
	
	float3 const start		= compute_trajectory_position( low, gravity );
	float3 const target		= compute_trajectory_position( high, gravity );
	float3 const max_error	= compute_trajectory_position( max_error_time, gravity );

	float3	start_to_max_error	= max_error - start;
	float	magnitude			= start_to_max_error.length( );
	start_to_max_error			*= 1.f / magnitude;
	float3	start_to_target		= ( target - start ).normalize( );
	float	cosine_alpha		= math::max( -1.0f, math::min( start_to_max_error | start_to_target, 1.0f ) );
	float	sine_alpha			= math::sqrt( 1.f - math::sqr( cosine_alpha ) );

	return	magnitude * sine_alpha;
}

static bool collision_predicate ( xray::collision::ray_triangle_result const& result )
{
	XRAY_UNREFERENCED_PARAMETER( result );
	return true;
}

collision_result bullet::check_collision (	bullet_manager& bullet_manager,
											float3 start_position,
											float start_time,
											float current_time
										 )
{
	collision_result result				= collision_result_no_collision;

	for ( ;; ) {
		float3 const& target_position	= compute_trajectory_position( current_time, bullet_manager.get_gravity( ) );

		R_ASSERT						( start_time < current_time, "start time can not be greater than current time" );

		float3 direction				= target_position - start_position;
		float const distance			= direction.length( );
		if ( math::is_zero( distance ) )
			return						collision_result_no_collision;

		direction						*= 1.f / distance;
		
		collision::ray_triangles_type triangles				= collision::ray_triangles_type( g_allocator );
		collision::space_partitioning_tree * const tree		= bullet_manager.get_game( ).get_game_world( ).get_collision_tree( );

		if ( !tree )
			return						collision_result_no_collision;

		bool const does_ray_intersect_something =
			tree->ray_query(
				collision_object_static_model,
				start_position,
				direction,
				distance,
				triangles,
				collision::triangles_predicate_type( &collision_predicate )
			);

		if ( !does_ray_intersect_something )
			break;

		bool const need_new_collision_check	=
			process_ray_query(
				bullet_manager,
				triangles,
				distance,
				start_position,
				direction,
				start_time,
				current_time,
				result
			);

		if ( !need_new_collision_check )
			break;
	}

	return result;
}

bool bullet::process_ray_query (	bullet_manager& bullet_manager,
									collision::ray_triangles_type const& triangles,
									float distance,
									float3& start_position,
									float3& direction,
									float& start_time,
									float& current_time,
									collision_result& result
								)
{
	u32 const triangles_count		= triangles.size( );
	for ( u32 i = 0; i < triangles_count; ++i )
	{
		float3		triangle_normal	= get_triangle_normal ( triangles[i] );
		float3		collide_point	= start_position + direction * triangles[i].distance;
		float		collision_time	= start_time + triangles[i].distance / distance * ( current_time - start_time );
		
		float		cos_alpha		= ( triangle_normal | direction );

		if( cos_alpha == 0 )
		{
#ifndef MASTER_GOLD
			bullet_manager.add_collision_point	( collide_point, math::color( 128, 128, 128, 128 ) );
#endif // #ifndef MASTER_GOLD
			continue;
		}

		triangle_orientation	orientation		= ( cos_alpha < 0 ) ? triangle_orientation_front_face : triangle_orientation_back_face;

		if( orientation == triangle_orientation_front_face && m_inside_collision_object != NULL )
			continue;

#ifdef STRICT_FOLLOW_TRAJECTORY
		fix_collision_point_and_time( collide_point, collision_time, start_time, current_time, orientation, triangle_normal, bullet_manager.get_gravity( ) );
#endif // #ifdef STRICT_FOLLOW_TRAJECTORY

		direction				= compute_trajectory_velocity( collision_time, bullet_manager.get_gravity( ) );
		float speed				= direction.length( );
		direction				/= speed;
		
		if( orientation == triangle_orientation_front_face )
		{
			if( m_inside_collision_object != NULL )
				continue;

			if ( cos_alpha >= - m_collided_material.bullet_reflection_epsilon )
			{
				if( try_reflect	( bullet_manager, collide_point, direction, triangle_normal, speed, collision_time, start_position, start_time, current_time, cos_alpha ) )
				{
					result	= collision_result_reflected;
					return	true;
				}

				result	= collision_result_collide;
				return	false;
			}

			collide_front_face	( bullet_manager, collide_point, direction, triangle_normal, speed, collision_time, start_position, start_time, current_time, triangles[i] );
			result				= collision_result_pierced;
			return true;
		}

		if ( m_inside_collision_object && triangles[i].object != m_inside_collision_object )
			continue;

		//R_ASSERT( m_inside_collision_object != NULL, "first collision triangle on bullet way, can not being back face" );
		if( m_inside_collision_object == NULL )
		{
			//m_bullet_manager.add_collision_point( collide_point, math::color( 255, 0, 255, 128 ) );
			continue;
		}
		
		if( triangles[i].object != m_inside_collision_object )
			continue;

		if( try_to_pierce ( bullet_manager, collide_point, direction, triangle_normal, speed, collision_time, start_position, start_time, current_time ) )
		{
			result	= collision_result_pierced;
			return	true;
		}

		result	= collision_result_collide;
		return	false;
	}
	return false;
}

static float3 get_triangle_normal ( collision::ray_triangle_result const& result )
{
	collision::collision_object const* gi = dynamic_cast<collision::collision_object const*>( result.object );
	if( NULL == gi )
		return float3( 0, 0, 0 );

	u32 triangle_id		= result.triangle_id;

	non_null< collision::geometry_instance const >::ptr geom = gi->get_geometry_instance	( );
	u32 const* indices								= geom->indices		( triangle_id );
	float3 verts[3];
	verts[0]			= geom->vertices( )[ indices[0] ];
	verts[1]			= geom->vertices( )[ indices[1] ];
	verts[2]			= geom->vertices( )[ indices[2] ];

	float4x4 const m	= gi->get_matrix( );
	verts[0]			= m.transform_position( verts[0] );
	verts[1]			= m.transform_position( verts[1] );
	verts[2]			= m.transform_position( verts[2] );

	return				( ( verts[1] - verts[0] ) ^ ( verts[2] - verts[1] ) ).normalize( );
}

void bullet::fix_collision_point_and_time (		float3& collide_point,
												float& collision_time,
												float const start_time,
												float const current_time,
												triangle_orientation const orientation,
												float3 const& triangle_normal,
												float3 const& gravity
											)
{
#ifndef MASTER_GOLD
	float3		dbg_new_collide_point	= compute_trajectory_position( start_time, gravity );
	float		dbg_delta				= ( ( dbg_new_collide_point - collide_point ) | triangle_normal );
	R_ASSERT	( ( orientation == triangle_orientation_front_face && dbg_delta >= 0 ) || 
					( orientation == triangle_orientation_back_face && dbg_delta <= 0 ) );
#endif // #ifndef MASTER_GOLD

	float3		new_collide_point	= compute_trajectory_position( collision_time, gravity );
	float		delta				= ( ( new_collide_point - collide_point ) | triangle_normal );

	if( !math::is_zero( delta ) )
	{
		float		low_time		= start_time;
		float		high_time		= current_time;

		while ( !math::is_zero( delta ) )
		{
			if (	( orientation == triangle_orientation_front_face && delta < 0 ) || 
					( orientation == triangle_orientation_back_face && delta > 0 ) )
				high_time			= collision_time;
			else
				low_time			= collision_time;

			collision_time		= ( low_time + high_time ) * .5f;

			new_collide_point	= compute_trajectory_position( collision_time, gravity );
			delta				= ( ( new_collide_point - collide_point ) | triangle_normal );
		}

		collide_point			= new_collide_point;
	}
}

void bullet::collide_front_face (	bullet_manager& bullet_manager,
									float3 const& collide_point,
									float3 const& direction,
									float3 const& triangle_normal,
									float const speed,
									float const collision_time,
									float3& start_position,
									float& start_time,
									float& current_time,
									collision::ray_triangle_result const& triangle )
{
	bullet_manager.add_decal	( collide_point, direction, triangle_normal, true );

	change_trajectory			( bullet_manager, collide_point, direction * speed, collision_time );
	m_current_resistance		= m_collided_material.material_resistance;
	m_inside_collision_object	= triangle.object;
	m_enter_collision_point		= collide_point;

#ifndef MASTER_GOLD

	bullet_manager.store_bullet_trajectory	( this );

#endif // #ifndef MASTER_GOLD

	start_position	= m_start_position;
	start_time		= m_life_time;
	current_time	-= collision_time;
}
bool bullet::try_reflect (	bullet_manager& bullet_manager,
							float3 const& collide_point,
							float3 direction,
							float3 const& triangle_normal,
							float speed,
							float const collision_time,
							float3& start_position,
							float& start_time,
							float& current_time,
							float const cos_alpha )
{
	speed					-=  m_collided_material.bullet_reflection_speed_down * -cos_alpha;
	if( speed < 0 )
	{
#ifndef MASTER_GOLD

		bullet_manager.add_collision_point	( collide_point, math::color( 255, 0, 0, 128 ) );

#endif // #ifndef MASTER_GOLD

		return false;
	}

	direction				= 2 * triangle_normal * -cos_alpha + direction;
	change_trajectory		( bullet_manager, collide_point, direction * speed, collision_time );

#ifndef MASTER_GOLD

	bullet_manager.store_bullet_trajectory	( this );
	bullet_manager.add_collision_point		( collide_point, math::color( 0, 255, 0, 128 ) );

#endif // #ifndef MASTER_GOLD

	start_position	= m_start_position;
	start_time		= m_life_time;
	current_time	-= collision_time;
	return true;
}
bool bullet::try_to_pierce (	bullet_manager& bullet_manager,
								float3 const& collide_point,
								float3 const& direction,
								float3 const& triangle_normal,
								float speed,
								float const collision_time,
								float3& start_position,
								float& start_time,
								float& current_time )
{
	float material_width	= ( collide_point - m_enter_collision_point ).length( );

	m_enter_collision_point		= float3( );
	m_inside_collision_object	= NULL;

	speed					-= m_collided_material.material_strength_coefficient * material_width;
	if( speed < 0 )
		return false;
	
	//barrier has pierced
	change_trajectory							( bullet_manager, collide_point, direction * speed, collision_time );

#ifndef MASTER_GOLD
	
	bullet_manager.store_bullet_trajectory	( this );

#endif // #ifndef MASTER_GOLD

	bullet_manager.add_decal					( collide_point, -direction, triangle_normal, false );

	start_position	= m_start_position;
	start_time		= m_life_time;
	current_time	-= collision_time;
	return true;
}
void bullet::change_trajectory ( bullet_manager& bullet_manager, float3 const& new_position, float3 const& new_velocity, float collision_time )
{
	++m_change_trajectory_count;
	m_start_position		= new_position;
	m_start_velocity		= new_velocity;
	m_position				= m_start_position;
	m_velocity				= m_start_velocity;
	m_current_resistance	= m_air_resistance;
	m_born_time				+= collision_time / bullet_manager.get_bullet_time_factor( );
	m_life_time				= 0;
}

bool bullet::update_bullet_position	( float const time, float3 const& gravity )
{
	float3 const new_position	= compute_trajectory_position( time, gravity );
	m_fly_distance				+= ( new_position - m_position ).length( );

	if ( m_fly_distance >= m_max_distance )
	{	
		m_fly_distance =		m_max_distance;
		return					false;
	}

	//if ( !m_game_world.aabb().contains( m_position ) )
	//	return false;

	m_velocity					= compute_trajectory_velocity( m_life_time, gravity );
	if ( math::is_zero( m_velocity.squared_length( ) ) )
		return					false;

	m_position					= new_position;
	m_life_time					= time;

	return						true;
}

} // namespace stalker2