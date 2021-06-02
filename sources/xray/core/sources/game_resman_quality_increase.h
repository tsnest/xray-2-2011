////////////////////////////////////////////////////////////////////////////
//	Created		: 03.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_QUALITY_INCREASE_H_INCLUDED
#define GAME_RESMAN_QUALITY_INCREASE_H_INCLUDED

#include "game_resman_data.h"

namespace xray {
namespace resources {

class quality_increase_functionality : public core::noncopyable
{
public:
	quality_increase_functionality								(game_resources_manager_data & data);
	
	void	tick												();
	void	erase_from_increase_quality_tree					(resource_base * );
	void	insert_to_increase_quality_tree						(resource_base * );
	void	update_current_satisfaction_for_resource			(resource_base * );
	
private:
	typedef	intrusive_list<resource_quality, resource_base *, & resource_quality::m_next_in_increase_quality_queue, threading::single_threading_policy >
			increase_quality_queue;

	void	update_current_satisfaction							();
	void		update_current_satisfaction_for_memory_type		(memory_type * memory_type_resources);

	void	select_to_increase_quality							(increase_quality_queue * );
	resource_base * find_next_to_select_to_increase_quality		();
	void			select_resource_to_increase_quality			(resource_base * , increase_quality_queue *	);

	void	schedule_to_increase_quality						(increase_quality_queue & );
	void	log_increase_quality_queue							();

private:
	game_resources_manager_data &				m_data;
	static u32 const							tick_period	=	300;
	static float								s_elapsed_sec_from_start;
	static bool									s_started_tick_timer;
	static timing::timer						s_tick_timer;
};

} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_QUALITY_INCREASE_H_INCLUDED