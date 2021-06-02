////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TRANSITION_TREE_CONSTRUCTOR_IMPL_H_INCLUDED
#define MIXING_N_ARY_TREE_TRANSITION_TREE_CONSTRUCTOR_IMPL_H_INCLUDED

#include "mixing_n_ary_tree_transition_to_one_detector.h"
#include "mixing_animation_state.h"

namespace xray {
namespace animation {
namespace mixing {

u32 computed_operands_count									( n_ary_tree_animation_node& from_animation, n_ary_tree_animation_node& to_animation, bool skip_time_scale_node );

inline bool is_asynchronous_animation						( n_ary_tree_animation_node& animation )
{
	return										animation.synchronization_group_id() == u32( -1 );
}

inline n_ary_tree_animation_node* find_animation			(
		n_ary_tree_animation_node* const begin,
		n_ary_tree_animation_node* const end,
		n_ary_tree_animation_node& animation_to_find
	)
{
	for ( n_ary_tree_animation_node* i = begin; i != end; i = i->m_next_animation ) {
		if ( (*i).animation_intervals_count() != animation_to_find.animation_intervals_count() )
			continue;

		bool equal							= true;
		animation_interval const* j			= (*i).animation_intervals();
		animation_interval const* const e	= (*i).animation_intervals() + (*i).animation_intervals_count();
		animation_interval const* k			= animation_to_find.animation_intervals();
		for ( ; j != e; ++j, ++k ) {
			if ( *j != *k ) {
				equal						= false;
				break;
			}
		}

		if ( !equal )
			continue;

		if ( animation_to_find.override_existing_animation() ) {
			if ( animation_to_find.animation_state().animation_interval_id != (*i).animation_state().animation_interval_id )
				continue;

			if ( animation_to_find.animation_state().animation_interval_time != (*i).animation_state().animation_interval_time )
				continue;
		}

		return								i;
	}

	return										0;
}

inline n_ary_tree_animation_node* synchronization_group_end	( n_ary_tree_animation_node* const driving_animation )
{
	R_ASSERT									( driving_animation );
	n_ary_tree_animation_node* i			= driving_animation;
	u32 const current_synchronization_group_id	= driving_animation->synchronization_group_id();
	for (; i && (*i).synchronization_group_id() == current_synchronization_group_id; i = i->m_next_animation ) ;
	return										i;
}

inline void get_next_synchronization_group		(
		n_ary_tree_animation_node*& begin,
		n_ary_tree_animation_node*& end
	)
{
	begin										= end;
	if ( begin )
		end										= synchronization_group_end( begin );
}

inline comparison_result_enum compare_animation_intervals	( n_ary_tree_animation_node const& left, n_ary_tree_animation_node const& right )
{
	if ( left.animation_intervals_count() < right.animation_intervals_count() )
		return									less;

	if ( right.animation_intervals_count() < left.animation_intervals_count() )
		return									more;

	if ( left.start_cycle_animation_interval_id() < right.start_cycle_animation_interval_id() )
		return									less;

	if ( right.start_cycle_animation_interval_id() < left.start_cycle_animation_interval_id() )
		return									more;

	animation_interval const* i					= left.animation_intervals();
	animation_interval const* const e			= left.animation_intervals() + left.animation_intervals_count();
	animation_interval const* j					= right.animation_intervals();
	for ( ; i != e; ++i, ++j ) {
		if ( *i < *j )
			return								less;

		if ( *i > *j )
			return								more;
	}

	return										equal;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TRANSITION_TREE_CONSTRUCTOR_IMPL_H_INCLUDED