////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_WATCHER_UTILS_H_INCLUDED
#define FS_WATCHER_UTILS_H_INCLUDED

#include <xray/fs/file_type.h>
#include <xray/fs/native_path_string.h>

namespace xray {
namespace fs_new {

void   skip_deassociation_notifications (file_type *					file, 
										 u32 const						notifications_to_skip);
void   skip_deassociation_notifications (native_path_string const &		absolute_physical_path,
										 u32 const						notifications_to_skip);

bool   try_remove_deassociation_notifications (native_path_string const &	physical_path, 
											   u32 const notifications_to_skip = 1);

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_WATCHER_UTILS_H_INCLUDED