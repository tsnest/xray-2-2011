#ifndef XRAY_FS_UTILS_H_INCLUDED
#define XRAY_FS_UTILS_H_INCLUDED

#if (XRAY_PLATFORM_WINDOWS == 1) && !defined(MASTER_GOLD)
#	define XRAY_FS_WATCHER_ENABLED	1
#else
#	define XRAY_FS_WATCHER_ENABLED	0
#endif

#include <xray/fs_platform_configuration.h>
#include <xray/fs_path_string.h>

namespace xray {
namespace fs {

#if XRAY_FS_WATCHER_ENABLED
	struct	file_type
	{
		file_type	() : handle(NULL), skip_notifications(true), done_write(false) {}

		FILE *		handle;
		bool		done_write;
		bool		skip_notifications;
#pragma warning (push)
#pragma warning (disable:4200)
		char		file_name[];
#pragma warning (pop)
	};
#else
	typedef	FILE	file_type;
#endif

enum			open_file_enum {	open_file_create		=	1 << 0, 
									open_file_truncate		=	1 << 1,
									open_file_read			=	1 << 2, 
									open_file_write			=	1 << 3, 
									open_file_append		=	1 << 4,		};

XRAY_CORE_API signalling_bool		open_file					(file_type * * out_dest, 
																 enum_flags<open_file_enum> open_flags, 
																 pcstr file_name,
																 bool assert_on_fail = true,
																 bool skip_deassociation_in_fat = true);

XRAY_CORE_API void					close_file					(file_type * file);
XRAY_CORE_API size_t				write_file					(file_type * file, pcvoid data, size_t size);
XRAY_CORE_API size_t				read_file					(file_type * file, pvoid data, size_t size);
XRAY_CORE_API void					set_end_of_file_and_close	(file_type * file);
XRAY_CORE_API void					set_file_size_and_close		(file_type * file, file_size_type size);
XRAY_CORE_API bool					seek_file					(file_type * file, file_size_type offset, int origin);
XRAY_CORE_API file_size_type		tell_file					(file_type * file);
XRAY_CORE_API void					setvbuf						(file_type * stream, char * buffer, int mode, size_t size);
XRAY_CORE_API void					flush_file					(file_type * file);

XRAY_CORE_API void					verify_path_is_portable 	(pcstr path_str);
XRAY_CORE_API signalling_bool		calculate_file_size			(file_size_type * const out_file_size, pcstr const file_path);
XRAY_CORE_API signalling_bool		calculate_file_size			(file_size_type * const out_file_size, FILE * const file_type);
XRAY_CORE_API signalling_bool		calculate_file_size			(file_size_type * const out_file_size, file_type * const file_type);
XRAY_CORE_API bool					make_dir					(pcstr portable_path);
XRAY_CORE_API bool			  		make_dir_r					(pcstr portable_path, bool create_last = true); // recursively creates directories
XRAY_CORE_API void					directory_part_from_path	(path_string * out_result, pcstr path);
XRAY_CORE_API void					directory_part_from_path_inplace (path_string * in_out_result);
XRAY_CORE_API pcstr					file_name_from_path			(pcstr path);
XRAY_CORE_API void					file_name_with_no_extension_from_path	(buffer_string * out_result, pcstr path);
XRAY_CORE_API void					set_extension_for_path		(buffer_string * in_out_result, pcstr extension);
XRAY_CORE_API pcstr					extension_from_path			(pcstr path);
XRAY_CORE_API path_string			convert_to_relative_path	(pcstr physical_path);
XRAY_CORE_API signalling_bool		convert_to_absolute_native_path (buffer_string * out_result, 
																	 pcstr relative_portable_path);
XRAY_CORE_API bool					file_exists					(pcstr path);

struct path_info
{
	enum type_enum					{ type_nothing, type_file, type_folder, type_archive };
	type_enum						type;
	file_size_type					file_size;
	u32								file_last_modify_time;

	path_info						() : type(type_nothing), file_size(0), file_last_modify_time(0) {}
};

XRAY_CORE_API path_info::type_enum	get_path_info				(path_info * out_path_info, pcstr path);

XRAY_CORE_API void					common_prefix_path			(buffer_string * out_common_path, pcstr first_path, pcstr second_path);

XRAY_CORE_API void					set_user_data_folder		(pcstr const user_data_folder);
XRAY_CORE_API pcstr					get_user_data_folder		();
XRAY_CORE_API void					calculate_file_hash			(u32 * out_file_hash, FILE * file_type);
XRAY_CORE_API signalling_bool		calculate_file_hash			(u32 * out_file_hash, pcstr file_path);

XRAY_CORE_API void					make_relative_path			(buffer_string * out_relative_path, pcstr original_path, pcstr reference_path);
XRAY_CORE_API signalling_bool		append_relative_path		(buffer_string * in_out_path, pcstr relative_path);

XRAY_CORE_API path_info::type_enum  get_path_info_by_logical_path	(path_info *				out_path_info, 
																	 pcstr						logical_path, 
																	 memory::base_allocator *	allocator);
XRAY_CORE_API path_info::type_enum  get_physical_path_info_by_logical_path (path_info * out_path_info, pcstr logical_path);

XRAY_CORE_API u32					last_modify_time_by_logical_path	(pcstr logical_path, memory::base_allocator * allocator);
XRAY_CORE_API FILE *				file_type_to_FILE			(file_type *);


} // namespace fs
} // namespace xray

#endif // #ifndef XRAY_FS_UTILS_H_INCLUDED