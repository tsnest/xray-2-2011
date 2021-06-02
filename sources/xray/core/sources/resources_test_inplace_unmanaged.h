////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_TEST_INPLACE_UNMANAGED_H_INCLUDED
#define RESOURCES_TEST_INPLACE_UNMANAGED_H_INCLUDED

#include "resources_test_base_cook.h"
#include "resources_test_resource.h"

namespace xray {
namespace resources {

class test_inplace_unmanaged_cook : public inplace_unmanaged_cook, public test_cook_base
{
public:
	test_inplace_unmanaged_cook ( reuse_enum	reuse_type, 
						 		  bool			create_in_current_thread, 
						 		  bool			allocate_in_current_thread,
						 		  generate_if_no_file_test_enum generate_if_no_file_test,
								  bool			inplace_in_creation_or_inline_data) 
	: test_cook_base(generate_if_no_file_test), m_inplace_in_creation_or_inline_data(inplace_in_creation_or_inline_data),
	inplace_unmanaged_cook	(test_resource_class, reuse_type, create_in_current_thread ? use_current_thread_id : use_cook_thread_id,
							 allocate_in_current_thread ? use_current_thread_id : use_resource_manager_thread_id),
	m_allocate_in_current_thread(allocate_in_current_thread) {}

	void	create_resource_if_no_file	(query_result_for_cook &	in_out_query, 
										 mutable_buffer				in_out_unmanaged_resource_buffer)
	{
		if ( generate_if_no_file_test() == generate_if_no_file_test_requery )
		{
			query_resource				("test/disk/generate_helper.txt", 
										 raw_data_class,  
										 boost::bind(&test_managed_cook::on_generate_helper_resource_loaded, this, _1),
										 & memory::g_mt_allocator,
										 NULL,
										 & in_out_query);

			in_out_query.finish_query	(result_postponed);
			return;
		}

		R_ASSERT						(generate_if_no_file_test() == generate_if_no_file_test_save_generated);

		mutable_buffer					generated_file_data(ALLOCA(generate_file_data_size), generate_file_data_size);
		fill_with_generated_file_data	(generated_file_data);

		mutable_buffer const			dest_file_data	((pbyte)in_out_unmanaged_resource_buffer.c_ptr() + sizeof(test_unmanaged_resource), generate_file_data_size);
		memory::copy					(dest_file_data, generated_file_data);

		test_unmanaged_resource * test_resource	=	new	(in_out_unmanaged_resource_buffer.c_ptr())	
													test_unmanaged_resource("", dest_file_data, creation_from_file);

		if ( !save_generated(in_out_query, generated_file_data) )
			return;

		in_out_query.set_unmanaged_resource	(test_resource, 
											 m_allocate_in_current_thread ?
											 nocache_memory : managed_memory,
											 in_out_unmanaged_resource_buffer.size());
		in_out_query.finish_query			(result_success);
	}
	
	void	create_resource_inplace			(query_result_for_cook &	in_out_query, 
											 mutable_buffer				in_out_unmanaged_resource_buffer)
	{
		XRAY_UNREFERENCED_PARAMETERS				(& in_out_query);
		R_ASSERT									(in_out_unmanaged_resource_buffer.size() >= sizeof(test_unmanaged_resource));
		mutable_buffer const raw_file_data			((pbyte)in_out_unmanaged_resource_buffer.c_ptr() + sizeof(test_unmanaged_resource), in_out_unmanaged_resource_buffer.size() - sizeof(test_unmanaged_resource));

		test_unmanaged_resource * out_resource	=	new	(in_out_unmanaged_resource_buffer.c_ptr())	
													test_unmanaged_resource
													(in_out_query.get_requested_path(),
													 raw_file_data,
													 in_out_query.creation_data_from_user() ? 
													 creation_inplace_from_memory : creation_inplace_from_file);
	
		out_resource->test_consistency				();
		in_out_query.set_unmanaged_resource			(out_resource, 
													 nocache_memory, 
													 in_out_unmanaged_resource_buffer.size());
		in_out_query.finish_query					(result_success);
	}

