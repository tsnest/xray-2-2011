////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/animation/cubic_spline_skeleton_animation.h>
#include "bi_spline_skeleton_animation.h"
#include "bi_spline_bone_animation.h"
#include <xray/animation/skeleton.h>

using xray::animation::skeleton;
using xray::animation::skeleton_bone;

namespace xray {
namespace animation {

cubic_spline_skeleton_animation::cubic_spline_skeleton_animation	( bi_spline_skeleton_animation const& animation	) :
#ifndef MASTER_GOLD
	m_allocator		( 0 ),
#endif // #ifndef MASTER_GOLD
	m_animation_type( animation.animation_type() )
{
	m_bone_count = animation.bones_count();

	pvoid mem_ptr	= this;
	mem_ptr			= static_cast<pbyte>(mem_ptr) + sizeof( *this );
	
	m_bone_names.create_internals_in_place(  animation.bone_names(), mem_ptr );

	mem_ptr	= get_shift_ptr( mem_ptr, bone_names::count_internal_memory_size( m_bone_count ) );
	
	m_internal_memory_position = bytes_dist( mem_ptr, this );
	
	mem_ptr	= get_shift_ptr( mem_ptr, m_bone_count * sizeof( bone_animation ) );

	for ( u32 bone = 0; bone < m_bone_count; ++bone )
	{
		const bi_spline_bone_animation &bd = animation.bone( bone );
		new ( &bone_data()[ bone ] ) bone_animation;
		bone_data()[ bone ].create_internals_in_place( bd, mem_ptr );
		mem_ptr	= get_shift_ptr( mem_ptr, bone_animation::count_internal_memory_size( bd ) );
	}
	
	if ( animation.event_channels()	)
		m_event_channels.create_in_place_internals( *animation.event_channels(), mem_ptr );

#ifndef MASTER_GOLD
	m_editable_event_channels	= XRAY_NEW_IMPL(debug::g_mt_allocator, editable_animation_event_channels )( m_event_channels );
#endif // #ifndef MASTER_GOLD
}

cubic_spline_skeleton_animation::~cubic_spline_skeleton_animation	( )
{
	R_ASSERT					( m_bone_count != u32(-1) );
	R_ASSERT					( m_internal_memory_position != size_t(-1) );

	for ( u32 bone = 0; bone < m_bone_count; ++bone )
		( bone_data()[ bone ] ).~bone_animation();

#ifndef MASTER_GOLD
	XRAY_DELETE_IMPL			( debug::g_mt_allocator, m_editable_event_channels );
#endif // #ifndef MASTER_GOLD
}

u32 cubic_spline_skeleton_animation::count_memory_size ( bi_spline_skeleton_animation const& animation )
{
	size_t size = 0;
	size += sizeof( cubic_spline_skeleton_animation );
	
	u32 bones_count = animation.bones_count();

	size += bone_names::count_internal_memory_size( bones_count );

	size += bones_count * sizeof( bone_animation );

	for ( u32 i = 0; i < bones_count; ++i )
	{
		size += bone_animation::count_internal_memory_size( animation.bone( i ) );
	}
	if ( animation.event_channels() )
		size += animation.event_channels()->internal_memory_size	();

	return size;
}

cubic_spline_skeleton_animation* cubic_spline_skeleton_animation::new_animation	( pvoid buffer_for_animation, bi_spline_skeleton_animation const& animation )
{
	return									new ( buffer_for_animation ) cubic_spline_skeleton_animation( animation );
}

#ifndef MASTER_GOLD
cubic_spline_skeleton_animation* cubic_spline_skeleton_animation::new_animation	( memory::base_allocator& allocator, bi_spline_skeleton_animation const& animation )
{
	cubic_spline_skeleton_animation* const result = 
		new (
			XRAY_MALLOC_IMPL( allocator, cubic_spline_skeleton_animation::count_memory_size(animation), "cubic_spline_skeleton_animation" )
		) cubic_spline_skeleton_animation	( animation );
	result->m_allocator						= &allocator;
	return									result;
}
#endif // #ifndef MASTER_GOLD

void cubic_spline_skeleton_animation::delete_animation							( cubic_spline_skeleton_animation const*& animation_to_delete )
{
#ifndef MASTER_GOLD
	memory::base_allocator* const allocator	= animation_to_delete->m_allocator;
#endif // #ifndef MASTER_GOLD

	animation_to_delete->~cubic_spline_skeleton_animation	( );

#ifndef MASTER_GOLD
	if ( allocator )
		XRAY_FREE_IMPL						( *allocator, animation_to_delete );
#endif // #ifndef MASTER_GOLD
}

} // namespace animation
} // namespace xray