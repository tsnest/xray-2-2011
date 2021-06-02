////////////////////////////////////////////////////////////////////////////
//	Created		: 07.11.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_PERFORMANCE_TEST_APPLICATION_H_INCLUDED
#define RESOURCES_PERFORMANCE_TEST_APPLICATION_H_INCLUDED

#include <xray/fs/simple_asynchronous_interface.h>
#include <xray/fs/simple_synchronous_interface.h>
#include <xray/fs/synchronous_device_interface.h>

namespace xray {
namespace resources_performance_test {

enum { max_files_to_request = 2048 };

struct path_pair
{
	fs_new::virtual_path_string			virtual_path;
	fs_new::native_path_string			physical_path;
	u32									size;
};

typedef fixed_vector<path_pair, max_files_to_request>	files_array;

class application {
public:
			void	initialize		( );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const	{ return m_exit_code; }

private:
	u32 			test_without_resources_manager	(u32 * out_hash);
	u32 			test_with_resources_manager		(u32 * out_hash);
	void			test				(u32 const								files_count, 
										 u32 const								average_file_size);
	void			create_test_files	(fs_new::native_path_string				folder,
										 u32 const								files_count, 
										 u32 const								average_file_size,
										 fs_new::synchronous_device_interface & device);


	uninitialized_reference< fs_new::simple_asynchronous_interface >	m_fs_devices;
	//uninitialized_reference< fs_new::simple_synchronous_interface >		m_fs_devices;

	files_array		m_files;
	u32				m_exit_code;
}; // class application

} // namespace resources_performance_test
} // namespace xray

#endif // #ifndef RESOURCES_PERFORMANCE_TEST_APPLICATION_H_INCLUDED