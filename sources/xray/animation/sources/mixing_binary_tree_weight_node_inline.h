////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BINARY_TREE_WEIGHT_NODE_INLINE_H_INCLUDED
#define BINARY_TREE_WEIGHT_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline binary_tree_weight_node::binary_tree_weight_node						(
		float const weight,
		base_interpolator const* const interpolator
	) :
	m_interpolator		( interpolator ),
	m_weight			( weight ),
	m_simplified_weight	( weight )
{
}

inline float binary_tree_weight_node::weight								( ) const
{
	return				m_weight;
}

inline void binary_tree_weight_node::set_weight								( float const new_weight )
{
	ASSERT				( m_weight == m_simplified_weight );
	m_weight			= new_weight;
	m_simplified_weight	= new_weight;
}

inline base_interpolator const& binary_tree_weight_node::interpolator_impl	( ) const
{
	ASSERT				( m_interpolator );
	return				*m_interpolator;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef BINARY_TREE_WEIGHT_NODE_INLINE_H_INCLUDED