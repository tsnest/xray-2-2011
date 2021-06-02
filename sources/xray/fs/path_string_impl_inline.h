////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PATH_STRING_IMPL_INLINE_H_INCLUDED
#define FS_PATH_STRING_IMPL_INLINE_H_INCLUDED

namespace xray {
namespace fs_new {

inline
path_string_impl::path_string_impl (char separator) : m_separator(separator) 
{
}

inline
path_string_impl::path_string_impl (char separator, path_string_impl const & src) 
	: m_separator(separator), m_string((string_type &)src) 
{ 
	verify_self								(); 
}
	
template <class T>
path_string_impl::path_string_impl (char separator, T const & src) 
	: m_separator(separator), m_string(src)			
{ 
	verify_self								(); 
}

inline
path_string_impl::path_string_impl (char separator, value_type const * src, size_type const & count)
	: m_separator(separator), m_string(src, count)	
{ 
	verify_self								(); 
}

template <class src_iterator>
path_string_impl::path_string_impl (char separator, src_iterator const & begin, src_iterator const & end)
	: m_separator(separator), m_string(begin, end)	
{ 
	verify_self								(); 
}

inline
path_string_impl const &   path_string_impl::operator = (path_string_impl const & s) 
{ 
	m_string							=	(string_type &)s;
	verify_self								(); 
	return									*this; 
}

template <class src_type>
path_string_impl const &   path_string_impl::operator =	(src_type const & s) 
{ 
	m_string							=	s;
	verify_self								();
	return									* this; 
}

inline
path_string_impl &   path_string_impl::assignf (pcstr const format, ...)
{
	clear									();
	va_list									argptr;
	va_start 								(argptr, format);
	appendf_va_list							(format, argptr);
	va_end	 								(argptr);
	return									* this;
}

inline
path_string_impl &   path_string_impl::appendf (pcstr const format, ...)
{
	va_list									argptr;
	va_start 								(argptr, format);
	appendf_va_list							(format, argptr);
	va_end	 								(argptr);
	return									* this;
}

template <class src_type>
path_string_impl &   path_string_impl::append (src_type const & s)
{
	u32 const current_length			=	m_string.length(); 
	m_string.append							(s); 
	verify									(m_string.c_str() + current_length);
	return									* this;
}

template <class input_iterator>
path_string_impl &   path_string_impl::append (input_iterator const & begin, input_iterator const & end)
{
	u32 const current_length			=	m_string.length(); 
	m_string.append							(begin, end); 
	verify									(m_string.c_str() + current_length);
	return									* this;
}

inline path_string_impl &	path_string_impl::append (path_string_impl const & s) 
{ 
	R_ASSERT_CMP							(m_separator, ==, s.m_separator, "trying to append string of other format: '%s'", s.c_str()); 
	m_string.append							(s.begin(), s.end()); 
	return									* this;
}

template <class src_type>
path_string_impl &   path_string_impl::assign (src_type const & s)
{
	m_string.assign							(s); 
	verify_self								();
	return									* this;
}

template <class input_iterator>
path_string_impl &   path_string_impl::assign (input_iterator const & begin, input_iterator const & end)
{
	m_string.assign							(begin, end);
	verify_self								();
	return									* this;
}

inline 
path_string_impl &   path_string_impl::append_repeat	(pcstr string_to_repeat, u32 count) 
{ 
	verify									(string_to_repeat);
	m_string.append_repeat					(string_to_repeat, count);
	return									* this;
}

inline
path_string_impl &   path_string_impl::appendf_va_list (pcstr const format, va_list argptr)
{
	u32 const current_length			=	m_string.length(); 
	m_string.appendf_va_list				(format, argptr);
	verify									(m_string.c_str() + current_length);
	return									* this;
}

inline
path_string_impl &   path_string_impl::appendf_va_list_with_conversion (pcstr const format, va_list argptr)
{
	m_string.appendf_va_list				(format, argptr);
	convert									(begin(), end());
	return									* this;
}

template <class src_type>
path_string_impl const &   path_string_impl::assign_with_conversion	(src_type const & s)
{
	m_string							=	s;
	convert									(begin(), end());
	return									* this;
}

inline
path_string_impl const &	path_string_impl::assignf_with_conversion	(pcstr const format, ...)
{
	clear									();
	va_list									argptr;
	va_start 								(argptr, format);
	appendf_va_list_with_conversion			(format, argptr);
	va_end	 								(argptr);
	return									* this;
}

inline
path_string_impl const &	path_string_impl::appendf_with_conversion	(pcstr const format, ...)
{
	va_list									argptr;
	va_start 								(argptr, format);
	appendf_va_list_with_conversion			(format, argptr);
	va_end	 								(argptr);
	return									* this;
}

template <class input_iterator>
path_string_impl const &   path_string_impl::assign_with_conversion (input_iterator const begin, input_iterator const end)
{
	m_string.assign							(begin, end);
	convert									(this->begin(), this->end());
	return									* this;
}

template <class src_type>
path_string_impl const &   path_string_impl::append_with_conversion	(src_type const & s)
{
	iterator it_begin					=	end();
	m_string.append							(s);
	convert									(it_begin, this->end());
	return									* this;
}

template <class input_iterator>
path_string_impl const &   path_string_impl::append_with_conversion (input_iterator const begin, input_iterator const end)
{
	iterator it_begin					=	this->end();
	m_string.append							(begin, end);
	convert									(it_begin, this->end());
	return									* this;
}

inline
void   verify_path_string (pcstr string_to_verify, pcstr string_to_assert, char separator)
{
	char wrong_separator1				=	0;
	char wrong_separator2				=	0;
	if ( separator == '/' )
	{
		wrong_separator1				=	'\\';
		wrong_separator2				=	'|';
	}
	else if ( separator == '\\' )
	{
		wrong_separator1				=	'/';
		wrong_separator2				=	'|';
	}
	else if ( separator == '|' )
	{
		wrong_separator1				=	'\\';
		wrong_separator2				=	'/';
	}
	else
		NODEFAULT							( );

	R_ASSERT_U								(!strchr(string_to_verify, wrong_separator1) && !strchr(string_to_verify, wrong_separator2),
											"wrong path separator found in '%s'", string_to_assert);
}

template <class src_type> inline
path_string_impl &	 path_string_impl::append_path (src_type const & s)
{
	if ( length() )
		append								(m_separator);

	append									(s);
	rtrim									(m_separator);
	return									* this;
}

inline bool	operator <					(path_string_impl const & left, path_string_impl const & right)
{
	return									left.as_buffer_string() < right.as_buffer_string();
}

inline bool	operator >					(path_string_impl const & left, path_string_impl const & right)
{
	return									left.as_buffer_string() > right.as_buffer_string();
}

} // namespace fs_new
} // namespace xray

#endif // #ifndef FS_PATH_STRING_IMPL_INLINE_H_INCLUDED