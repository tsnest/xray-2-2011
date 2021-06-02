////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_PATH_STRING_UTILS_H_INCLUDED
#define XRAY_FS_PATH_STRING_UTILS_H_INCLUDED

#include <xray/type_extensions.h>
#include <xray/fs/native_path_string.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/fs/file_type.h>

namespace xray {
namespace fs_new {

template <class in_path_type>
signalling_bool		convert_to_absolute_path	(native_path_string *	out_result, 
												 in_path_type const &	relative_path, 
												 assert_on_fail_bool	assert_on_fail);

signalling_bool		convert_to_absolute_path_inplace	(native_path_string *	in_out_path,
														 assert_on_fail_bool	assert_on_fail);

XRAY_FS_API native_path_string const &	get_current_directory ();

template <class out_path_type, class in_path_type>
signalling_bool   append_relative_path			(out_path_type * in_out_path, in_path_type const & in_relative_path);

template <class out_path_type, class in_path_type>
void		convert_to_relative_path			(out_path_type *		out_relative_path, 
								 				 in_path_type const &	absolute_path,
								 				 in_path_type const &	root_to_relate = in_path_type(get_current_directory()));

template <class in_out_path_type>
void		convert_to_relative_path_inplace	(in_out_path_type *			in_out_path,
								 				 in_out_path_type const &	root_to_relate = in_out_path_type(get_current_directory()));

template <class out_path_type>
void		get_path_without_last_item			(out_path_type * out_result, pcstr path);

template <class in_out_path_type>
void		cut_last_item_from_path				(in_out_path_type * in_out_result);

template <class in_path_type>
pcstr		file_name_from_path					(in_path_type const & path);

template <class in_path_type>
void		file_name_with_no_extension_from_path	(in_path_type * out_result, pcstr path);

template <class in_out_path_type>
void		set_extension_for_path			(in_out_path_type * in_out_result, pcstr extension);

template <class in_path_type>
pcstr		extension_from_path				(in_path_type const & path);

inline
pcstr		extension_from_path				(pcstr path);

u32			crc32							(u32 starting_hash, u32 hash_to_combine);
u32			crc32							(pcstr data, u32 size, u32 start_value = 0);
u32			path_crc32						(pcstr data, u32 size, u32 start_value = 0);

template <class StringType> inline
u32	  path_crc32							(StringType const & string, u32 start_value = 0)
{ 
	return									path_crc32(string.c_str(), string.length(), start_value);
}

inline
bool   path_starts_with						(pcstr path, pcstr with);

} // namespace fs_new
} // namespace xray

#include <xray/fs/path_string_utils_inline.h>

#endif // #ifndef XRAY_FS_PATH_STRING_UTILS_H_INCLUDED