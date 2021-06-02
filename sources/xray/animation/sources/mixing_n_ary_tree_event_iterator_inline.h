////////////////////////////////////////////////////////////////////////////
//	Created		: 26.09.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_EVENT_ITERATOR_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_EVENT_ITERATOR_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_event_iterator::n_ary_tree_event_iterator				( ) :
	m_animation_node			( 0 ),
	m_state						( target_time_reached_state ),
	m_value						( u32(-1), 0 )
{
}

inline animation_event n_ary_tree_event_iterator::operator*				( ) const
{
	return						m_value;
}

inline animation_event* n_ary_tree_event_iterator::operator ->			( )
{
	return						&m_value;
}

inline animation_event const* n_ary_tree_event_iterator::operator ->	( ) const
{
	return						&m_value;
}

inline bool n_ary_tree_event_iterator::are_there_any_weight_transitions	( ) const
{
	return						m_weight_event_iterator != n_ary_tree_weight_event_iterator();
}

inline bool n_ary_tree_event_iterator::is_equal							( n_ary_tree_event_iterator const& other ) const
{
	if ( (m_state == target_time_reached_state) && (m_state == other.m_state)  )
		return					true;

	return
		( m_state == other.m_state ) &&
		( m_value == other.m_value ) &&
		( m_animation_event_iterator == other.m_animation_event_iterator ) &&
		( m_weight_event_iterator == other.m_weight_event_iterator );
}

inline n_ary_tree_animation_node& n_ary_tree_event_iterator::animation	( ) const
{
	R_ASSERT					( m_animation_node );
	return						*m_animation_node;
}

inline bool operator ==													( n_ary_tree_event_iterator const& left, n_ary_tree_event_iterator const& right )
{
	return						left.is_equal( right );
}

inline bool operator !=													( n_ary_tree_event_iterator const& left, n_ary_tree_event_iterator const& right )
{
	return						!(left == right);
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_EVENT_ITERATOR_INLINE_H_INCLUDED