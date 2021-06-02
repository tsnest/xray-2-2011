////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PRE_PERCEPTORS_FILTER_H_INCLUDED
#define PRE_PERCEPTORS_FILTER_H_INCLUDED

#include <xray/ai/filter_types.h>
#include "base_filter.h"

namespace xray {
namespace ai {

class pre_perceptors_filter : private boost::noncopyable
{
public:
			pre_perceptors_filter	( );

	void	ignore					( ignorable_game_object const* begin, ignorable_game_object const* end );
	bool	is_object_ignored		( game_object const* const object ) const;
	void	on_hit_event			( game_object const* const hitting_object );
	
	void							add_aux_filter		( planning::base_filter* const aux_filter );
	planning::base_filter*			pop_aux_filter		( );
	
private:
	typedef fixed_vector< ignorable_game_object, 16 >	ignorable_objects;
	typedef intrusive_list< planning::base_filter,
							planning::base_filter*,
							&planning::base_filter::m_next
						  >								aux_filters_type;

private:
	ignorable_game_object*			find_ignored_object	( game_object const* const object );
	ignorable_game_object const*	find_ignored_object	( game_object const* const object ) const;
	void							stop_ignoring		( game_object const* const object );
	
	ignorable_objects				m_ignorable_objects;
	aux_filters_type				m_aux_filters;
}; // class pre_perceptors_filter

} // namespace ai
} // namespace xray

#endif // #ifndef PRE_PERCEPTORS_FILTER_H_INCLUDED