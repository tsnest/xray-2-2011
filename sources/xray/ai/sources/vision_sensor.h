////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VISION_SENSOR_H_INCLUDED
#define VISION_SENSOR_H_INCLUDED

#include "active_sensor_base.h"
#include <xray/ai/sensed_visual_object.h>
#include "game_object_subscriber.h"
#include "vision_sensor_parameters.h"

namespace xray {
namespace ai {

typedef vectora< game_object const* > ai_objects_type;

namespace sensors {

class vision_sensor : public active_sensor_base
{
public:
						vision_sensor			(
							npc& npc,
							ai_world& world,
							brain_unit& brain
						);
	virtual				~vision_sensor			( );

	virtual	void		tick					( );	
	virtual void		set_parameters			( behaviour const& behaviour_parameters );
	virtual	void		dump_state				( npc_statistics& stats ) const;

			void		add_fact				( sensed_visual_object* visual_object ) const;

			void		on_object_destruction	( game_object const& destroyed_object );

public:
	typedef intrusive_list< sensed_visual_object,
						sensed_visual_object*,
						&sensed_visual_object::next,
						threading::single_threading_policy,
						size_policy
					  >							visible_objects;

private:
			void		update_visible_objects	( );
			void		update_visibility_value	( );
			void		check_frustum			( );
			void		delete_not_in_frustum 	( );
			void		update_perceptors		( ) const;
			void		remove_invisible_objects( );
			void		reset_frustum_status	( );
			void		update_frustum_objects	( game_object const& ai_game_object );
			void		trace_objects_in_frustum( );
			bool		check_if_in_blind_zones	( game_object const* const ai_game_object ) const;
			void		add_new_visible_object	( game_object const* const ai_game_object );

	static	sensed_visual_object*	find_visual_object_in_list	(
										game_object const* const ai_game_object,
										visible_objects& objects_list
									);
	inline vision_sensor*			return_this	( ) { return this; }

private:
	visible_objects					m_visible_objects;
	game_object_subscriber			m_subscription;
	vision_sensor_parameters		m_parameters;
	u32								m_last_tick;
}; // class vision_sensor

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef VISION_SENSOR_H_INCLUDED