	virtual create_resource_inplace_delegate_type	get_create_resource_inplace_in_inline_fat_delegate () 
	{ 
		if ( !m_inplace_in_creation_or_inline_data )
			return						NULL;

		return							create_resource_inplace_delegate_type
										(this, & test_inplace_unmanaged_cook::create_resource_inplace); 
	}

	virtual create_resource_inplace_delegate_type	get_create_resource_inplace_in_creation_data_delegate () 
	{ 
		if ( !m_inplace_in_creation_or_inline_data )
			return						NULL;

		return							create_resource_inplace_delegate_type
										(this, & test_inplace_unmanaged_cook::create_resource_inplace); 
	}

	virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() 
	{ 
		if ( generate_if_no_file_test() == generate_if_no_file_test_unset )
			return				NULL; 
		return							create_resource_if_no_file_delegate_type(this, & test_inplace_unmanaged_cook::create_resource_if_no_file);
	}

	virtual	mutable_buffer	 allocate_resource	(query_result_for_cook &	in_query, 
												 u32						file_size, 
												 u32 &						out_offset_to_file, 
												 bool						file_exist)
	{
		R_ASSERT								(file_size || !file_exist);

		if ( !file_exist && generate_if_no_file_test() == generate_if_no_file_test_requery )
		{
			in_query.set_zero_allocation		();
			return								mutable_buffer::zero();
		}

		XRAY_UNREFERENCED_PARAMETER				(in_query);
		u32 const raw_file_size				=	file_exist ? file_size : generate_file_data_size;
		u32 const resource_size				=	sizeof(test_unmanaged_resource) + raw_file_size;
		out_offset_to_file					=	sizeof(test_unmanaged_resource);
		
		mutable_buffer const out_resource		(m_allocate_in_current_thread ? 
												 MT_ALLOC(char, resource_size) : 
												 UNMANAGED_ALLOC(char, resource_size), 
												 resource_size);

		return									out_resource;
	}

	virtual void					deallocate_resource		(pvoid buffer)
	{
		if ( m_allocate_in_current_thread )
			MT_FREE											(buffer);
		else
			UNMANAGED_FREE									(buffer);
	}

	virtual void					destroy_resource		(unmanaged_resource * resource)
	{
		test_unmanaged_resource * test_resource			=	static_cast_checked<test_unmanaged_resource *>(resource);
		test_resource->~test_unmanaged_resource				();
	}

	result_enum				create_resource_impl			(query_result_for_cook &	in_out_query,
															 mutable_buffer				in_out_unmanaged_resource_buffer)
	{
		XRAY_UNREFERENCED_PARAMETER					(in_out_query);

		mutable_buffer const raw_file_data			((pbyte)in_out_unmanaged_resource_buffer.c_ptr() + sizeof(test_unmanaged_resource), in_out_query.get_raw_file_size());

		test_unmanaged_resource * out_resource	=	new	(in_out_unmanaged_resource_buffer.c_ptr())	
													test_unmanaged_resource
													(in_out_query.get_requested_path(),
													 raw_file_data,
													 in_out_query.creation_data_from_user() ? creation_from_memory : creation_from_file);
	
		out_resource->test_consistency				();
		in_out_query.set_unmanaged_resource			(out_resource, m_allocate_in_current_thread ?
													 nocache_memory : managed_memory,
													 in_out_unmanaged_resource_buffer.size());
		return										result_success;
	}

	virtual void			create_resource			(query_result_for_cook &	in_out_query,
													 mutable_buffer				in_out_unmanaged_resource_buffer)
	{
		result_enum const out_result			=	create_resource_impl(in_out_query, in_out_unmanaged_resource_buffer);
		in_out_query.finish_query					(out_result);
	}

private:
	bool											m_inplace_in_creation_or_inline_data;
	bool											m_allocate_in_current_thread;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_TEST_INPLACE_UNMANAGED_H_INCLUDED