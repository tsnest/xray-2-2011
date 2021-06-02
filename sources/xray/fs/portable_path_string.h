////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PORTABLE_PATH_STRING_H_INCLUDED
#define FS_PORTABLE_PATH_STRING_H_INCLUDED

#include "path_string_impl.h"

namespace xray {
namespace fs_new {

class native_path_string;

class portable_path_string : public path_string_impl 
{
public:
	typedef	path_string_impl	super;
	static const char separator = '/';

public:
	portable_path_string() : path_string_impl(separator) {}

	portable_path_string	(portable_path_string const & other) : path_string_impl(separator, (path_string_impl &)other) {}
	portable_path_string	(native_path_string const & other);

	template <class T>
	portable_path_string	(T const & other) : path_string_impl(separator, other) {}

	template <class src_iterator>
	portable_path_string	(src_iterator const & begin, src_iterator const & end) 
		: path_string_impl(separator, begin, end) {}

	static portable_path_string 	convert (pcstr path) 
		{ portable_path_string out_result; out_result.assign_with_conversion(path); return out_result; }

	portable_path_string const &	operator =	(portable_path_string const & s)
		{ super::operator = ((path_string_impl &)s); return *this; }		

	template <class src_type>
	portable_path_string const &	operator =	(src_type const & s)
		{ super::operator = (s); return *this; }		

	template <class src_type>
	portable_path_string const &	operator +=	(src_type const & s) { super::operator += (s); return * this; }
	portable_path_string const &	operator +=	(portable_path_string const & s) { return append(s); }


	template <class src_type>
	portable_path_string const &	append_path	(src_type const & s) { super::append_path(s); return * this; }
	portable_path_string const &	append_path	(portable_path_string const & s) { super::append_path((string_type &)s); return * this; }

	template <class src_type>
	portable_path_string const &	append		(src_type const & s) { super::append(s); return * this; }
	portable_path_string const &	append		(portable_path_string const & s) { super::append((string_type &)s); return * this; }
	template <class input_iterator>
	portable_path_string const &	append		(input_iterator const & begin, input_iterator const & end) { super::append(begin, end); return * this; }

	static void						verify		(pcstr string) { verify_path_string	(string, string, separator); }
}; // class portable_path_string

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_PORTABLE_PATH_STRING_H_INCLUDED