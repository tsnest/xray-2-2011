////////////////////////////////////////////////////////////////////////////
//	Created		: 14.01.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>
#include <xray/buffer_string.h>
#include "fs_file_system.h"
#include "fs_impl.h"

namespace xray {
namespace fs {

void   verify_path_is_portable (pcstr path_str)
{
	XRAY_UNREFERENCED_PARAMETER	( path_str );
	ASSERT						( path_str );
	ASSERT						( !strchr(path_str, '\\') );

#ifndef MASTER_GOLD
	u32 const path_length	=	strings::length(path_str);
	R_ASSERT					( !path_length || path_str[path_length-1] != '/', 
								  "path should not end with '/' ('%s')", path_str );
#endif // #ifdef DEBUG
}

void   file_name_with_no_extension_from_path (buffer_string * const out_result, pcstr const path)
{
	verify_path_is_portable			(path);

	pcstr file_name_start		=	0;
	
	if ( pcstr const last_slash_pos = strrchr(path, '/') )
		file_name_start			=	last_slash_pos + 1;
	else
		file_name_start			=	path;

	pcstr const last_point_pos	=	strrchr(path, '.');

	if ( last_point_pos && last_point_pos > file_name_start )
		out_result->assign			(file_name_start, last_point_pos);
	else
		* out_result			=	file_name_start;
}

void   set_extension_for_path (buffer_string * in_out_result, pcstr extension)
{
	u32 const point_pos			=	in_out_result->rfind('.');
	u32 const slash_pos			=	in_out_result->rfind('/');
	bool const file_name_without_extension	=	point_pos == u32(-1) || 
												(slash_pos != u32(-1) && point_pos < slash_pos);
	if ( file_name_without_extension )
	{
		CURE_ASSERT					(in_out_result->length(), return);
		in_out_result->appendf		(".%s", extension);
		return;
	}

	in_out_result->set_length		(point_pos + 1);
	* in_out_result				+=	extension;
}

pcstr   extension_from_path (pcstr path)
{
	verify_path_is_portable			(path);
	
	if ( pcstr const last_point_pos = strrchr(path, '.') )
		return						last_point_pos + 1;
	else
		return						path + strings::length(path);
}

void   directory_part_from_path (path_string * const out_result, pcstr const path)
{
	verify_path_is_portable			(path);

	if ( pcstr const last_slash_pos = strrchr(path, '/') )
		out_result->assign			(path, last_slash_pos);
	else
		* out_result			=	"";
}

void   directory_part_from_path_inplace (path_string * const in_out_result)
{
	u32 const last_slash_pos	=	in_out_result->rfind('/');
	if ( last_slash_pos == u32(-1) )
		in_out_result->set_length	(0);
	else
		in_out_result->set_length	(last_slash_pos);
}

pcstr   file_name_from_path (pcstr const path)
{
	verify_path_is_portable			(path);

	if ( pcstr const last_slash_pos = strrchr(path, '/') )
		return						last_slash_pos + 1;
	else
		return						path;
}

void   common_prefix_path (buffer_string * out_common_path, pcstr first_path, pcstr second_path)
{
	verify_path_is_portable				(first_path);
	verify_path_is_portable				(second_path);
	get_common_prefix					(out_common_path, first_path, second_path);
	
	u32 const common_path_length	=	out_common_path->length();
	if ( (first_path[common_path_length] && first_path[common_path_length] != '/') ||
		 (second_path[common_path_length] && second_path[common_path_length] != '/') )
	{
		u32 const last_slash_pos	=	out_common_path->rfind('/');
		if ( last_slash_pos == u32(-1) )
			* out_common_path		=	"";
		else
			out_common_path->set_length	(last_slash_pos);
	}

	if ( out_common_path->last() == '/' )
		out_common_path->rtrim			();
}

void   make_relative_path (buffer_string * out_relative_path, pcstr original_path, pcstr reference_path)
{
		u32 const count_of_slashes_in_original	=	strings::count_of(original_path, '/');
			path_string								common_path;
			common_prefix_path						(& common_path, original_path, reference_path);
		u32 const count_of_slashes_in_common	=	strings::count_of(common_path.c_str(), '/');
		u32 const count_of_parent_parts			=	count_of_slashes_in_original - count_of_slashes_in_common;

	out_relative_path->append_repeat				("../", count_of_parent_parts);
	out_relative_path->append						(reference_path + common_path.length());
}

signalling_bool   append_relative_path (buffer_string * in_out_path, pcstr relative_path)
{
	char const up_token[]						=	"../";
	u32 const up_token_length					=	strings::length(up_token);
	while ( strstr(relative_path, up_token) == relative_path )
	{
		relative_path							+=	up_token_length;
		u32 const last_slash					=	in_out_path->rfind('/');
		if ( last_slash == u32(-1) )
			return									false;
		in_out_path->set_length						(last_slash);
	}

	if ( * relative_path )
	{
		* in_out_path							+=	"/";
		* in_out_path							+=	relative_path;
	}

	return											true;
}

struct get_path_info_predicate
{
public:
	get_path_info_predicate	(path_info::type_enum *	out_result, 
							 path_info *			out_info)
	: m_out_info(out_info), m_out_result(out_result) {}

	void callback (resources::fs_iterator it)
	{
		path_string	disk_path;
		if ( it.is_end() )
		{
			* m_out_result	=	path_info::type_nothing;
			if ( m_out_info )
				m_out_info->type	=	path_info::type_nothing;
		}
		else
		{
			it.get_disk_path		(disk_path);
			* m_out_result		=	get_path_info(m_out_info, disk_path.c_str());
		}
	}

	path_info *				m_out_info;
	path_info::type_enum *	m_out_result;
};

path_info::type_enum   get_path_info_by_logical_path (path_info *				out_path_info, 
													  pcstr						logical_path, 
													  memory::base_allocator *	allocator)
{
	XRAY_UNREFERENCED_PARAMETER				(allocator);
	return	get_physical_path_info_by_logical_path	(out_path_info, logical_path);

// 	path_info::type_enum		out_result = path_info::type_nothing;
// 	get_path_info_predicate		predicate(& out_result, out_path_info);
// 
// 	resources::query_fs_iterator_and_wait	(logical_path, 
// 											 boost::bind(& get_path_info_predicate::callback, & predicate, _1),
// 											 allocator, resources::recursive_false, NULL, true);
// 
// 	return						out_result;
}

path_info::type_enum   get_physical_path_info_by_logical_path (path_info * out_path_info, pcstr logical_path)
{
	return	g_fat->impl()->get_physical_path_info_by_logical_path(out_path_info, logical_path);
}

} // namespace fs
} // namespace xray