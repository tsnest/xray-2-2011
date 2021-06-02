////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_PATH_STRING_UTILS_INLINE_H_INCLUDED
#define XRAY_FS_PATH_STRING_UTILS_INLINE_H_INCLUDED

namespace xray {
namespace fs_new {

template <class out_path_type, class in_path_type>
struct select_separator
{
	enum { separator = in_path_type::separator };
};

template <class out_path_type>
struct select_separator<out_path_type, pcstr>
{
	enum { separator = out_path_type::separator };
};

template <class string_type>
inline pcstr c_str						(string_type const & string)
{
	return									string.c_str();
}

inline pcstr c_str						(pcstr string)
{
	return									string;
}

template <class out_path_type, class in_path_type>
signalling_bool   append_relative_path	(out_path_type * in_out_path, in_path_type const & in_relative_path)
{
	char up_token[]						=	{ '.', '.', select_separator<out_path_type, in_path_type>::separator, 0 };
	u32 const up_token_length			=	strings::length(up_token);
	pcstr	relative_path				=	c_str(in_relative_path);

	while ( strstr(relative_path, up_token) == relative_path )
	{
		relative_path					+=	up_token_length;
		u32 const last_slash			=	in_out_path->rfind(out_path_type::separator);
		if ( last_slash == u32(-1) )
		{
			if ( !in_out_path->length() )
				return						false;
			in_out_path->set_length			(0);
		}
		else
			in_out_path->set_length			(last_slash);
	}

	if ( * relative_path )
	{
		if ( in_out_path->length() )
			* in_out_path				+=	out_path_type::separator;
		in_out_path->append_with_conversion	(relative_path);
	}

	return									true;
}

template <class path_type>
bool   is_absolute_path					(path_type const & path)
{
	typename path_type::const_iterator	it		=	path.begin();
	typename path_type::const_iterator	it_end	=	path.end();
	for ( ; it != it_end; ++it )
	{
		if ( * it == ':' )
			return							true;
		else if ( * it == path_type::separator )
			return							false;
	}
	return									false;
}

template <class path_type>
signalling_bool   convert_to_absolute_path (native_path_string * out_result, path_type const & relative_path, assert_on_fail_bool assert_on_fail)
{
	R_ASSERT								((pvoid)out_result != (pvoid)& relative_path);
	if ( is_absolute_path(relative_path) )
	{
		* out_result					=	(native_path_string)relative_path;
		return								true;
	}

	* out_result						=	get_current_directory ();
	if ( !append_relative_path(out_result, relative_path) )
	{
		if ( assert_on_fail )
			FATAL							("cannot convert to absolute path: %s", relative_path.c_str());
		return								false;
	}

	return									true;
}

inline
signalling_bool		convert_to_absolute_path_inplace	(native_path_string *	in_out_path,
														 assert_on_fail_bool	assert_on_fail)
{
	native_path_string						absolute_path;
	if ( convert_to_absolute_path(& absolute_path, * in_out_path, assert_on_fail) )
	{
		* in_out_path					=	absolute_path;
		return								true;
	}
	return									false;
}

template <class path_type>
void   common_prefix_path				(path_type *		out_common_path, 
										 path_type const &	first_path, 
										 path_type const &	second_path)
{
	get_common_prefix						(& out_common_path->as_buffer_string(), first_path.c_str(), second_path.c_str());
	
	u32 const common_path_length		=	out_common_path->length();
	if ( (first_path[common_path_length] && first_path[common_path_length] != path_type::separator) ||
		 (second_path[common_path_length] && second_path[common_path_length] != path_type::separator) )
	{
		u32 const last_slash_pos		=	out_common_path->rfind(path_type::separator);
		if ( last_slash_pos == u32(-1) )
			* out_common_path			=	"";
		else
			out_common_path->set_length		(last_slash_pos);
	}

	if ( out_common_path->last() == path_type::separator )
		out_common_path->rtrim				();
}

template <class out_path_type, class in_path_type>
void   convert_to_relative_path			(out_path_type *		out_relative_path, 
										 in_path_type const &	absolute_path,
										 in_path_type const &	root_to_relate)
{
	R_ASSERT								((pvoid)out_relative_path != (pvoid)& absolute_path);

	u32 const parts_in_root				=	root_to_relate.length() ? 
											strings::count_of(root_to_relate.c_str(), in_path_type::separator) + 1 : 0;			

	u32 const parts_in_absolute			=	absolute_path.length() ? 
											strings::count_of(absolute_path.c_str(), in_path_type::separator) + 1 : 0;

	in_path_type							common_path;
	common_prefix_path						(& common_path, absolute_path, root_to_relate);

	u32 const parts_in_common			=	common_path.length() ? 
											strings::count_of(common_path.c_str(), in_path_type::separator) + 1 : 0;

	if ( parts_in_absolute == parts_in_common )
	{
		* out_relative_path				=	"";
		return;
	}

	u32 const up_count					=	parts_in_root - parts_in_common;

	char up[]							=	{ '.', '.', out_path_type::separator, 0 };
	out_relative_path->append_repeat		(up, up_count);
	
	u32 const skip						=	common_path.length() ? common_path.length() + 1 : 0;
	out_relative_path->append_with_conversion	(absolute_path.c_str() + skip);
}

template <class in_out_path_type>
void	convert_to_relative_path_inplace(in_out_path_type *			in_out_path,
										 in_out_path_type const &	root_to_relate)
{
	in_out_path_type						relative_path;
	convert_to_relative_path				(& relative_path, * in_out_path, root_to_relate);
	* in_out_path						=	relative_path;
}

template <class out_path_type>
void   get_path_without_last_item		(out_path_type * out_result, pcstr path)
{
	if ( pcstr const last_slash_pos = strrchr(path, out_path_type::separator) )
		out_result->assign					(path, last_slash_pos);
	else
		out_result->clear					();
}

template <class in_out_path_type>
void   get_path_without_last_item_inplace	(in_out_path_type * in_out_path)
{
	if ( pcstr const last_slash_pos = strrchr(in_out_path->c_str(), in_out_path_type::separator) )
	{
		in_out_path_type					new_path;
		new_path.assign						(in_out_path->c_str(), last_slash_pos);
		* in_out_path					=	new_path;
	}
	else
		in_out_path->clear					();
}

template <class in_out_path_type>
void   cut_last_item_from_path			(in_out_path_type * in_out_result)
{
	u32 const last_slash_pos			=	in_out_result->rfind(in_out_path_type::separator);
	if ( last_slash_pos == u32(-1) )
		in_out_result->set_length			(0);
	else
		in_out_result->set_length			(last_slash_pos);
}

template <class in_path_type>
pcstr   file_name_from_path				(in_path_type const & path)
{
	u32 const last_slash_pos			=	path.rfind(in_path_type::separator);
	if ( last_slash_pos != u32(-1) )
		return								path.c_str() + last_slash_pos + 1;
	else
		return								path.c_str();
}

template <class in_path_type>
void   file_name_with_no_extension_from_path (in_path_type * out_result, pcstr path)
{
	pcstr file_name_start				=	0;
	if ( pcstr const last_slash_pos = strrchr(path, in_path_type::separator) )
		file_name_start					=	last_slash_pos + 1;
	else
		file_name_start					=	path;

	pcstr const last_point_pos			=	strrchr(path, '.');

	if ( last_point_pos && last_point_pos > file_name_start )
		out_result->assign					(file_name_start, last_point_pos);
	else
		* out_result					=	file_name_start;
}

template <class in_out_path_type>
void   set_extension_for_path			(in_out_path_type * in_out_result, pcstr extension)
{
	u32 const point_pos					=	in_out_result->rfind('.');
	u32 const slash_pos					=	in_out_result->rfind(in_out_path_type::separator);
	bool const file_name_without_extension	=	point_pos == u32(-1) || 
												(slash_pos != u32(-1) && point_pos < slash_pos);
	if ( file_name_without_extension )
	{
		CURE_ASSERT							(in_out_result->length(), return);
		in_out_result->appendf				(".%s", extension);
		return;
	}

	in_out_result->set_length				(point_pos + 1);
	* in_out_result						+=	extension;

}

template <class in_path_type>
pcstr   extension_from_path				(in_path_type const & path)
{
	u32 const last_slash_pos			=	path.rfind('.');
	if ( last_slash_pos != u32(-1) )
		return								path.c_str() + last_slash_pos + 1;
	else
		return								path;
}

inline
pcstr   extension_from_path				(pcstr path)
{
	if ( pcstr const last_point_pos = strrchr(path, '.') )
		return								last_point_pos + 1;
	else
		return								path + strings::length(path);
}

inline 
bool   path_starts_with					(pcstr path, pcstr with)
{
	if ( !strings::starts_with(path, with) )
		return								false;

	u32 const with_length				=	strings::length(with);
	u32 const next_symbol_in_path		=	path[with_length];
	return									next_symbol_in_path == '\0' || 
											next_symbol_in_path == native_path_string::separator || 
											next_symbol_in_path == virtual_path_string::separator;
}

} // namespace fs_new
} // namespace xray

#endif // #ifndef XRAY_FS_PATH_STRING_UTILS_INLINE_H_INCLUDED