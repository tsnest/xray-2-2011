////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_WEIGHT_EVENT_ITERATOR_H_INCLUDED
#define MIXING_N_ARY_TREE_WEIGHT_EVENT_ITERATOR_H_INCLUDED

#include "mixing_animation_event.h"

namespace xray {
namespace animation {
namespace mixing {

class n_ary_tree_animation_node;

class XRAY_ANIMATION_API n_ary_tree_weight_event_iterator {
public:
			explicit	n_ary_tree_weight_event_iterator	(
							n_ary_tree_animation_node& animation,
							u32 const start_time_in_ms,
							u32 const initial_event_types
						);
	inline				n_ary_tree_weight_event_iterator	( );
			n_ary_tree_weight_event_iterator&	operator ++	( );
	inline	animation_event						operator *	( );
	inline	bool								is_equal	( n_ary_tree_weight_event_iterator const& other ) const;

private:
	n_ary_tree_animation_node*	m_animation;
	u32							m_time_in_ms;
	u32							m_event_type;
}; // class n_ary_tree_weight_event_iterator

	inline	bool								operator ==	( n_ary_tree_weight_event_iterator const& left, n_ary_tree_weight_event_iterator const& right );
	inline	bool								operator !=	( n_ary_tree_weight_event_iterator const& left, n_ary_tree_weight_event_iterator const& right );

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_weight_event_iterator_inline.h"

#endif // #ifndef MIXING_N_ARY_TREE_WEIGHT_EVENT_ITERATOR_H_INCLUDED