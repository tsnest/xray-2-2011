#ifndef BUFFER_STRING_INLINE_H_INCLUDED
#define BUFFER_STRING_INLINE_H_INCLUDED

namespace xray {
namespace core {

//-----------------------------------------------------------------------------------
// initializations
//-----------------------------------------------------------------------------------

inline
buffer_string::buffer_string (value_type* buffer, size_type const& max_count) 
							: m_begin(buffer), m_end(buffer), m_max_end(buffer+max_count) 
{
}
	
inline
buffer_string::buffer_string (value_type* buffer, size_type const& max_count, size_type const& count)
							: m_begin(buffer), m_end(buffer+count), m_max_end(buffer+max_count) 
{
}

inline
buffer_string::buffer_string (value_type* buffer, size_type const& max_count, const value_type* src)
							: m_begin(buffer), m_end(buffer), m_max_end(buffer+max_count) 
{
	*this += src;
}

template <typename input_iterator>
buffer_string::buffer_string (value_type*			buffer, 
							  size_type const&		max_count, 
							  input_iterator const&	begin_src, 
							  input_iterator const&	end_src)
							: m_begin(buffer), m_max_end(buffer+max_count),	m_end(m_begin)
{
	append(begin_src, end_src);
}

template <typename input_iterator>
void buffer_string::append (input_iterator const& begin_src, input_iterator const& end_src)
{
	VERIFY(m_end + (end_src-begin_src) < m_max_end);

	for ( input_iterator	it		=	begin_src;
							it		!=	end_src;
							++it, ++m_end)
	{
		*m_end	=	*it;
	}

	*m_end	=	NULL;
}

template <typename input_iterator>
void buffer_string::erase (input_iterator const& begin_src, input_iterator const& end_src)
{
	VERIFY(m_begin<=begin_src && m_end>=end_src);
	m_end	=	std::copy(end_src, m_end, begin_src);
	*m_end	=	NULL;
}

//-----------------------------------------------------------------------------------
// operations
//-----------------------------------------------------------------------------------

inline
void buffer_string::operator += (const value_type* s)
{
	if ( !s )
	{
		return;
	}

	VERIFY(m_end + strings::length(s) < m_max_end);

	while ( *s && m_end < m_max_end )
	{
		*m_end = *s;
		++m_end;
		++s;
	}

	*m_end = NULL;
}

inline
void buffer_string::operator += (value_type  c)
{
	char buffer[] = {c, NULL};
	*this += buffer;
}

inline
void buffer_string::operator += (const buffer_string& s)
{
	append(s.begin(), s.end());
}

inline
void buffer_string::set (size_type pos, value_type c)
{
	pointer	ptr	=	m_begin + pos;
	VERIFY(ptr	<=	m_end, "trying to set character after terminating NULL");
	*ptr	=	c;
	if ( !c )
	{
		m_end	= ptr;
	}
}

inline
void buffer_string::set_size (size_type size)
{
	VERIFY(m_begin + size < m_max_end);
	m_end	=	m_begin	+	size;
	*m_end	=	NULL;
}

inline
buffer_string::size_type buffer_string::find	(const value_type c) const
{
	char* res = strchr(m_begin, c);
	return res ? res-m_begin : -1;
}

inline
buffer_string::size_type buffer_string::find	(const value_type c, size_type offs) const
{
	char* res = strchr(m_begin+offs, c);
	return res ? res-m_begin : -1;
}

inline
buffer_string::size_type buffer_string::find	(const value_type* s) const
{
	char* res = strstr(m_begin, s);
	return res ? res-m_begin : -1;
}

inline
buffer_string::size_type buffer_string::find	(const value_type* s, size_type offs) const
{
	char* res = strstr(m_begin+offs, s);
	return res ? res-m_begin : -1;
}

inline
void buffer_string::rtrim (value_type c)
{
	while ( m_end > m_begin && *(m_end-1) == c )
	{
		--m_end;
	}

	*m_end	=	NULL;
}

inline
void buffer_string::clear ()
{
	VERIFY		(m_begin);
	m_end	=	m_begin;
	*m_end	=	NULL;
}

inline
void buffer_string::substr (const size_t pos, size_t count, buffer_string& dest) const
{
	dest.clear();

	if ( count == npos )
	{
		count	=	size() - pos;
	}
	else
	{
		size_t	src_max_count	=	size() > pos ? size() - pos : 0;
		if ( count > src_max_count )
		{
			count	=	src_max_count;
		}

		size_t	dest_max_count	=	dest.max_size();
		if ( count > dest_max_count )
		{
			count	=	dest_max_count;
		}
	}

	dest.append(m_begin+pos, m_begin+pos+count);
}

//-----------------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------------

inline
buffer_string::value_type&	 buffer_string::operator [] (size_type i)
{
	VERIFY(m_begin);
	VERIFY(m_begin + i < m_max_end);
	return *(m_begin + i);
}

inline
buffer_string::value_type	 buffer_string::operator [] (size_type i) const
{
	VERIFY(m_begin);
	VERIFY(m_begin + i < m_max_end);
	return *(m_begin + i);
}

inline
u32   buffer_string::size	() const
{
	return (u32)(m_end - m_begin); 
}

inline
u32   buffer_string::max_size	() const
{
	return (u32)(m_max_end - m_begin); 
}

inline
bool   buffer_string::empty	() const
{
	return m_begin == m_end; 
}

inline
buffer_string::value_type*	  buffer_string::c_str ()			
{
	return m_begin; 
}

inline
const buffer_string::value_type*	  buffer_string::c_str () const
{
	return m_begin; 
}

//-----------------------------------------------------------------------------------
// iterators
//-----------------------------------------------------------------------------------

inline
buffer_string::iterator   buffer_string::begin	()			
{ 
	return m_begin; 
}

inline
buffer_string::iterator   buffer_string::end ()			
{ 
	return m_end; 
}

inline
buffer_string::const_iterator   buffer_string::begin () const	
{ 
	return m_begin; 
}

inline
buffer_string::const_iterator   buffer_string::end	() const	
{
	return m_end; 
}

inline
buffer_string::reverse_iterator   buffer_string::rbegin ()
{
	return reverse_iterator(end()); 
}

inline
buffer_string::reverse_iterator   buffer_string::rend ()			
{
	return reverse_iterator(begin()); 
}

inline
buffer_string::const_reverse_iterator	 buffer_string::rbegin () const	
{
	return const_reverse_iterator(end()); 
}

inline
buffer_string::const_reverse_iterator	 buffer_string::rend ()	const	
{
	return const_reverse_iterator(begin()); 
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
	m_begin		=	buffer;
	m_end		=	buffer;
	m_max_end	=	buffer	+ max_count;
}

inline
void   buffer_string::construct	(value_type* buffer, size_type const& max_count, size_type const& count)
{
	m_begin		=	buffer;
	m_end		=	buffer	+ count;
	m_max_end	=	buffer	+ max_count;
}

//-----------------------------------------------------------------------------------
// buffer_string outer operators
//-----------------------------------------------------------------------------------

namespace detail
{

inline
int	  xr_strcmp_s (const char* s1, const char* s2)
{
	if ( !s1 && !s2 )
	{
		return 0;
	}
	else if ( !s1 )
	{
		return (!*s2) ? 0 : -1;
	}
	else if ( !s2 )
	{
		return (!*s1) ? 0 : +1;
	}

	return strings::compare(s1, s2);
}

} // namespace detail

inline
bool   operator == (const buffer_string& s1, const buffer_string& s2)
{
	return !detail::xr_strcmp_s(s1.c_str(), s2.c_str());
}

inline
bool   operator == (const buffer_string::value_type* s1, const buffer_string& s2)
{
	return !detail::xr_strcmp_s(s1, s2.c_str());
}

inline
bool   operator == (const buffer_string& s1, const buffer_string::value_type* s2)
{
	return !detail::xr_strcmp_s(s1.c_str(), s2);
}

inline
bool   operator < (const buffer_string& s1, const buffer_string& s2)
{
	return detail::xr_strcmp_s(s1.c_str(), s2.c_str()) == -1;
}

inline
bool   operator < (const buffer_string::value_type* s1, const buffer_string& s2)
{
	return detail::xr_strcmp_s(s1, s2.c_str()) == -1;
}

inline
bool   operator < (const buffer_string& s1, const buffer_string::value_type* s2)
{
	return detail::xr_strcmp_s(s1.c_str(), s2) == -1;
}

inline
bool   operator > (const buffer_string& s1, const buffer_string& s2)
{
	return detail::xr_strcmp_s(s1.c_str(), s2.c_str()) == +1;
}

inline
bool   operator > (const buffer_string::value_type* s1, const buffer_string&	s2)
{
	return detail::xr_strcmp_s(s1, s2.c_str()) == +1;
}

inline
bool   operator > (const buffer_string& s1, const buffer_string::value_type* s2)
{
	return detail::xr_strcmp_s(s1.c_str(), s2) == +1;
}

} // namespace core
} // namespace xray

#endif // BUFFER_STRING_INLINE_H_INCLUDED