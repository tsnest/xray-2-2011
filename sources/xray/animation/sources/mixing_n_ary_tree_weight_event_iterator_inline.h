////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_WEIGHT_EVENT_ITERATOR_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_WEIGHT_EVENT_ITERATOR_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_weight_event_iterator::n_ary_tree_weight_event_iterator				( ) :
	m_animation			( 0 ),
	m_time_in_ms		( u32(-1) ),
	m_event_type		( 0 )
{
}

inline animation_event n_ary_tree_weight_event_iterator::operator *						( )
{
	return				animation_event( m_time_in_ms, m_event_type);
}

inline bool n_ary_tree_weight_event_iterator::is_equal									(
		n_ary_tree_weight_event_iterator const& other
	) const
{
	return				(m_animation == other.m_animation) && (m_time_in_ms == other.m_time_in_ms) && (m_event_type == other.m_event_type);
}

inline bool operator ==	( n_ary_tree_weight_event_iterator const& left, n_ary_tree_weight_event_iterator const& right )
{
	return				left.is_equal( right );
}

inline bool operator != ( n_ary_tree_weight_event_iterator const& left, n_ary_tree_weight_event_iterator const& right )
{
	return				!(left == right);
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_WEIGHT_EVENT_ITERATOR_INLINE_H_INCLUDED