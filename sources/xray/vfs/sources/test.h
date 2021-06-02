////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_TEST_H_INCLUDED
#define VFS_TEST_H_INCLUDED

#include <xray/vfs/virtual_file_system.h>
#include <xray/core_test_suite.h>

namespace xray {
namespace vfs {

class vfs_test
{
public:
	void	test							(core_test_suite * suite);

private:
	void	test_inside_mount				();
	bool		test_inside_mount_impl		();
	void	test_mount_unmount				();
	bool		test_mount_unmount_impl		();
	void	test_find						();
	void		test_find_impl				();
	void	test_watcher					();
	void	test_patch						();

private:
	core_test_suite *						m_suite;
	virtual_file_system						m_vfs;
	
	uninitialized_reference<fs_new::asynchronous_device_interface>	m_device;
};

void	log_vfs_iterator				(vfs_iterator const & it);
void	log_vfs_root					(virtual_file_system & vfs, bool assert_empty = false);
void	mount_callback					(mount_result result);
void	indexes_from_iteration			(int iteration, int * i0, int * i1, int * i2, int * i3);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_TEST_H_INCLUDED