////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_EVENT_ITERATOR_H_INCLUDED
#define MIXING_N_ARY_TREE_EVENT_ITERATOR_H_INCLUDED

#include "mixing_animation_event.h"
#include "mixing_n_ary_tree_animation_event_iterator.h"
#include "mixing_n_ary_tree_weight_event_iterator.h"

namespace xray {
namespace animation {
namespace mixing {

struct animation_state;
class n_ary_tree_animation_node;

class XRAY_ANIMATION_API n_ary_tree_event_iterator {
public:
						n_ary_tree_event_iterator	(	
											animation_state& animation,
											n_ary_tree_animation_node& animation_node,
											u32 start_time_in_ms,
											u32 initial_event_types,
											subscribed_channel*& channels_head
										);
	inline				n_ary_tree_event_iterator	( );
			n_ary_tree_event_iterator&	operator ++	( );
	inline	animation_event				operator *	( ) const;
	inline	animation_event*			operator ->	( );
	inline	animation_event const*		operator ->	( ) const;
	inline	bool	are_there_any_weight_transitions( ) const;
	inline	bool						is_equal	( n_ary_tree_event_iterator const& other ) const;
			bool						is_less		( n_ary_tree_event_iterator const& other ) const;
	inline	n_ary_tree_animation_node&	animation	( ) const;

private:
			void						select_state( );

private:
	enum state {
		animation_event_state			= u32(1) << 0,
		weight_transition_event_state	= u32(1) << 1,
		target_time_reached_state		= u32(1) << 2,
	}; // enum iterator_state

private:
	n_ary_tree_animation_event_iterator		m_animation_event_iterator;
	n_ary_tree_weight_event_iterator		m_weight_event_iterator;
	animation_event							m_value;
	n_ary_tree_animation_node*				m_animation_node;
	u32										m_state;
}; // class n_ary_tree_event_iterator

	inline	bool						operator == ( n_ary_tree_event_iterator const& left, n_ary_tree_event_iterator const& right );
	inline	bool						operator != ( n_ary_tree_event_iterator const& left, n_ary_tree_event_iterator const& right );

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_event_iterator_inline.h"

#endif // #ifndef MIXING_N_ARY_TREE_EVENT_ITERATOR_H_INCLUDED