////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mount_physical_path.h"
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

using namespace fs_new;

void   physical_path_mounter::mount_lazy	()
{
	R_ASSERT								(m_args.submount_node);
	R_ASSERT								(m_args.submount_node->is_physical() && m_args.submount_node->is_folder());

	LOG_INFO								("lazy mount of '%s' on '%s' %s", 
											 m_args.physical_path.c_str(), m_args.virtual_path.c_str(),
											 m_args.recursive ? "(recursive)" : "");

	physical_folder_node<> * folder		=	node_cast<physical_folder_node>(m_args.submount_node);
	m_mount_root_base					=	m_args.submount_node->get_mount_root();
	bool const is_already_scanned		=	!folder->set_is_scanned(m_args.recursive == recursive_true);
	// folder can be set as scanned concurrently by other thread
	if ( !is_already_scanned )
	{
		virtual_path_string	virtual_path	=	m_args.virtual_path;
		native_path_string	physical_path	=	get_node_physical_path(m_args.submount_node);

		u32 const hash					=	path_crc32(m_args.virtual_path);
		mount_physical_folder				(m_args.virtual_path, folder, physical_path, hash);
	}

	m_mount_ptr							=	m_mount_root_base->mount;
}

} // namespace vfs
} // namespace xray
