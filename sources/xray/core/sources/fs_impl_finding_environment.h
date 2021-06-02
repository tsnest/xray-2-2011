////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_IMPL_FINDING_ENV_H_INCLUDED
#define FS_IMPL_FINDING_ENV_H_INCLUDED

#include <xray/fs_sub_fat_resource.h>
#include <xray/resources_fs_iterator.h>
#include "fs_nodes.h"

namespace xray {
namespace fs {

struct find_env
{
	find_results_struct *						find_results;
	pcstr 										path_to_find;
	pcstr 										partial_path;
	bool										is_full_path() const { return strings::equal(path_to_find, partial_path); }
	resources::sub_fat_pin_fs_iterator	*		out_pin_iterator;
	fat_node<> *								node;
	enum_flags<find_enum>						find_flags;

	find_env () : find_results(NULL), partial_path(NULL), path_to_find(NULL), node(NULL), find_flags(0) {}
};

} // namespace fs
} // namespace xray

#endif // #ifndef FS_IMPL_FINDING_ENV_H_INCLUDED