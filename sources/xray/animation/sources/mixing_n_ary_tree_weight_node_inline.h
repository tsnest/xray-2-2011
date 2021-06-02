////////////////////////////////////////////////////////////////////////////
//	Created		: 20.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_WEIGHT_NODE_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_WEIGHT_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_weight_node::n_ary_tree_weight_node				(
		base_interpolator const& interpolator,
		float const weight
	) :
	m_interpolator		( interpolator ),
	m_weight			( weight )
{
}

inline n_ary_tree_weight_node::n_ary_tree_weight_node				( n_ary_tree_weight_node const& other ) :
	n_ary_tree_base_node( other ),
	m_interpolator		( other.m_interpolator ),
	m_weight			( other.m_weight )
{
}

inline base_interpolator const& n_ary_tree_weight_node::interpolator( ) const
{
	return				m_interpolator;
}

inline float n_ary_tree_weight_node::weight							( ) const
{
	return				m_weight;
}

inline bool n_ary_tree_weight_node::operator ==						( n_ary_tree_weight_node const& other ) const
{
	return				
		( other.interpolator() == interpolator() )
		&&
		math::is_similar( other.weight(),weight() );
}

inline bool n_ary_tree_weight_node::operator !=						( n_ary_tree_weight_node const& other ) const
{
	return				!(*this == other);
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_WEIGHT_NODE_INLINE_H_INCLUDED