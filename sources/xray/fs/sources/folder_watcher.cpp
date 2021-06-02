/////////////////////////////////////////////////////////////////////////////////
//	Created 	: 01.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
/////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_FS_NEW_WATCHER_ENABLED

#include <xray/fs/folder_watcher.h>
#include <xray/fs/watcher_utils.h>

namespace xray {
namespace fs_new {

folder_watcher::folder_watcher (native_path_string const &			path, 
								fs_notification_callback const &	callback, 
								threading::event *					receive_changes_event) 
							:	m_path								(path),
								m_callback							(callback),
								m_current_buffer					(m_buffer[0]),
								m_next_buffer						(m_buffer[1]),
								m_receive_changes_event				(receive_changes_event)
{
	DWORD flags							=	FILE_FLAG_BACKUP_SEMANTICS;
	if ( receive_changes_event )
		flags							|=	FILE_FLAG_OVERLAPPED;

	m_handle							=	CreateFile (path.c_str(),
														FILE_LIST_DIRECTORY,
														FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
														0,
														OPEN_EXISTING,
														flags,
														0);

	R_ASSERT								(m_handle != INVALID_HANDLE_VALUE);

	memory::zero							(& m_overlapped[0], sizeof(m_overlapped[0]));
	memory::zero							(& m_overlapped[1], sizeof(m_overlapped[1]));
	if ( !m_receive_changes_event )
		try_receive_changes_sync			();
	else
		subscribe_to_receive_changes		(swap_buffers_false);
}

folder_watcher::~folder_watcher	()
{
	m_overlapped[0].hEvent				=	NULL;
	m_overlapped[1].hEvent				=	NULL;
	R_ASSERT								(m_handle != INVALID_HANDLE_VALUE);
	CloseHandle								(m_handle);
}

void   folder_watcher::subscribe_to_receive_changes (swap_buffers_bool const swap)
{
	if ( swap == swap_buffers_true )
		std::swap							(m_current_buffer, m_next_buffer);

	OVERLAPPED * const overlapped		=	& m_overlapped[ ((pvoid)m_current_buffer == & m_buffer[0]) ? 0 : 1 ];

	R_ASSERT								(m_receive_changes_event);
	overlapped->hEvent					=	m_receive_changes_event->get_handle();

	BOOL const result					=	ReadDirectoryChangesW ( m_handle,
																	m_current_buffer,
																	sizeof(m_buffer[0]),
																	TRUE,
 																		FILE_NOTIFY_CHANGE_FILE_NAME |
 																		FILE_NOTIFY_CHANGE_DIR_NAME |
																		FILE_NOTIFY_CHANGE_LAST_WRITE,
																	NULL,
																	overlapped,
																	NULL );
	R_ASSERT_U								(result);
}

void   folder_watcher::try_receive_changes_sync	()
{
	R_ASSERT								(!m_receive_changes_event);
	DWORD bytes_received				=	0;
	BOOL const result					=	ReadDirectoryChangesW ( m_handle,
																	m_current_buffer,
																	sizeof(m_buffer[0]),
																	TRUE,
 																		FILE_NOTIFY_CHANGE_FILE_NAME |
 																		FILE_NOTIFY_CHANGE_DIR_NAME |
																		FILE_NOTIFY_CHANGE_LAST_WRITE,
																	& bytes_received,
																	NULL,
																	NULL );
	R_ASSERT_U								(result);

	if ( bytes_received )
		receive_changes						((FILE_NOTIFY_INFORMATION *)m_current_buffer);
}

void   folder_watcher::try_receive_changes_async ()
{
	DWORD bytes_transfered				=	0;

	OVERLAPPED * const overlapped		=	& m_overlapped[ ((pvoid)m_current_buffer == & m_buffer[0]) ? 0 : 1 ];

	BOOL const overlapped_successfull	=	GetOverlappedResult(m_handle, overlapped, &bytes_transfered, false);

	if ( !overlapped_successfull )
	{
		u32 const last_error			=	GetLastError();
		if ( last_error == ERROR_IO_INCOMPLETE )
			return;

		if ( last_error == ERROR_OPERATION_ABORTED )
			return;

		FATAL								("Async operation failed. ErrorCode: %d", last_error);
	}

	if ( !bytes_transfered )
	{
		subscribe_to_receive_changes		(swap_buffers_false);
		return;
	}

	subscribe_to_receive_changes			(swap_buffers_true);
	receive_changes							((FILE_NOTIFY_INFORMATION *)m_next_buffer);
}

static inline
FILE_NOTIFY_INFORMATION *   next_file_notify_information	(FILE_NOTIFY_INFORMATION * const info)
{
	if ( !info->NextEntryOffset )
		return								0;

	return									(FILE_NOTIFY_INFORMATION*)(((u8*)info) + info->NextEntryOffset);
}

static 
bool   get_file_name_from_file_notify_information (FILE_NOTIFY_INFORMATION * const	info, 
												   native_path_string *		out_file_name)
{
	u32 const file_name_length			=	info->FileNameLength / 2;

	CURE_ASSERT_CMP							(file_name_length, <, MAX_PATH, return false);
	size_t	size_in_bytes				=	(file_name_length + 1) * 2;
	pstr	result						=	(pstr)ALLOCA((u32)size_in_bytes);
	WCHAR	wc_file_name[MAX_PATH + 1]	=	{ 0 };
	memory::copy							(wc_file_name, 
											 sizeof(wc_file_name), 
											 info->FileName, 
											 (DWORD) std::min( MAX_PATH * sizeof(WCHAR), file_name_length * sizeof(WCHAR)));
	wc_file_name[file_name_length]		=	0;

	size_t converted_characters			=	0;
	errno_t	const error					=	wcstombs_s	(&converted_characters, result, size_in_bytes, wc_file_name, size_in_bytes);
	R_ASSERT_U								(!error);

	* out_file_name						=	native_path_string::convert(result);
	
	out_file_name->make_lowercase			();
	return									true;
}

void   folder_watcher::receive_changes	(FILE_NOTIFY_INFORMATION * in_buffer)
{
	FILE_NOTIFY_INFORMATION * current	=	in_buffer;

	fs_notification::type_enum type		=	fs_notification::type_unset;
	native_path_string						path;
	native_path_string						old_path;
	native_path_string						new_path;
	
	while ( current ) 
	{
		if ( type != fs_notification::type_unset )
		{
			type						=	fs_notification::type_unset;
			path						=	"";
			old_path					=	"";
			new_path					=	"";
		}

		native_path_string					relative_path;
		if ( !get_file_name_from_file_notify_information(current, & relative_path) 
			  || relative_path.find(".svn") != relative_path.npos )
		{
			current						=	next_file_notify_information(current);
			continue;
		}

		path.assignf						("%s%c%s", m_path.c_str(), native_path_string::separator, relative_path.c_str());
				
		switch ( current->Action ) 
		{
			case FILE_ACTION_ADDED :	type	=	fs_notification::type_added;	break;
			case FILE_ACTION_REMOVED :	type	=	fs_notification::type_removed;	break;
			case FILE_ACTION_MODIFIED :	type	=	fs_notification::type_modified;	break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				old_path.assignf			("%s%c%s", m_path.c_str(), native_path_string::separator, relative_path.c_str());

				if ( new_path )
					type				=	fs_notification::type_renamed;
				break;

			case FILE_ACTION_RENAMED_NEW_NAME:	
				new_path.assignf			("%s%c%s", m_path.c_str(), native_path_string::separator, relative_path.c_str());

				if ( old_path )
					type				=	fs_notification::type_renamed;
				break;
		};

		if ( type != fs_notification::type_unset )
		{
			if ( type == fs_notification::type_renamed )
				path					=	"";

			path.make_lowercase				();
			new_path.make_lowercase			();
			old_path.make_lowercase			();

			fs_notification					notification;
			notification.type			=	type;
			notification.path			=	(pstr)path.c_str();
			notification.new_path		=	(pstr)new_path.c_str();
			notification.old_path		=	(pstr)old_path.c_str();

			m_callback						(notification);
			type						=	fs_notification::type_unset;
		}

		current							=	next_file_notify_information(current);
	}

	R_ASSERT								(type == fs_notification::type_unset);
}

} // namespace fs_new
} // namespace xray

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED