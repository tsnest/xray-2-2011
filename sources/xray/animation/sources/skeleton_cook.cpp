////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "skeleton_cook.h"
#include <xray/animation/skeleton.h>

using xray::animation::skeleton_cook;
using xray::animation::skeleton_bone;
using xray::animation::bone_index_type;

skeleton_cook::skeleton_cook				( ) :
	super							( resources::skeleton_class, reuse_true, use_resource_manager_thread_id )
{
	register_cook					( this );
}

static u32 get_bones_count	( xray::configs::binary_config_value const& value, u32& bones_ids_buffer_length )
{
	u32 count				= 0;

	if ( value.id )
		bones_ids_buffer_length	+= (xray::strings::length( value.id ) + 1) * sizeof(char);
	else
		R_ASSERT			( !bones_ids_buffer_length );

	xray::configs::binary_config_value::const_iterator i		= value.begin();
	xray::configs::binary_config_value::const_iterator const e	= value.end();
	for ( ; i != e; ++i, ++count )
		count				+= get_bones_count( *i, bones_ids_buffer_length );

	return					count;
}

static void add_bone		(
		skeleton_bone const* parent,
		skeleton_bone* bones_begin,
		bone_index_type index,
		bone_index_type& last_index,
		xray::configs::binary_config_value::const_iterator const& config,
		pstr& bones_ids_buffer,
		u32& bones_ids_buffer_size
	)
{
	xray::configs::binary_config_value const value	= *config;
	u32 const children_count = value.size();

	u32 const bone_id_buffer_size	= (xray::strings::length( config->key() ) + 1)*sizeof(char);
	xray::memory::copy	( bones_ids_buffer, bones_ids_buffer_size, config->key(), bone_id_buffer_size );

	new (bones_begin + index) skeleton_bone (
		bones_ids_buffer,
		parent,
		children_count ? bones_begin + last_index : 0,
		children_count ? bones_begin + last_index + children_count : 0
	);

	bones_ids_buffer		+= bone_id_buffer_size;
	bones_ids_buffer_size	-= bone_id_buffer_size;

	if ( !children_count )
		return;

	bone_index_type const old_last_index = last_index;
	last_index				+= children_count;
	xray::configs::binary_config_value::const_iterator i	= value.begin();
	for (u32 new_index = old_last_index, n = new_index + children_count; new_index != n; ++new_index, ++i )
		add_bone			( bones_begin + index, bones_begin, new_index, last_index, i, bones_ids_buffer, bones_ids_buffer_size );
}

void skeleton_cook::on_sub_resources_loaded	( xray::resources::queries_result& result )
{
	resources::query_result_for_cook* const parent	=	result.get_parent_query();

	if ( !result.is_successful() ) {
		parent->finish_query		( result_error );
		return;
	}
	
	configs::binary_config_ptr config	= static_cast_resource_ptr< xray::configs::binary_config_ptr >(result[0].get_unmanaged_resource());
	u32 bones_ids_buffer_length		= 0;
	u32 const bones_count			= get_bones_count( config->get_root(), bones_ids_buffer_length );
	u32 const buffer_size			= sizeof(skeleton) + bones_count*sizeof(skeleton_bone) + bones_ids_buffer_length;
	pvoid const buffer				= resources::allocate_unmanaged_memory( buffer_size, "skeleton" );
	if ( !buffer ) {
		parent->set_out_of_memory	( resources::unmanaged_memory, sizeof(skeleton) );
		parent->finish_query		( result_out_of_memory );
		return;
	}

	skeleton_bone* const bones		= pointer_cast<skeleton_bone*>( static_cast<pbyte>(buffer) + sizeof(skeleton) );
	pstr bones_ids_buffer			= pointer_cast<pstr>( bones + bones_count );

	ASSERT_CMP						( config->size(), ==, 1 );
	u32 last_index					= 1;
	add_bone						( 0, bones, 0, last_index, config->begin(), bones_ids_buffer, bones_ids_buffer_length );
	R_ASSERT_CMP					( bones_ids_buffer_length, ==, 0 );

	skeleton* const resource		= new ( buffer ) skeleton( bones_count );
	parent->set_unmanaged_resource	( resource, resources::nocache_memory, sizeof(skeleton) );
	parent->finish_query			( result_success );
}

void skeleton_cook::translate_query			( xray::resources::query_result_for_cook& parent )
{
	pcstr config_path				= 0; 
	STR_JOINA						( config_path, parent.get_requested_path(), ".skeleton" );

	resources::query_resource 		(
		config_path, 
		resources::binary_config_class, 
		boost::bind( &skeleton_cook::on_sub_resources_loaded, this, _1 ), 
		resources::helper_allocator(),
		0,
		&parent
	);
}

void skeleton_cook::delete_resource			( xray::resources::resource_base* res )
{
	skeleton* data					= static_cast_checked< skeleton* >( res ) ;
	data->~skeleton					( );
	UNMANAGED_FREE					( data );
}