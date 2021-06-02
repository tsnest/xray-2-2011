////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef UNMANAGED_ALLOCATION_COOK_H_INCLUDED
#define UNMANAGED_ALLOCATION_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace resources {

class unmanaged_allocation_cook : public resources::unmanaged_cook
{
public:
							unmanaged_allocation_cook	( );

	virtual	mutable_buffer	allocate_resource			(
								resources::query_result_for_cook& in_query, 
								const_buffer raw_file_data, 
								bool file_exist
							);
	virtual void			create_resource				(
								resources::query_result_for_cook& in_out_query, 
								const_buffer raw_file_data,
								mutable_buffer in_out_unmanaged_resource_buffer
							);

	virtual void			destroy_resource			( resources::unmanaged_resource* resource );
	virtual void			deallocate_resource			( pvoid buffer );
};

} // namespace resources
} // namespace xray

#endif // #ifndef UNMANAGED_ALLOCATION_COOK_H_INCLUDED