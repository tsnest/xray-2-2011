////////////////////////////////////////////////////////////////////////////
//	Created		: 22.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BEHAVIOUR_H_INCLUDED
#define BEHAVIOUR_H_INCLUDED

#include "vision_sensor_parameters.h"
#include "interaction_sensor_parameters.h"
#include "damage_sensor_parameters.h"
#include "hearing_sensor_parameters.h"
#include "smell_sensor_parameters.h"
#include "pre_perceptors_filter.h"
#include <xray/ai/animation_item.h>
#include <xray/ai/sound_item.h>
#include <xray/ai/movement_target.h>
#include "base_filter.h"
#include "goal.h"

namespace xray {
namespace ai {

namespace planning {
	class pddl_domain;
	class pddl_problem;
	class action_parameter;
} // namespace planning

class ai_world;

class behaviour :
	public resources::unmanaged_resource,
	private boost::noncopyable
{
public:
					behaviour						(
						configs::binary_config_value const& general_options,
						configs::binary_config_value const& behaviour_options,
						ai_world& world,
						u32 const animations_count,
						u32 const sounds_count,
						u32 const movement_targets_count
					);
					~behaviour						( );

	inline	vision_sensor_parameters const&			get_vision_sensor_parameters		( ) const { return m_vision_parameters; }
	inline	interaction_sensor_parameters const&	get_interaction_sensor_parameters	( ) const { return m_interaction_parameters; }
	inline	damage_sensor_parameters const&			get_damage_sensor_parameters		( ) const { return m_damage_parameters; }
	inline	hearing_sensor_parameters const&		get_hearing_sensor_parameters		( ) const { return m_hearing_parameters; }
	inline	smell_sensor_parameters const&			get_smell_sensor_parameters			( ) const { return m_smell_parameters; }
	inline	planning::goal const*					get_goals							( )		  { return m_goals.front(); }
	inline	planning::pddl_domain const&			get_domain							( ) const { return *m_domain; }
	inline	planning::pddl_problem const&			get_problem							( )		  { return *m_problem; }
	inline	pre_perceptors_filter&					get_ignorance_filter				( )		  { return m_ignorance_filter; }
	inline	u32										get_animations_count				( ) const { return m_animations_count; }
	inline	u32										get_sounds_count					( ) const { return m_sounds_count; }
	inline	u32										get_movement_targets_count			( ) const { return m_movement_targets_count; }
	inline	animation_item const*					get_animations						( ) const { return pointer_cast< animation_item const* >( pointer_cast< pcbyte >( this ) + sizeof( *this ) ); }
	inline	sound_item const*						get_sounds							( ) const { return pointer_cast< sound_item const* >( pointer_cast< pcbyte >( get_animations() ) + m_animations_count * sizeof( animation_item ) ); }
	inline	movement_target const*					get_movement_targets				( ) const { return pointer_cast< movement_target const* >( pointer_cast< pcbyte >( get_sounds() ) + m_sounds_count * sizeof( sound_item ) ); }

			void									get_available_animations			( animation_items_type& destination ) const;
			void									get_available_sounds				( sound_items_type& destination ) const;
			void									get_available_movement_targets		( movement_targets_type& destination ) const;
			animation_item const*					find_animation_by_filename			( pcstr animation_filename ) const;

	static	void	fill_goal_filter_sets	( configs::binary_config_value const& options, ai_world& world, planning::goal& owner );
	static	void	fill_action_filter_sets	( configs::binary_config_value const& options, ai_world& world, planning::action_instance& owner );
	static	void	fill_action_parameter	( configs::binary_config_value const& options, planning::action_parameter& parameter );

private:
	typedef intrusive_list<	planning::goal,
							planning::goal*,
							&planning::goal::m_next,
							threading::single_threading_policy,
							size_policy >	goals_type;
			
private:
			void	deserialize_parameters	( configs::binary_config_value const& options );
			void	create_domain			( configs::binary_config_value const& options, ai_world& world );
			void	create_problem			( configs::binary_config_value const& options, ai_world& world );
			void	create_goals			( configs::binary_config_value const& options, ai_world& world );
			void	fill_ignorance_filter	( configs::binary_config_value const& options, ai_world& world );
			void	sort_goals				( );

	static	void	delete_filter			( planning::base_filter* filter_to_be_deleted );
	static	void delete_parameters_filters	( planning::parameters_filters_type* filters );

private:
	vision_sensor_parameters				m_vision_parameters;
	interaction_sensor_parameters			m_interaction_parameters;
	damage_sensor_parameters				m_damage_parameters;
	hearing_sensor_parameters				m_hearing_parameters;
	smell_sensor_parameters					m_smell_parameters;

	pre_perceptors_filter					m_ignorance_filter;

	planning::pddl_domain*					m_domain;
	planning::pddl_problem*					m_problem;
	goals_type								m_goals;

	u32										m_animations_count;
	u32										m_sounds_count;
	u32										m_movement_targets_count;
}; // class behaviour

typedef resources::resource_ptr< behaviour, resources::unmanaged_intrusive_base > behaviour_ptr;

} // namespace ai
} // namespace xray

#endif // #ifndef BEHAVIOUR_H_INCLUDED
