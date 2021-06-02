////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_NATIVE_PATH_STRING_H_INCLUDED
#define FS_NATIVE_PATH_STRING_H_INCLUDED

#include <xray/fs/path_string_impl.h>

namespace xray {
namespace fs_new {

class portable_path_string;

class XRAY_FS_API native_path_string : public path_string_impl 
{
public:
	typedef	path_string_impl	super;
	static const char separator = '\\';

public:
	native_path_string() : path_string_impl(separator) {}

	native_path_string	(native_path_string const & other) : path_string_impl(separator, (path_string_impl &)other) {}
	native_path_string	(portable_path_string const & other);

	template <class T>
	native_path_string	(T const & other) : path_string_impl(separator, other) {}

	template <class src_iterator>
	native_path_string	(src_iterator const & begin, src_iterator const & end) 
		: path_string_impl(separator, begin, end) {}

	static native_path_string 	convert (pcstr path) 
		{ native_path_string out_result; out_result.assign_with_conversion(path); return out_result; }

	native_path_string const &	operator =	(native_path_string const & s)
		{ super::operator = ((path_string_impl &)s); return *this; }		

	template <class src_type>
	native_path_string const &	operator =	(src_type const & s)
		{ super::operator = (s); return *this; }		

	template <class src_type>
	native_path_string const &	operator +=	(src_type const & s) { super::operator += (s); return * this; }
	native_path_string const &	operator +=	(native_path_string const & s) { return append(s); }

	template <class src_type>
	native_path_string const &	append_path	(src_type const & s) { super::append_path(s); return * this; }
	native_path_string const &	append_path	(native_path_string const & s) { super::append_path((string_type &)s); return * this; }

	template <class src_type>
	native_path_string const &	append		(src_type const & s) { super::append(s); return * this; }
	native_path_string const &	append		(native_path_string const & s) { super::append((string_type &)s); return * this; }
	template <class input_iterator>
	native_path_string const &	append		(input_iterator const & begin, input_iterator const & end) { super::append(begin, end); return * this; }

	static void					verify		(pcstr string) { verify_path_string	(string, string, separator); }
}; // class native_path_string

void	convert_to_native (native_path_string * out_path, pcstr path);

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_NATIVE_PATH_STRING_H_INCLUDED