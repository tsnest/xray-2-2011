////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_ANIMATION_EVENT_ITERATOR_H_INCLUDED
#define MIXING_N_ARY_TREE_ANIMATION_EVENT_ITERATOR_H_INCLUDED

#include "mixing_animation_event.h"

namespace xray {
namespace animation {

struct subscribed_channel;

namespace mixing {

struct animation_state;
class n_ary_tree_animation_node;
class animation_interval;
class channel_event_callback_base;

class XRAY_ANIMATION_API n_ary_tree_animation_event_iterator {
public:
			n_ary_tree_animation_event_iterator		(
								animation_state& animation_state,
								n_ary_tree_animation_node& animation_node,
								u32 start_time_in_ms,
								u32 initial_event_types,
								subscribed_channel*& channels_head
							);
	inline	n_ary_tree_animation_event_iterator		( );
	n_ary_tree_animation_event_iterator& operator ++( );
	inline	animation_event operator *				( );
	inline	bool			is_equal				( n_ary_tree_animation_event_iterator const& other ) const;

private:
			void			dispatch_callback		(
								channel_event_callback_base const& callback,
								u8 domain_data
							) const;
			void			dispatch_callbacks		(
								animation_interval const& interval,
								float target_time
							) const;
			u32				get_time_in_ms			(
								u32 start_time_in_ms,
								float time_from_interval_start,
								float& event_time,
								u32& event_type
							);
			float	get_nearest_animation_interval_event_time (
								animation_interval const& interval,
								float start_time,
								float target_time,
								u32& event_type
							);
			void			advance					( u32 initial_event_types );

private:
	animation_event				m_value;
	n_ary_tree_animation_node*	m_animation;
	subscribed_channel**		m_channels_head;
}; // struct n_ary_tree_animation_event_iterator

	inline	bool			operator ==			( n_ary_tree_animation_event_iterator const& left, n_ary_tree_animation_event_iterator const& right );
	inline	bool			operator !=			( n_ary_tree_animation_event_iterator const& left, n_ary_tree_animation_event_iterator const& right );

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_n_ary_tree_animation_event_iterator_inline.h"

#endif // #ifndef MIXING_N_ARY_TREE_ANIMATION_EVENT_ITERATOR_H_INCLUDED