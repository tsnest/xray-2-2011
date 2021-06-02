/////////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
/////////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_FOLDER_WATCHER_H_INCLUDED
#define XRAY_FS_FOLDER_WATCHER_H_INCLUDED

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/os_preinclude.h>
#include <xray/os_include.h>
#include <xray/fs/native_path_string.h>
#include <xray/fs/watcher_notification.h>

namespace xray {
namespace fs_new {

class folder_watcher : private core::noncopyable 
{
public:
					folder_watcher					(native_path_string const &			path, 
													 fs_notification_callback const &	callback,
													 threading::event *					receive_changes_event);
					~folder_watcher					();

		void		try_receive_changes_sync		();
		void		try_receive_changes_async		();
		pcstr		path							() const { return m_path.c_str(); }

private:
		enum		swap_buffers_bool				{	swap_buffers_false, swap_buffers_true	};

		void		subscribe_to_receive_changes	(swap_buffers_bool swap);
		void		receive_changes					(FILE_NOTIFY_INFORMATION * in_buffer);

public: // really private
	folder_watcher *			m_next_watcher; // for intrusive_double_linked_list
	folder_watcher *			m_prev_watcher; // for intrusive_double_linked_list

private:
	// set 128 to avoid overflows buffer should be less than 64K to avoid Windows problem with network watches (see MSDN)
	u8							m_buffer[2][ 128*1024 ];
	native_path_string			m_path;
	threading::event *			m_receive_changes_event;
	pbyte						m_current_buffer;
	pbyte						m_next_buffer;
	OVERLAPPED					m_overlapped[2];
	fs_notification_callback	m_callback;
	HANDLE						m_handle;

}; // class folder_watcher

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

#endif // #ifndef XRAY_FS_FOLDER_WATCHER_H_INCLUDED