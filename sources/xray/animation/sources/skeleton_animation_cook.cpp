////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skeleton_animation_cook.h"
#include "bi_spline_skeleton_animation.h"
#include "check_animation_data.h"
#include <xray/animation/cubic_spline_skeleton_animation.h>

namespace xray {
namespace animation {

xray::command_line::key	check_animation_b_spline_approximation( "check_animation_b_spline_approximation", "", "animation", "" );

} // namespace animation
} // namespace xray

using xray::animation::skeleton_animation_cook;

skeleton_animation_cook::skeleton_animation_cook		( xray::animation::world &world ) :
	super(
		resources::animation_class,
		reuse_true,
		use_current_thread_id
	),
	m_world( world )
{ 
	register_cook	( this );
}

void skeleton_animation_cook::on_bi_spline_data_arrived	( xray::resources::queries_result& result )
{
	timing::timer					timer;
	timer.start						( );
	configs::binary_config_ptr config	= static_cast_resource_ptr< xray::configs::binary_config_ptr >( result[0].get_unmanaged_resource() );

	if ( !config )
	{
		result.get_parent_query()->finish_query			( result_error );
		return;
	}

#pragma message( XRAY_TODO("Dima to all: we should create B-spline animations in-place and not from binary configs, but from a custom format") )
	bi_spline_skeleton_animation* const animation = NEW( bi_spline_skeleton_animation )( );
	animation->load					( (*config)["splines"] );
	
	if ( result[1].get_unmanaged_resource() )
	{
		xray::configs::binary_config_ptr edit_data_config	= static_cast_resource_ptr< xray::configs::binary_config_ptr >( result[1].get_unmanaged_resource() );
		animation->create_event_channels	( (*edit_data_config)["data"]["event_channels"] );
	}

	const_buffer buffer( (pvoid)animation, sizeof( animation ) );

	resources::query_create_resource(
		"",
		buffer,
		resources::cubic_spline_skeleton_animation_class,
		boost::bind( &skeleton_animation_cook::on_cubic_spline_animation_cooked, this, _1, result.size() > 2 ? result[2].get_unmanaged_resource() : 0 ),
		g_allocator,
		0,
		result.get_parent_query() 
	);

#if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)
	if ( check_animation_b_spline_approximation )
	{
		xray::configs::lua_config_ptr config_check_animation	= static_cast_resource_ptr<xray::configs::lua_config_ptr>(result[2].get_unmanaged_resource());
		test_data					( config->get_root(), *config_check_animation, m_world );
	}
#endif // #if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)

	LOG_INFO						( "animation creation took time: %f", timer.get_elapsed_sec() );
}

void skeleton_animation_cook::on_cubic_spline_animation_cooked( xray::resources::queries_result& data,
															   xray::resources::unmanaged_resource_ptr check_data )
{
	R_ASSERT						( data.is_successful() );

	bi_spline_skeleton_animation const* bi_animation = static_cast< bi_spline_skeleton_animation const* >( data[ 0 ].creation_data_from_user().c_ptr() );

#if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)
	if ( check_animation_b_spline_approximation )
	{
		resources::pinned_ptr_const< cubic_spline_skeleton_animation > const& pinned_ptr	= data[ 0 ].get_managed_resource( );
		cubic_spline_skeleton_animation const* cubic_animation = pinned_ptr.c_ptr();

		xray::configs::lua_config_ptr config_check_animation	= static_cast_resource_ptr<xray::configs::lua_config_ptr>(check_data);

		compare( config_check_animation, *cubic_animation );
	}
#endif // #if XRAY_PLATFORM_WINDOWS && !defined(MASTER_GOLD)

	DELETE							( bi_animation );

	resources::managed_resource_ptr managed_resource = data[ 0 ].get_managed_resource( );
	R_ASSERT						( managed_resource );
//	update_class_id					( managed_resource, resources::animation_class );
	data.get_parent_query()->set_managed_resource	( managed_resource );
	data.get_parent_query()->finish_query			( result_success );
}

void skeleton_animation_cook::translate_query		( xray::resources::query_result_for_cook& parent )
{
	pcstr clip_path				= 0; 
	STR_JOINA					( clip_path, parent.get_requested_path(), ".clip" );

	pcstr clip_data_path		= 0; 
	STR_JOINA					(clip_data_path, parent.get_requested_path(), ".clip_data");

	if ( !check_animation_b_spline_approximation ) {
		resources::request const requests[] = {	
			{ clip_path,		resources::binary_config_class },
			{ clip_data_path,	resources::binary_config_class },
		};

		resources::query_resources	(
			requests,
			array_size( requests ), 
			boost::bind( &skeleton_animation_cook::on_bi_spline_data_arrived, this, _1 ),
			g_allocator, 
			0,
			&parent
		);
		return;
	}

	pcstr check_data_file_path = 0; 
	STR_JOINA						( check_data_file_path, parent.get_requested_path(), ".clip_check_data" );
	resources::request const requests[] = {	
		{ clip_path,			resources::binary_config_class	},
		{ clip_data_path,		resources::binary_config_class	},
		{ check_data_file_path,	resources::lua_config_class		},
	};

	resources::query_resources		(
		requests, 
		array_size( requests ), 
		boost::bind( &skeleton_animation_cook::on_bi_spline_data_arrived, this, _1 ), 
		g_allocator, 
		0,
		&parent
	); 
}

void skeleton_animation_cook::delete_resource		( xray::resources::resource_base* dying_resource )
{
	XRAY_UNREFERENCED_PARAMETER	( dying_resource );
	NODEFAULT					( );
}