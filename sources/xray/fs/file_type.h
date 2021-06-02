////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_FILE_TYPE_H_INCLUDED
#define XRAY_FS_FILE_TYPE_H_INCLUDED

namespace xray {
namespace fs_new {

namespace file_mode {
	enum mode_enum { create_always, open_existing, append_or_create };
} // namespace file_mode
namespace file_access {
	enum access_enum { write, read, read_write };
} // namespace file_access

enum 	use_buffering_bool	{ use_buffering_false, use_buffering_true   };
enum 	notify_watcher_bool	{ notify_watcher_false, notify_watcher_true };

enum	seek_file_enum		{ seek_file_begin, seek_file_current, seek_file_end	};

struct open_file_params
{
	file_mode::mode_enum		mode;
	file_access::access_enum	access;
	assert_on_fail_bool			assert_on_fail;
	notify_watcher_bool			notify_watcher;
	use_buffering_bool			use_buffering;

	// this flag is used internally by subsystems that work before memory allocators are created
	bool						file_type_allocated_by_user;

	open_file_params	(file_mode::mode_enum		mode,
						 file_access::access_enum	access,						 
						 assert_on_fail_bool		assert_on_fail = assert_on_fail_false,
						 notify_watcher_bool		notify_watcher = notify_watcher_true,
						 use_buffering_bool			use_buffering  = use_buffering_true)
		:	mode(mode), access(access), assert_on_fail(assert_on_fail), file_type_allocated_by_user(false),
			notify_watcher(notify_watcher), use_buffering(use_buffering) {}
};

#if XRAY_FS_NEW_WATCHER_ENABLED
	struct	file_type
	{
		file_type	() : handle(NULL), flags(flags_skip_notifications) {;}

		pvoid		handle;
		enum		flags_enum			{	flags_done_write			=	1 << 0,
											flags_skip_notifications	=	1 << 1,
											flags_owned_by_pointer		=	1 << 2,	
											flags_allocated_by_user		=	1 << 3,	};

		void		set_skip_notifications	(bool value) { flags.set(flags_skip_notifications, value); }
		void		set_done_write			(bool value) { flags.set(flags_done_write, value); }
		void		set_owned_by_pointer	(bool value) { flags.set(flags_owned_by_pointer, value); }
		void		set_allocated_by_user	(bool value) { flags.set(flags_allocated_by_user, value); }
		bool		skip_notifications		() const { return flags.has(flags_skip_notifications); }
		bool		done_write				() const { return flags.has(flags_done_write); }
		bool		owned_by_pointer		() const { return flags.has(flags_owned_by_pointer); }
		bool		allocated_by_user		() const { return flags.has(flags_allocated_by_user); }

		flags_type<flags_enum>			flags;
#pragma warning (push)
#pragma warning (disable:4200)
		char		file_name[];
#pragma warning (pop)
	};

	inline pvoid	file_type_to_handle		(file_type * file) { return	file->handle; }
	inline void		set_file_type_handle	(file_type * file, pvoid handle) { file->handle	=	handle; }
	inline void		set_owned_by_pointer	(file_type * file, bool value) { file->set_owned_by_pointer(value); }

#else // #if XRAY_FS_NEW_WATCHER_ENABLED

#pragma message(XRAY_TODO("Lain 2 Lain: revisit correctness of this casts"))
	typedef	pvoid	file_type;
	inline pvoid	file_type_to_handle		(file_type * file) { return	file; }
	inline void		set_file_type_handle	(file_type * file, pvoid handle) { * file	=	handle; }
	inline void		set_owned_by_pointer	(file_type * file, bool value) { XRAY_UNREFERENCED_PARAMETERS(file, value); }

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

inline fixed_string<128>   file_open_flags_to_string (file_mode::mode_enum const	mode, 
													  file_access::access_enum		access)
{
	fixed_string<128>	out_string;
	if ( mode == file_mode::create_always )
		out_string	+=	"create_always";
	else if ( mode == file_mode::open_existing )
		out_string	+=	"open_existing";
	else if ( mode == file_mode::append_or_create )
		out_string	+=	"append_or_create";

	if ( access == file_access::write )
		out_string	+=	"+write";
	else if ( access == file_access::read )
		out_string	+=	"+read";
	else if ( access== file_access::read_write )
		out_string	+=	"+read_write";
	return				out_string;
}

} // namespace fs_new
} // namespace xray

#endif // #ifndef XRAY_FS_FILE_TYPE_H_INCLUDED