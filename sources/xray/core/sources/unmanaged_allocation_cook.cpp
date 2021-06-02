////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "unmanaged_allocation_cook.h"
#include <xray/unmanaged_allocation_resource.h>

namespace xray {
namespace resources {

unmanaged_allocation_cook::unmanaged_allocation_cook		( ) :
	unmanaged_cook											(
		resources::unmanaged_allocation_class,
		reuse_false,
		use_resource_manager_thread_id,
		use_resource_manager_thread_id
	)
{
}

mutable_buffer unmanaged_allocation_cook::allocate_resource	(
		resources::query_result_for_cook& in_query, 
		const_buffer raw_file_data, 
		bool file_exist
	)
{
	XRAY_UNREFERENCED_PARAMETER			( in_query );
	XRAY_UNREFERENCED_PARAMETER			( file_exist );

	R_ASSERT							( raw_file_data.size() );
	u32 const allocation_size			= raw_file_data.size() + sizeof( unmanaged_allocation_resource );

	mutable_buffer const result			( UNMANAGED_ALLOC( char, allocation_size ), allocation_size );
	return								result;
}

void unmanaged_allocation_cook::create_resource		(
		resources::query_result_for_cook& in_out_query, 
		const_buffer raw_file_data,
		mutable_buffer in_out_unmanaged_resource_buffer
	)
{
	unmanaged_allocation_resource* resource			= new ( in_out_unmanaged_resource_buffer.c_ptr() )unmanaged_allocation_resource( raw_file_data.size() );

	in_out_query.set_unmanaged_resource				(
		resource, 
		resources::unmanaged_memory,
		in_out_unmanaged_resource_buffer.size()
	);

	in_out_query.finish_query						( result_success );
}

void unmanaged_allocation_cook::destroy_resource	( resources::unmanaged_resource* resource )
{
	resource->~unmanaged_resource					( );
}

void unmanaged_allocation_cook::deallocate_resource	( pvoid buffer )
{
	UNMANAGED_FREE									( buffer );
}

} // namespace resources
} // namespace xray