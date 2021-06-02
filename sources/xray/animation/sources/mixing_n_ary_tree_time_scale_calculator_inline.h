////////////////////////////////////////////////////////////////////////////
//	Created		: 27.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_SCALE_CALCULATOR_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_SCALE_CALCULATOR_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline float n_ary_tree_time_scale_calculator::time_scale						( ) const
{
	return						m_time_scale;
}

inline n_ary_tree_base_node* n_ary_tree_time_scale_calculator::result			( ) const
{
	return						m_result;
}

inline base_interpolator const* n_ary_tree_time_scale_calculator::interpolator	( ) const
{
	return						m_interpolator;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TIME_SCALE_CALCULATOR_INLINE_H_INCLUDED