////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_VIRTUAL_PATH_STRING_H_INCLUDED
#define FS_VIRTUAL_PATH_STRING_H_INCLUDED

#include "path_string_impl.h"

namespace xray {
namespace fs_new {

class XRAY_FS_API virtual_path_string : public path_string_impl 
{
public:
	typedef	path_string_impl	super;
	static const char separator = '/';

public:
	virtual_path_string() : path_string_impl(separator) {}

	virtual_path_string	(virtual_path_string const & other) : path_string_impl(separator, (path_string_impl &)other) {}

	template <class T>
	virtual_path_string	(T const & other) : path_string_impl(separator, other) {}

	template <class src_iterator>
	virtual_path_string	(src_iterator const & begin, src_iterator const & end) 
		: path_string_impl(separator, begin, end) {}

	static virtual_path_string 	convert (pcstr path) 
		{ virtual_path_string out_result; out_result.assign_with_conversion(path); return out_result; }

	virtual_path_string const &	operator =	(virtual_path_string const & s)
		{ super::operator = ((path_string_impl &)s); return *this; }		

	template <class src_type>
	virtual_path_string const &	operator =	(src_type const & s)
		{ super::operator = (s); return *this; }		

	template <class src_type>
	virtual_path_string const &	operator +=	(src_type const & s) { super::operator += (s); return * this; }
	virtual_path_string const &	operator +=	(virtual_path_string const & s) { return append(s); }

	template <class src_type>
	virtual_path_string const &	append_path	(src_type const & s) { super::append_path(s); return * this; }
	virtual_path_string const &	append_path	(virtual_path_string const & s) { super::append_path((string_type &)s); return * this; }

	template <class src_type>
	virtual_path_string const &	append		(src_type const & s) { super::append(s); return * this; }
	virtual_path_string const &	append		(virtual_path_string const & s) { super::append((string_type &)s); return * this; }
	template <class input_iterator>
	virtual_path_string const &	append		(input_iterator const & begin, input_iterator const & end) { super::append(begin, end); return * this; }

	static void					verify		(pcstr string) { verify_path_string	(string, string, separator); }
}; // class virtual_path_string

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_VIRTUAL_PATH_STRING_H_INCLUDED