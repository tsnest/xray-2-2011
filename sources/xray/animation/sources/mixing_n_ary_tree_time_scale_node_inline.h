////////////////////////////////////////////////////////////////////////////
//	Created		: 24.09.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_N_ARY_TREE_TIME_SCALE_NODE_INLINE_H_INCLUDED
#define MIXING_N_ARY_TREE_TIME_SCALE_NODE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree_time_scale_node::n_ary_tree_time_scale_node					(
		base_interpolator const& interpolator,
		float const time_scale,
		float animation_time_before_scale_starts,
		u32 time_scale_start_time_in_ms
	) :
	m_interpolator						( interpolator ),
	m_time_scale						( time_scale ),
	m_animation_time_before_scale_starts( animation_time_before_scale_starts ),
	m_time_scale_start_time_in_ms		( time_scale_start_time_in_ms )
{
	R_ASSERT_CMP						( math::abs(time_scale), <=, 10.f );
}

inline n_ary_tree_time_scale_node::n_ary_tree_time_scale_node					( n_ary_tree_time_scale_node const& other ) :
	n_ary_tree_base_node				( other ),
	m_interpolator						( other.m_interpolator ),
	m_time_scale						( other.m_time_scale ),
	m_animation_time_before_scale_starts( other.m_animation_time_before_scale_starts ),
	m_time_scale_start_time_in_ms		( other.m_time_scale_start_time_in_ms )
{
	R_ASSERT_CMP						( math::abs(other.m_time_scale), <=, 10.f );
}

inline base_interpolator const& n_ary_tree_time_scale_node::interpolator		( ) const
{
	return				m_interpolator;
}

inline float n_ary_tree_time_scale_node::time_scale								( ) const
{
	return				m_time_scale;
}

inline u32 n_ary_tree_time_scale_node::time_scale_start_time_in_ms				( ) const
{
	return				m_time_scale_start_time_in_ms;
}

inline float n_ary_tree_time_scale_node::animation_time_before_scale_starts		( ) const
{
	R_ASSERT_CMP		( m_animation_time_before_scale_starts, >=, 0.f );
	return				m_animation_time_before_scale_starts;
}

inline	void	n_ary_tree_time_scale_node::set_time_scale_start_time			( u32 const time_scale_start_time_in_ms, float const animation_time_before_scale_starts )
{
	R_ASSERT_CMP		( animation_time_before_scale_starts, >=, 0.f );

	m_time_scale_start_time_in_ms			= time_scale_start_time_in_ms;
	m_animation_time_before_scale_starts	= animation_time_before_scale_starts;
}

inline bool n_ary_tree_time_scale_node::operator ==								( n_ary_tree_time_scale_node const& other ) const
{
	return				
		( other.interpolator() == interpolator() )
		&&
		math::is_similar( other.time_scale(), time_scale() );
}

inline bool n_ary_tree_time_scale_node::operator !=								( n_ary_tree_time_scale_node const& other ) const
{
	return				!(*this == other);
}

inline bool operator <															(
		n_ary_tree_time_scale_node const& left,
		n_ary_tree_time_scale_node const& right
	)
{
	if ( left.interpolator().transition_time() < right.interpolator().transition_time() )
		return	true;

	if ( right.interpolator().transition_time() < left.interpolator().transition_time() )
		return	false;

	return				left.time_scale() < right.time_scale();
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_N_ARY_TREE_TIME_SCALE_NODE_INLINE_H_INCLUDED