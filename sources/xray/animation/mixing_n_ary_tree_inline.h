////////////////////////////////////////////////////////////////////////////
//	Unit		: n_ary_tree_inline.h
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_MIXING_N_ARY_TREE_INLINE_H_INCLUDED
#define XRAY_ANIMATION_MIXING_N_ARY_TREE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline n_ary_tree::n_ary_tree									( float4x4 const& object_transform ) :
	m_root							( 0 ),
	m_interpolators					( 0 ),
	m_animation_states				( 0 ),
	m_animation_events				( 0 ),
	m_reference_counter				( 0 ),
	m_animations_count				( 0 ),
	m_interpolators_count			( 0 ),
	m_tree_actual_time_in_ms		( 0 ),
	m_object_transform				( object_transform )
{
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_MIXING_N_ARY_TREE_INLINE_H_INCLUDED