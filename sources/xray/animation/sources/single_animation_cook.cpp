////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "single_animation_cook.h"
#include "single_animation.h"
#include <xray/animation/base_interpolator.h>
#include <xray/animation/instant_interpolator.h>
#include <xray/animation/linear_interpolator.h>
#include <xray/animation/fermi_interpolator.h>
#include <xray/animation/cubic_spline_skeleton_animation.h>

namespace xray {
namespace animation {

pcstr single_extention								= ".clip";
pcstr options_extention								= ".options";
struct animation_collection_cook_user_data
{
	configs::binary_config_value						val;
	configs::binary_config_ptr							cfg_ptr;
};

single_animation_cook::single_animation_cook	( ) :
	super												(
		resources::single_animation_class,
		reuse_true,
		use_resource_manager_thread_id
	)
{
	register_cook( this );
}

single_animation_cook::~single_animation_cook			( )
{
}

void single_animation_cook::translate_query				( resources::query_result_for_cook& parent )
{
	xray::fs::path_string animation_path;
	animation_path.assignf	("resources/animations/single/%s", parent.get_requested_path( ) );
	xray::fs::path_string options_path;
	options_path.assignf	("resources/animations/single/%s%s", parent.get_requested_path( ), options_extention );

	xray::resources::request arr[2] = {
		{animation_path.c_str( ), xray::resources::animation_class},
		{options_path.c_str( ), xray::resources::binary_config_class}
	};

	query_resources(
		arr,
		array_size(arr),
		boost::bind( &single_animation_cook::on_sub_resources_loaded, this, _1 ),
		resources::unmanaged_allocator( ),
		0,
		&parent 
	);
}

void single_animation_cook::on_sub_resources_loaded		( resources::queries_result& data )
{
	resources::query_result_for_cook* const	parent		= data.get_parent_query( );
	if ( !data[0].is_successful( ) )
	{
		parent->finish_query							( result_error );
		return;
	}
	
	
	xray::animation::skeleton_animation_ptr anim = static_cast_resource_ptr<xray::animation::skeleton_animation_ptr>( data[0].get_managed_resource( ) );

	base_interpolator* interpolator = NULL;

	if( data[1].is_successful( ) )
	{
		configs::binary_config_ptr		config				= static_cast_resource_ptr<configs::binary_config_ptr>( data[1].get_unmanaged_resource( ) );
		if( config->get_root( ).value_exists("interpolator") )
		{
			configs::binary_config_value	interpolator_value	= config->get_root( )["interpolator"];

			u32 type = interpolator_value["type"];

			switch( type )
			{
			case 0:
			{
				interpolator = XRAY_NEW_IMPL( resources::unmanaged_allocator( ), instant_interpolator )( );
				break;
			}

			case 1:
			{
				interpolator = XRAY_NEW_IMPL( resources::unmanaged_allocator( ), linear_interpolator )( interpolator_value["time"] );
				break;
			}
					
			case 2:
			{
				interpolator = XRAY_NEW_IMPL( resources::unmanaged_allocator( ), fermi_interpolator )( interpolator_value["time"], interpolator_value["epsilon"] );
				break;
			}

			default: 
				NODEFAULT( );
			}
		}
		else
		{
			interpolator = XRAY_NEW_IMPL( resources::unmanaged_allocator( ), instant_interpolator )( );
		}
	}
	else
	{
		interpolator = XRAY_NEW_IMPL( resources::unmanaged_allocator( ), instant_interpolator )( );
	}

	single_animation* result_anim;
	UNMANAGED_NEW( result_anim, single_animation )( anim, interpolator );
	
	parent->set_unmanaged_resource					( result_anim, resources::nocache_memory, sizeof( single_animation ) );
	parent->finish_query							( result_success );
}

void single_animation_cook::delete_resource			( resources::resource_base* res )
{
	res->~resource_base								( );
	UNMANAGED_DELETE								( res );
}

} // namespace animation
} // namespace xray