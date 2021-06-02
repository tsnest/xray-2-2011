////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#define	STRICT_FOLLOW_TRAJECTORY

namespace xray {
namespace collision {

struct	ray_triangle_result;
class	object;

typedef vectora< ray_triangle_result >	ray_triangles_type;

} // namespace collision
} // namespace xray

namespace stalker2 {

class	bullet_manager;
enum	collision_result;
enum	triangle_orientation;

//enum bullet_state
//{
//	bullet_still_flying,
//	bullet_finish_flying
//}; // enum bullet_state

class bullet : private boost::noncopyable {
public:
						bullet							(
							float3 const& position,
							float3 const& velocity,
							float born_time,
							float air_resistance
						);
						bullet							( bullet const& other );
						~bullet							( );

	void				tick							( bullet_manager& bullet_manager, float elapsed_game_seconds );
	bool				is_finish_flying				( ) const;
	
	float3 const&		get_start_position				( ) const;
	float3 const&		get_start_velocity				( ) const;

	float3 const&		get_position					( ) const;
	float3 const&		get_velocity					( ) const;

	float				pick_next_permissible_time		( float low_time, float high_time, float3 const& gravity );
	
#ifndef MASTER_GOLD
	float3				evaluate_position				( float time, float& fall_down_time, float3 const& gravity );
	float				get_max_time					( float3 const& gravity );
#endif // #ifndef MASTER_GOLD

private:
	float3				compute_parabolic_velocity		( float time, float3 const& gravity );
	float3				compute_trajectory_velocity		( float time, float3 const& gravity );
	float3				compute_parabolic_position		( float time, float3 const& gravity );
	float3				compute_trajectory_position		( float time, float3 const& gravity );
	float				get_parabolic_time				( );

private:
	bool				pick_permissible_range			( float& result, float low_time, float high_time, float3 const& gravity );
	float				get_check_time					( float const start_low_time, float high_time, float3 const& gravity );
	float				get_check_time_in_vacuum		( float start_low_time, float high_time, float3 const& gravity );
	float				compute_max_error				( float low_time, float high_time, float3 const& gravity );
	collision_result	check_collision					(	bullet_manager& bullet_manager,
															float3 start_position,
															float start_time,
															float current_time
														);
	bool				process_ray_query				(	bullet_manager& bullet_manager,
															collision::ray_triangles_type const& triangles,
															float distance,
															float3& start_position,
															float3& direction,
															float& start_time,
															float& current_time,
															collision_result& result
														);
	void				fix_collision_point_and_time	(	float3& collide_point,
															float& collision_time,
															float start_time,
															float current_time,
															triangle_orientation const orientation,
															float3 const& triangle_normal,
															float3 const& gravity
														);
	void				collide_front_face				(	bullet_manager& bullet_manager,
															float3 const& collide_point,
															float3 const& direction,
															float3 const& triangle_normal,
															float speed,
															float collision_time,
															float3& start_position,
															float& start_time,
															float& current_time,
															collision::ray_triangle_result const& triangle
														);
	bool				try_reflect						(	bullet_manager& bullet_manager,
															float3 const& collide_point,
															float3 direction,
															float3 const& triangle_normal,
															float speed,
															float collision_time,
															float3& start_position,
															float& start_time,
															float& current_time,
															float cos_alpha
														);
	bool				try_to_pierce					(	bullet_manager& bullet_manager,
															float3 const& collide_point,
															float3 const& direction,
															float3 const& triangle_normal,
															float speed,
															float collision_time,
															float3& start_position,
															float& start_time,
															float& current_time																
														);
	void				change_trajectory				(	bullet_manager& bullet_manager,
															float3 const& new_position,
															float3 const& new_velocity,
															float collision_time
														);
	bool				update_bullet_position			( float time, float3 const& gravity );

private:
	typedef collision::object object;

private:
	float3				m_position;
	float3				m_velocity;
	float3				m_start_position;
	float3				m_start_velocity;
	float3				m_enter_collision_point;
	
private:
	object const*		m_inside_collision_object;

	float				m_born_time;
	float				m_life_time;
	float				m_air_resistance;
	float				m_current_resistance;
	float				m_max_distance;
	float				m_fly_distance;

	u16					m_importance;				//=0;
	u16					m_change_trajectory_count;	//=0;
}; // class bullet

} // namespace stalker2

#endif // #ifndef BULLET_H_INCLUDED