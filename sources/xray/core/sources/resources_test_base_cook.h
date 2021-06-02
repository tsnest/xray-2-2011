////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_TEST_BASE_COOK_H_INCLUDED
#define RESOURCES_TEST_BASE_COOK_H_INCLUDED

#include "resources_test_resource.h"
#include <xray/fs/native_path_string.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/fs/synchronous_device_interface.h>
#include <xray/core_entry_point.h>

namespace xray {
namespace resources {

extern	fixed_vector<fs_new::native_path_string, 16>	s_created_files;

enum generate_if_no_file_test_enum	{ generate_if_no_file_test_unset, generate_if_no_file_test_requery, generate_if_no_file_test_save_generated };

class test_cook_base
{
public:
	test_cook_base (generate_if_no_file_test_enum generate_if_no_file_test) : m_generate_if_no_file_test(generate_if_no_file_test) {}

	generate_if_no_file_test_enum	generate_if_no_file_test () const { return m_generate_if_no_file_test; }

	bool    save_generated				(query_result_for_cook &	in_out_query, 
										 mutable_buffer const &		generated_file_data)
	{
		fs_new::virtual_path_string const	virtual_path	=	in_out_query.get_requested_path();
		fs_new::native_path_string		physical_path;
		if ( !resources::convert_virtual_to_physical_path(& physical_path, virtual_path, "disk") )
		{
			in_out_query.finish_query		(cook_base::result_error);
			return							false;
		}
		s_created_files.push_back			(physical_path.c_str());
		in_out_query.save_generated_resource	(save_generated_data::create(generated_file_data, true, physical_path, virtual_path));
		return								true;
	}

	void   on_generate_helper_resource_loaded (queries_result & result)
	{
		query_result_for_cook * const	parent	=	result.get_parent_query();
		fs_new::native_path_string		disk_path;

		if ( !resources::convert_virtual_to_physical_path(& disk_path, parent->get_requested_path(), "disk") )
		{
			parent->finish_query	(cook_base::result_error);
			return;
		}

		using namespace fs_new;

		synchronous_device_interface &	device	=	core::get_core_synchronous_device( );

		file_type_pointer	file			(disk_path, device, file_mode::create_always, file_access::write, assert_on_fail_false);
		if ( !file )
		{
			parent->finish_query			(cook_base::result_error);
			return;
		}

		mutable_buffer						generated_file_data(ALLOCA(generate_file_data_size), generate_file_data_size);
		fill_with_generated_file_data		(generated_file_data);		
		device->write						(file, generated_file_data.c_ptr(), generated_file_data.size());
		file.close							();

		s_created_files.push_back			(disk_path.c_str());
		parent->finish_query				(cook_base::result_requery);
	}

private:
	generate_if_no_file_test_enum	m_generate_if_no_file_test;
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_TEST_BASE_COOK_H_INCLUDED
