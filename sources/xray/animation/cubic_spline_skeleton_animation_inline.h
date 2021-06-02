////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_CUBIC_SPLINE_SKELETON_ANIMATION_INLINE_H_INCLUDED
#define XRAY_ANIMATION_CUBIC_SPLINE_SKELETON_ANIMATION_INLINE_H_INCLUDED

namespace xray {
namespace animation {

inline animation_event_channels	const	&cubic_spline_skeleton_animation::event_channels		( ) const 
{
#ifndef	MASTER_GOLD
	if ( m_editable_event_channels )
		return m_editable_event_channels->channels( );
	return m_event_channels; 
#else
	return m_event_channels; 
#endif
}

inline	bone_animation	const&	cubic_spline_skeleton_animation::bone( pcstr id )const	
{ 
	bone_index_type order_idx = m_bone_names.bone_index( id );

	R_ASSERT( order_idx != bone_index_type(-1) );

	return bone_data()[ order_idx ]; 
}

inline	bone_animation&			cubic_spline_skeleton_animation::bone( pcstr id )		
{ 
	
	bone_index_type order_idx = m_bone_names.bone_index( id );

	R_ASSERT( order_idx != bone_index_type(-1) );

	return bone_data()[ order_idx ]; 
}

inline		bool	cubic_spline_skeleton_animation::has_bone( pcstr id )const
{
	bone_index_type order_idx = m_bone_names.bone_index( id );
	return ( order_idx != bone_index_type(-1) );
}

//inline		bool	cubic_spline_skeleton_animation::has_bone( u32 id )const
//{
//	return id < bones_count( );
//}

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_CUBIC_SPLINE_SKELETON_ANIMATION_INLINE_H_INCLUDED