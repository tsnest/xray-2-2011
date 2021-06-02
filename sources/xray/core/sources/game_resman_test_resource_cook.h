////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_RESMAN_TEST_RESOURCE_COOK_H_INCLUDED
#define GAME_RESMAN_TEST_RESOURCE_COOK_H_INCLUDED

#include <xray/resources_cook_classes.h>
#include "game_resman_test_utils.h"

namespace xray {
namespace resources {

class test_resource_cook : public unmanaged_cook
{
public:
							test_resource_cook	(class_id_enum				class_id,
						 						 u32						allocator_index,
						 						 bool						create_allocates_destroy_deallocates,
			 			 						 u32						create_thread_id, 
			 			 						 u32						allocate_thread_id);

	virtual	mutable_buffer	allocate_resource	(query_result_for_cook &	in_query, 
												 const_buffer				raw_file_data, 
												 bool						file_exist);

	virtual void			destroy_resource	(unmanaged_resource *		resource);
	virtual void			deallocate_resource	(pvoid						buffer);
	virtual void			create_resource		(query_result_for_cook &	in_out_query, 
												 const_buffer				raw_file_data,
												 mutable_buffer				in_out_unmanaged_resource_buffer);

private:
	u32						m_allocator_index;
	bool					m_in_create_resource;
	bool					m_create_allocates_destroy_deallocates;
}; // class test_resource_cook


} // namespace resources
} // namespace xray

#endif // #ifndef GAME_RESMAN_TEST_RESOURCE_COOK_H_INCLUDED