////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BUFFER_STRING_INLINE_H_INCLUDED
#define BUFFER_STRING_INLINE_H_INCLUDED

namespace xray {

//-----------------------------------------------------------------------------------
// initializations
//-----------------------------------------------------------------------------------

inline
buffer_string::buffer_string (value_type* buffer, size_type const& max_count) 
							: m_begin(buffer), m_end(buffer), m_max_end(buffer+max_count) 
{
	ASSERT						(max_count);
	*m_end					=	NULL;
}
	
inline
buffer_string::buffer_string (value_type* buffer, size_type const& max_count, size_type const& count)
							: m_begin(buffer), m_end(buffer+count), m_max_end(buffer+max_count) 
{
	ASSERT						(max_count);
	*m_end					=	NULL;
}

inline
buffer_string::buffer_string (value_type* buffer, size_type const& max_count, value_type const* src)
							: m_begin(buffer), m_end(buffer), m_max_end(buffer+max_count) 
{
	ASSERT						(src < buffer || src >= buffer + max_count);
	ASSERT						(max_count);
	*m_end					=	NULL;
	*this					+=	src;
}

template <typename input_iterator>
buffer_string::buffer_string (value_type*			buffer, 
							  size_type const&		max_count, 
							  input_iterator const&	begin_src, 
							  input_iterator const&	end_src)
							: m_begin(buffer),	m_end(m_begin), m_max_end(buffer+max_count)
{
	ASSERT						(&*begin_src < buffer || &*begin_src >= buffer + max_count);
	ASSERT						(&*end_src <= buffer  || &*end_src >= buffer + max_count);
	append						(begin_src, end_src);
}

template <typename input_iterator>
buffer_string &   buffer_string::append (input_iterator const& begin_src, input_iterator const& end_src)
{
	R_ASSERT_CMP			(begin_src, <=, end_src);
	ASSERT					(m_end + (end_src-begin_src) < m_max_end);

	for ( input_iterator	it		=	begin_src;
							it		!=	end_src;
							++it, ++m_end)
	{
		*m_end				=	*it;
	}

	*m_end					=	NULL;
	return						* this;
}

template <typename input_iterator>
buffer_string &   buffer_string::assign (input_iterator const& begin_src, input_iterator const& end_src)
{
	clear						();
	return						append(begin_src, end_src);
}

inline
buffer_string &   buffer_string::append_repeat (pcstr const string_to_repeat, u32 const count)
{
	u32 const string_to_repeat_length	=	strings::length(string_to_repeat);
	for ( u32 i=0; i<count; ++i )
		append					(string_to_repeat, string_to_repeat + string_to_repeat_length);
	return						* this;
}

inline
buffer_string &   buffer_string::append (value_type const c)
{
	ASSERT						(m_end < m_max_end);
	* m_end++				=	c;
	* m_end					=	NULL;
	return						* this;
}

inline
buffer_string &   buffer_string::append (pcstr const begin_src, pcstr const end_src)
{
	R_ASSERT					(m_end + (end_src-begin_src) < m_max_end);

	u32 const range			=	(u32)(end_src - begin_src);
	memory::copy				(m_end, u32(m_max_end - m_end), begin_src, range);
	m_end					+=	range; 
	* m_end					=	NULL;
	return						* this;
}

inline
buffer_string &   buffer_string::append (pcstr const c_string)
{
	R_ASSERT					(m_end + strings::length(c_string) < m_max_end);

	u32 const range			=	strings::length(c_string);
	memory::copy				(m_end, u32(m_max_end - m_end), c_string, range);
	m_end					+=	range; 
	* m_end					=	NULL;
	return						* this;
}

template <typename input_iterator>
void   buffer_string::erase (input_iterator const& begin_src, input_iterator const& end_src)
{
	ASSERT						(m_begin<=begin_src && m_end>=end_src);
	m_end					=	std::copy(&*end_src, m_end, &*begin_src);
	*m_end					=	NULL;
}

//-----------------------------------------------------------------------------------
// operations
//-----------------------------------------------------------------------------------

inline
buffer_string const&   buffer_string::operator = (value_type const* s)
{
	clear						();
	return						*this += s;
}

inline
buffer_string const&   buffer_string::operator = (value_type  c)
{
	clear						();
	return						*this += c;
}

inline
buffer_string const&   buffer_string::operator = (const buffer_string& s)
{
	if ( this == &s )
	{
		return					*this;
	}
	clear						();
	return						*this += s;
}

inline
buffer_string const&   buffer_string::operator += (value_type  c)
{
	append						(c);
	return						*this;
}

inline
buffer_string const&   buffer_string::operator += (const buffer_string& s)
{
	append						(s.begin(), s.end());
	return						*this;
}

inline
void   buffer_string::set (size_type pos, value_type c)
{
	pointer	ptr				=	m_begin + pos;
	ASSERT						(ptr	<	m_end, "trying to set character after terminating NULL");
	*ptr					=	c;
	if ( !c )
	{
		m_end				=	ptr;
	}
}

inline
void   buffer_string::set_length (size_type const length)
{
	ASSERT						(m_begin + length < m_max_end);
	m_end					=	m_begin	+ length;
	*m_end					=	NULL;
	ASSERT						( m_end >= m_begin);
}

inline
buffer_string::size_type   buffer_string::find	(value_type const c) const
{
	return						find(c, 0);
}

inline
buffer_string::size_type   buffer_string::find	(value_type const c, size_type offs) const
{
	char* res				=	strchr(m_begin+offs, c);
	return						res ? (size_type)(res - m_begin) : npos;
}

inline
buffer_string::size_type   buffer_string::rfind	(value_type const c) const
{
	return						rfind(c, 0);
}

inline
buffer_string::size_type   buffer_string::rfind	(value_type const c, size_type offs) const
{
	pstr current			=	m_end - 1 - offs;

	if ( current < m_begin )
		return					npos;

	for ( ; ; --current )
	{
		if ( * current == c )
			return				(size_type)(current - m_begin);

		if ( current == m_begin )
			break;
	}

	return						npos;
}

inline
buffer_string::size_type   buffer_string::find	(value_type const * s) const
{
	return						find(s, 0);
}

inline
buffer_string::size_type   buffer_string::find	(value_type const * s, size_type offs) const
{
	pstr res				=	strstr(m_begin + offs, s);
	return						res ? (size_type)(res - m_begin) : size_type(-1);
}

inline
void   buffer_string::rtrim (value_type c)
{
	while ( m_end > m_begin && *(m_end-1) == c )
		--m_end;

	*m_end					=	NULL;
}

inline
void   buffer_string::rtrim ()
{
	R_ASSERT					(m_begin != m_end);
	--m_end;
	*m_end					=	NULL;
}

inline 
void   buffer_string::rtrim		(pcstr string)
{
	u32 const string_length	=	strings::length(string);
	if ( !string_length )
		return;

	while ( ends_with(string) )
		set_length				(length() - string_length);
}

inline
void   buffer_string::clear ()
{
	ASSERT						(m_begin);
	m_end					=	m_begin;
	*m_end					=	NULL;
}

inline
u32	  buffer_string::count_of (value_type const char_to_count) const
{
	ASSERT						(m_begin);
	return						strings::count_of(m_begin, char_to_count);
}

//-----------------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------------

inline
buffer_string::value_type&	 buffer_string::operator [] (size_type i)
{
	ASSERT						(m_begin);
	ASSERT						(m_begin + i < m_max_end);
	return						 *(m_begin + i);
}

inline
buffer_string::value_type   buffer_string::operator [] (size_type i) const
{
	ASSERT						(m_begin);
	ASSERT						(m_begin + i < m_max_end);
	return						 *(m_begin + i);
}

inline
u32   buffer_string::length	() const
{
	return						(u32)(m_end - m_begin); 
}

inline
u32   buffer_string::max_length	() const
{
	return						(u32)(m_max_end - m_begin); 
}

inline
bool   buffer_string::empty	() const
{
	return						m_begin == m_end; 
}

inline
buffer_string::value_type*	  buffer_string::c_str ()			
{
	return						m_begin; 
}

inline
const buffer_string::value_type*	  buffer_string::c_str () const
{
	return						m_begin; 
}

//-----------------------------------------------------------------------------------
// iterators
//-----------------------------------------------------------------------------------

inline
buffer_string::iterator   buffer_string::begin	()			
{ 
	return						m_begin; 
}

inline
buffer_string::iterator   buffer_string::end ()			
{ 
	return						m_end; 
}

inline
buffer_string::const_iterator   buffer_string::begin () const	
{ 
	return						m_begin; 
}

inline
buffer_string::const_iterator   buffer_string::end	() const	
{
	return						m_end; 
}

inline
char   buffer_string::last () const
{
	if ( m_end == m_begin )
		return					0;
	//CURE_ASSERT				(m_end != m_begin, return 0, "trying to get last symbol of empty string");
	return						* (m_end - 1);
}

inline
buffer_string::reverse_iterator   buffer_string::rbegin ()
{
	return						reverse_iterator(end()); 
}

inline
buffer_string::reverse_iterator   buffer_string::rend ()			
{
	return						reverse_iterator(begin()); 
}

inline
buffer_string::const_reverse_iterator	 buffer_string::rbegin () const	
{
	return						const_reverse_iterator(end()); 
}

inline
buffer_string::const_reverse_iterator	 buffer_string::rend ()	const	
{
	return						const_reverse_iterator(begin()); 
}

//-----------------------------------------------------------------------------------
// private functions for friends
//-----------------------------------------------------------------------------------

inline
buffer_string::buffer_string () : m_begin(NULL), m_end(NULL), m_max_end(NULL)
{
}

inline
void   buffer_string::construct	(value_type* buffer, size_type const& max_count)
{
	m_begin					=	buffer;
	m_end					=	buffer;
	m_max_end				=	buffer	+ max_count;
}

inline
void   buffer_string::construct	(value_type* buffer, size_type const& max_count, size_type const& count)
{
	m_begin					=	buffer;
	m_end					=	buffer	+ count;
	m_max_end				=	buffer	+ max_count;
}

inline
bool   buffer_string::ends_with			(pcstr string) const 
{ 
	if ( !length() )
		return					strings::length(string) == 0;

	return						strings::ends_with(m_begin, length(), string, strings::length(string)); 
}

inline
bool   buffer_string::starts_with		(pcstr string) const
{ 
	if ( !length() )
		return					strings::length(string) == 0;

	return						strings::starts_with(m_begin, string); 
}

//-----------------------------------------------------------------------------------
// buffer_string outer operators
//-----------------------------------------------------------------------------------

namespace detail
{
	inline int					strcmp_s (const char * s1, const char * s2);

} // namespace detail

inline
bool   operator == (const buffer_string & s1, const buffer_string & s2)
{
	return						s1.length() == s2.length() && 
								!memory::compare(const_buffer((pbyte)s1.c_str(), s1.length()),
												 const_buffer((pbyte)s2.c_str(), s2.length()));
}

inline
bool   operator == (const buffer_string::value_type * s1, const buffer_string & s2)
{
	return						!detail::strcmp_s(s1, s2.c_str());
}

inline
bool   operator == (const buffer_string& s1, const buffer_string::value_type* s2)
{
	return						!detail::strcmp_s(s1.c_str(), s2);
}

inline
bool   operator != (const buffer_string& s1, const buffer_string& s2)
{
	return						!(s1 == s2);
}

inline
bool   operator != (const buffer_string::value_type* s1, const buffer_string& s2)
{
	return						!(s1 == s2);
}

inline
bool   operator != (const buffer_string& s1, const buffer_string::value_type* s2)
{
	return						!(s1 == s2);
}

inline
bool   operator < (const buffer_string& s1, const buffer_string& s2)
{
	return						detail::strcmp_s(s1.c_str(), s2.c_str()) == -1;
}

inline
bool   operator < (const buffer_string::value_type* s1, const buffer_string& s2)
{
	return						detail::strcmp_s(s1, s2.c_str()) == -1;
}

inline
bool   operator < (const buffer_string& s1, const buffer_string::value_type* s2)
{
	return						detail::strcmp_s(s1.c_str(), s2) == -1;
}

inline
bool   operator > (const buffer_string& s1, const buffer_string& s2)
{
	return						detail::strcmp_s(s1.c_str(), s2.c_str()) == +1;
}

inline
bool   operator > (const buffer_string::value_type* s1, const buffer_string&	s2)
{
	return						detail::strcmp_s(s1, s2.c_str()) == +1;
}

inline
bool   operator > (const buffer_string& s1, const buffer_string::value_type* s2)
{
	return						detail::strcmp_s(s1.c_str(), s2) == +1;
}

//-----------------------------------------------------------------------------------
// buffer_string outer operators
//-----------------------------------------------------------------------------------

namespace detail
{

inline
int	  strcmp_s (const char * s1, const char * s2)
{
	if ( !s1 && !s2 )
		return 0;
	else if ( !s1 )
		return (!*s2) ? 0 : -1;
	else if ( !s2 )
		return (!*s1) ? 0 : +1;

	return xray::strings::compare(s1, s2);
}

} // namespace detail

} // namespace xray

#endif // BUFFER_STRING_INLINE_H_INCLUDED