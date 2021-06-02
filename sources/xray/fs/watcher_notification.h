////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_WATCHER_NOTIFICATION_H_INCLUDED
#define FS_WATCHER_NOTIFICATION_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/fs/api.h>
#include <xray/fs/native_path_string.h>

namespace xray {
namespace fs_new {

struct XRAY_FS_API fs_notification
{
	enum			type_enum			{	type_unset,
											type_added, 
											type_removed,
											type_modified,
											type_renamed,	};

	fs_notification						();

	// factory functions:
	static fs_notification *	create	(memory::base_allocator *		allocator, 
										 type_enum const				type,
										 native_path_string const &		path);

	static fs_notification *	create	(memory::base_allocator *		allocator, 
										 fs_notification const &		other);

	static fs_notification *	create_rename	
										(memory::base_allocator *		allocator, 
										 native_path_string const &		old_path,
										 native_path_string const &		new_path);

	u32									size () const { return sizeof(fs_notification) + 
														(path ? strings::length(path) + 1 : 0) +
														(old_path ? strings::length(old_path) + 1 : 0) +
														(new_path ? strings::length(new_path) + 1 : 0); }

public:
	type_enum							type;
	pstr								path;
	pstr								old_path;
	pstr								new_path;

	fs_notification *					next;
};

enum	watcher_type_enum						{ watcher_type_synchronous, watcher_type_asynchronous, };

typedef boost::function< void (fs_notification const & info) >	fs_notification_callback;

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef FS_WATCHER_NOTIFICATION_H_INCLUDED