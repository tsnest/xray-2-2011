////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "single_sound_cook.h"
#include <xray/sound/single_sound.h>
#include <xray/memory_stream.h>
#include <xray/sound/spl_utils.h>
#include <xray/resources_types.h>

namespace xray {
namespace sound {

using namespace resources;

pcstr converted_local_path				= "resources.converted/sounds/";
pcstr spl_extention						= ".single_sound_options";

single_sound_cook::single_sound_cook	( ) :
	super								( resources::single_sound_class, reuse_true, use_resource_manager_thread_id )
{
}

single_sound_cook::~single_sound_cook	( )
{
}

void single_sound_cook::translate_query	( query_result_for_cook& parent )
{
	pcstr const req_path			= parent.get_requested_path();
	fs_new::virtual_path_string spl_config_path			= req_path;
	spl_config_path.append					( spl_extention );
	

	resources::request request_array[] =
	{	
		{ req_path, encoded_sound_interface_class },
		{ req_path, sound_rms_class },
		{ spl_config_path.c_str(), sound_spl_class },
	};

	resources::autoselect_quality_bool	autoselect_quality[] =
	{
		resources::autoselect_quality_true,
		resources::autoselect_quality_false,
		resources::autoselect_quality_false,
	};

	math::float4x4 const* matrix_pointers[] =
	{
		&math::float4x4().identity(),
		0,
		0
	};

	resources::query_resource_params params	(
		request_array,
		0,
		array_size( request_array ), 
		boost::bind( &single_sound_cook::on_sub_resources_loaded, this, _1 ),
		unmanaged_allocator(), 
		0,
		matrix_pointers,
		0,
		&parent,
		0, 0, 0, 0, query_type_normal, 0,
		autoselect_quality
	);
	resources::query_resources				( params );
}

void single_sound_cook::on_sub_resources_loaded		( queries_result& data )
{
	query_result_for_cook* const parent		= data.get_parent_query();

	if ( !data.is_successful() )
	{
		parent->finish_query				( result_error );
		return;
	}

	encoded_sound_with_qualities_ptr encoded_sound	= static_cast_resource_ptr<encoded_sound_with_qualities_ptr>( data[0].get_unmanaged_resource() );
	sound_rms_ptr rms								= static_cast_resource_ptr<sound_rms_ptr>( data[1].get_managed_resource() );
	sound_spl_ptr spl								= static_cast_resource_ptr<sound_spl_ptr>( data[2].get_unmanaged_resource() );

	single_sound* new_sound					= UNMANAGED_NEW( new_sound, single_sound )( encoded_sound, rms, 0, spl );

	if ( !new_sound )
	{
		parent->set_out_of_memory			( resources::unmanaged_memory, sizeof( single_sound ) );
		parent->finish_query				( result_out_of_memory );
		return;
	}

	parent->set_unmanaged_resource			( new_sound, nocache_memory, sizeof( single_sound ) );
	parent->finish_query					( result_success );
}

void single_sound_cook::delete_resource	( resource_base* res )
{
	UNMANAGED_DELETE					( res );
}

} // namespace sound
} // namespace xray
