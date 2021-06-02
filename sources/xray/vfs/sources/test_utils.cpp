////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/fs/asynchronous_device_interface.h>
#include <xray/fs/windows_hdd_file_system.h>
#include "test.h"
#include <xray/vfs/watcher.h>
#include "mount_archive.h"
#include "find.h"
#include "branch_locks.h"

namespace xray {
namespace vfs {

void   log_vfs_nodes					(base_node<> * node, u32 level = 0, pcstr original_name = NULL);

using namespace fs_new;

void   log_vfs_iterator					(vfs_iterator const & it)
{
	log_vfs_nodes							(it.get_node());
}

void   mount_callback					(mount_result result)
{
	R_ASSERT								(result.mount && result.result == vfs::result_success);
}

void   indexes_from_iteration			(int iteration, int * i0, int * i1, int * i2, int * i3)
{
	R_ASSERT								(iteration < 24);

	int remainder						=	iteration;

	int index0							=	remainder / 6;
	remainder							%=	6;
	int index1							=	remainder / 2;
	remainder							%=	2;
	int index2							=	remainder;

	int indexes[]						=	{0,1,2,3};
	std::swap								(indexes[0], indexes[index0]);
	std::swap								(indexes[1], indexes[1+index1]);
	std::swap								(indexes[2], indexes[2+index2]);

	* i0								=	indexes[0];
	* i1								=	indexes[1];
	* i2								=	indexes[2];
	* i3								=	indexes[3];
}

} // namespace vfs
} // namespace xray