////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "cubic_spline_skeleton_animation_cook.h"
#include "bi_spline_skeleton_animation.h"
#include <xray/animation/cubic_spline_skeleton_animation.h>

using xray::animation::cubic_spline_skeleton_animation_cook;

cubic_spline_skeleton_animation_cook::cubic_spline_skeleton_animation_cook	( ) :
	super								( 
		resources::cubic_spline_skeleton_animation_class,
		reuse_true,
		use_resource_manager_thread_id 
	)
{
	register_cook	( this );
}
	
u32 cubic_spline_skeleton_animation_cook::calculate_resource_size			( xray::const_buffer bi_spline_skeleton_animation_buffer, bool const file_exist )
{
	R_ASSERT_U							( file_exist );
	return
		cubic_spline_skeleton_animation::count_memory_size(
			*static_cast_checked< bi_spline_skeleton_animation const* >(
				bi_spline_skeleton_animation_buffer.c_ptr()
			)
		);
}

void cubic_spline_skeleton_animation_cook::create_resource					(
		xray::resources::query_result_for_cook& in_out_query, 
		xray::const_buffer bi_spline_skeleton_animation_buffer,
		xray::resources::managed_resource_ptr out_resource
	)
{ 
	resources::pinned_ptr_mutable< cubic_spline_skeleton_animation > const& pinned_ptr	= pin_for_write< cubic_spline_skeleton_animation >( out_resource );
	cubic_spline_skeleton_animation* const animation	=
		cubic_spline_skeleton_animation::new_animation(
			pinned_ptr.c_ptr(),
			*static_cast_checked< bi_spline_skeleton_animation const* >(
				bi_spline_skeleton_animation_buffer.c_ptr()
			)
		);

#ifndef	MASTER_GOLD
	animation->set_file_name			( in_out_query.get_parent_query()->get_requested_path() );
#else // #ifndef	MASTER_GOLD
	XRAY_UNREFERENCED_PARAMETER			( animation );
#endif // #ifndef	MASTER_GOLD
	
	in_out_query.finish_query			( result_success );
}

void cubic_spline_skeleton_animation_cook::destroy_resource					( xray::resources::managed_resource* const dying_resource ) 
{
	resources::pinned_ptr_mutable<cubic_spline_skeleton_animation> pinned_animation = pin_for_write< cubic_spline_skeleton_animation >( dying_resource );
	cubic_spline_skeleton_animation const* instance		= &*pinned_animation;
	cubic_spline_skeleton_animation::delete_animation	( instance );
}