////////////////////////////////////////////////////////////////////////////
//	Created		: 17.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VFS_VFS_PATCH_ARGS_H_INCLUDED
#define XRAY_VFS_VFS_PATCH_ARGS_H_INCLUDED

namespace xray {
namespace vfs {

struct patch_args : public pack_archive_args
{
	fs_new::native_path_string				from_sources;
	fs_new::native_path_string				to_sources;

	patch_args	(fs_new::synchronous_device_interface &		synchronous_device,
				 logging::log_format * const				log_format,
				 logging::log_flags_enum const				log_flags)
		:	pack_archive_args(synchronous_device, log_format, log_flags) {}
};

bool   make_patch						(patch_args & args);

} // namespace vfs
} // namespace xray

#endif // #ifndef XRAY_VFS_VFS_PATCH_ARGS_H_INCLUDED