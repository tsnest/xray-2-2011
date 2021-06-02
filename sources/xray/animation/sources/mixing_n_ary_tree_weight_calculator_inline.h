////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_WEIGHT_CALCULATOR_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_WEIGHT_CALCULATOR_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_weight_calculator::n_ary_tree_weight_calculator	( u32 const current_time_in_ms ) :
	m_result			( 0 ),
	m_current_time_in_ms( current_time_in_ms ),
	m_weight			( 0.f ),
	m_null_weight_found	( false ),
	m_weight_transition_ended_time_in_ms( 0 )
{
}

inline float n_ary_tree_weight_calculator::weight					( ) const
{
	return				m_weight;
}

inline bool n_ary_tree_weight_calculator::null_weight_found			( ) const
{
	return				m_null_weight_found;
}

inline u32 n_ary_tree_weight_calculator::weight_transition_ended_time_in_ms( ) const
{
	return				m_weight_transition_ended_time_in_ms;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_WEIGHT_CALCULATOR_INLINE_H_INCLUDED