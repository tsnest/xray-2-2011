////////////////////////////////////////////////////////////////////////////
//	Created		: 03.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_FINDING_ENVIRONMENT_H_INCLUDED
#define VFS_FINDING_ENVIRONMENT_H_INCLUDED

#include <xray/vfs/platform_configuration.h>
#include <xray/vfs/find_results.h>
#include <xray/vfs/hashset.h>
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

struct XRAY_VFS_API find_struct
{
	vfs_mount_ptr						sub_fat;
	find_enum							find_flags;
 	find_callback						callback;
};

struct find_environment
{
	find_struct *						find_results;
	pcstr 								path_to_find;
	pcstr 								partial_path;
	u32									path_part_index;

	bool	is_full_path				() const { return strings::equal(path_to_find, partial_path); }
	vfs_locked_iterator *				out_iterator;	// for sync
	find_callback						callback;		// for async
	base_node<> *						node;
	base_node<> *						node_parent;
	enum_flags<find_enum>				find_flags;
	virtual_file_system *				file_system;
	vfs_hashset *						hashset() const { return & file_system->hashset; }
	memory::base_allocator *			allocator;
	u32									mount_operation_id;

	find_environment	() 
		:	find_results(NULL), partial_path(NULL), path_to_find(NULL), node(NULL), node_parent(NULL),
			find_flags(0), mount_operation_id(0), out_iterator(NULL), path_part_index(0) {}
};

typedef	find_environment				find_env;

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_FINDING_ENVIRONMENT_H_INCLUDED