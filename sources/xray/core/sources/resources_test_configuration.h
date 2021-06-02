////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_TEST_CONFIGURATION_H_INCLUDED
#define RESOURCES_TEST_CONFIGURATION_H_INCLUDED

#include "resources_test_base_cook.h"
#include "resources_test_resource.h"

namespace xray {
namespace resources {

enum cook_type_enum {	cook_type_managed, 
						cook_type_inplace_managed, 
						cook_type_unmanaged, 
						cook_type_inplace_unmanaged,
						cook_type_translate_query,
						cook_type_last				
					};

struct test_configuration_type
{
	test_configuration_type (u32 num_iterations) : iterations_todo(num_iterations)
	{
//		 managed, cook_reuse: reuse_true, create_in_current_thread: 0, allocate_in_current_thread: 0, sleep_after_mount: 1

		iterations_todo						=	1;
		//cook_type							=	cook_type_inplace_unmanaged;
		cook_type							=	cook_type_managed;
		reuse_type							=	cook_base::reuse_false;
		allocate_in_current_thread			=	false;
		create_in_current_thread			=	false;
		generate_if_no_file_test			=	generate_if_no_file_test_unset;
		wait_after_mount					=	0;
		do_query_create_resource			=	false;
		inplace_in_creation_or_inline_data	=	false;
		translate_in_current_thread			=	false;
	}

	bool move_next	()
	{
		if ( !iterations_todo )
			return									false;

    		if ( !wait_after_mount ) {
    			wait_after_mount					=	true;
    			return									true;
    		} else wait_after_mount					=	false;
    
    		if ( reuse_type == cook_base::reuse_false ) {
    			reuse_type							=	cook_base::reuse_true;
    			return									true;
    		} 
    		else if ( reuse_type == cook_base::reuse_true ) {
    			reuse_type							=	cook_base::reuse_false;
    			if ( !do_query_create_resource && cook_type != cook_type_translate_query ) {
    				if ( cook_type == cook_type_unmanaged ) {
    					reuse_type					=	cook_base::reuse_raw;
    					return							true;
    				}
    			}
    		} else if ( reuse_type == cook_base::reuse_raw ) 
    			reuse_type					=	cook_base::reuse_false;
    
    		if ( cook_type != cook_type_translate_query )
    		{
 			if ( !do_query_create_resource ) {
 				do_query_create_resource		=	true;
 				return								true;
 			} else do_query_create_resource		=	false;
 
 			if ( !do_query_create_resource ) {
 				if ( generate_if_no_file_test == generate_if_no_file_test_unset ) {
 					generate_if_no_file_test	=	generate_if_no_file_test_requery;
 					return							true;
 				} else if ( generate_if_no_file_test == generate_if_no_file_test_requery ) {
 					generate_if_no_file_test	=	generate_if_no_file_test_save_generated;
 					return							true;
 				} else generate_if_no_file_test	=	generate_if_no_file_test_unset;
 			}
 
 			if ( !create_in_current_thread ) {
 				create_in_current_thread		=	true;
 				return								true;
 			} else create_in_current_thread		=	false;
 			
 			if ( cook_type == cook_type_unmanaged || cook_type == cook_type_inplace_unmanaged )	{
 				if ( !allocate_in_current_thread ) {
 					allocate_in_current_thread		=	true;
 					return								true;
 				} else allocate_in_current_thread	=	false;
 			}
 
 			if ( cook_type == cook_type_inplace_unmanaged && reuse_type == cook_base::reuse_true )
 			{
 				if ( !inplace_in_creation_or_inline_data ) {
 					inplace_in_creation_or_inline_data	=	true;
 					return									true;
 				} else
 					inplace_in_creation_or_inline_data	=	false;
 				}
    		}
    		else
    		{
    			if ( !translate_in_current_thread ) {
    				translate_in_current_thread		=	true;
    				return								true; 
    			} else
    				translate_in_current_thread		=	false;
    		}
   
     	if ( cook_type == cook_type_last - 1 )
 			cook_type							=	(cook_type_enum)0;
 		else
 		{
 			u32 const cook_type_int				=	(u32)cook_type + 1;
 			cook_type							=	(cook_type_enum)cook_type_int;
 			return									true;
 		}

		--iterations_todo;
		if ( !iterations_todo )
			return									false;
		
		return										true;			
	}

	bool										wait_after_mount;
	cook_type_enum								cook_type;
	cook_base::reuse_enum						reuse_type;
	bool										allocate_in_current_thread;
	bool										create_in_current_thread;
	bool										translate_in_current_thread;
	generate_if_no_file_test_enum				generate_if_no_file_test;
	bool										do_query_create_resource;
	bool										inplace_in_creation_or_inline_data;
//private:
	u32											iterations_todo;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_TEST_CONFIGURATION_H_INCLUDED