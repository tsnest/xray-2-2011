////////////////////////////////////////////////////////////////////////////
//	Unit		: n_ary_tree_inline.h
//	Created		: 04.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright(C) Challenge Solutions(tm) - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_N_ARY_TREE_INLINE_H_INCLUDED
#define XRAY_ANIMATION_N_ARY_TREE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree::n_ary_tree									( ) :
	m_root					( 0 ),
	m_allocator				( 0 ),
	m_interpolators			( 0 ),
	m_animation_states		( 0 ),
	m_reference_counter		( 0 ),
	m_animations_count		( 0 ),
	m_significant_animations_count ( 0 ),
	m_interpolators_count	( 0 ),
	m_transitions_count		( 0 )
{
}

inline n_ary_tree::n_ary_tree									(
		n_ary_tree_animation_node* const root,
		memory::base_allocator* const allocator,
		base_interpolator const** const interpolators,
		animation_state* const animation_states,
		n_ary_tree_intrusive_base* const reference_counter,
		u32 const animations_count,
		u32 const interpolators_count,
		u32 const transitions_count
	) :
	m_root					( root ),
	m_allocator				( allocator ),
	m_interpolators			( interpolators ),
	m_animation_states		( animation_states ),
	m_reference_counter		( reference_counter ),
	m_animations_count		( animations_count ),
	m_significant_animations_count	( animations_count ) ,
	m_interpolators_count	( interpolators_count ),
	m_transitions_count		( transitions_count )
{
}

inline n_ary_tree::n_ary_tree									( n_ary_tree const& other )
{
	assign					( other );
}

inline n_ary_tree& n_ary_tree::operator =						 ( n_ary_tree const& other )
{
	R_ASSERT				( other.m_animations_count );

	destroy					( );
	assign					( other );

	return					*this;
}

inline void n_ary_tree::assign									( n_ary_tree const& other )
{
	m_root							= other.root();
	m_allocator						= other.m_allocator;
	m_interpolators					= other.m_interpolators;
	m_animation_states				= other.m_animation_states;
	m_reference_counter				= other.m_reference_counter;
	m_animations_count				= other.m_animations_count;
	m_significant_animations_count	= other.m_significant_animations_count;
	m_interpolators_count			= other.m_interpolators_count;
	m_transitions_count				= other.m_transitions_count;
}

inline animation_state const* n_ary_tree::animation_states		( ) const
{
	return					m_animation_states;
}

inline u32 n_ary_tree::significant_animations_count				( ) const
{
	return					m_significant_animations_count;
}

inline n_ary_tree_animation_node* n_ary_tree::root				( ) const
{
	return					m_root;
}

inline base_interpolator const* const* n_ary_tree::interpolators( ) const
{
	return					m_interpolators;
}

inline u32 n_ary_tree::interpolators_count						( ) const
{
	return					m_interpolators_count;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_N_ARY_TREE_INLINE_H_INCLUDED