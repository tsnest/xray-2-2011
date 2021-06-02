////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PATH_ITERATOR_INLINE_H_INCLUDED
#define PATH_ITERATOR_INLINE_H_INCLUDED

namespace xray {
namespace fs {

//-----------------------------------------------------------------------------------
// path_iterator
//-----------------------------------------------------------------------------------

inline
path_iterator::path_iterator(const char* path_str)
{
	m_path_str	=	path_str;
	m_path_end	=	path_str + (path_str ? strings::length(path_str) : 0);
	m_cur_str	=	path_str ? path_str - 1 : NULL;
	m_cur_end	=	path_str ? path_str - 1 : NULL;
	++*this;
}

inline
path_iterator::path_iterator(const char* path_str, const int len)
{
	m_path_str	=	path_str;
	m_path_end	=	path_str + len;
	m_cur_str	=	path_str ? path_str - 1 : NULL;
	m_cur_end	=	path_str ? path_str - 1 : NULL;
	++*this;
}

inline
void   path_iterator::to_string (buffer_string& out_str) const
{
	ASSERT(m_cur_str);
	
	if ( m_cur_str == m_cur_end )
	{
		return;
	}
	out_str.assign(m_cur_str + ((*m_cur_str == '/') ? 1 : 0), m_cur_end);
}

inline
bool   path_iterator::equal (const char* s) const
{
	ASSERT(m_cur_str);
	const char* cur_str = m_cur_str + ((*m_cur_str == '/') ? 1 : 0);
	while ( *s && *cur_str )
	{
		if ( *s != *cur_str )
		{
			break;
		}
		++s;
		++cur_str;
	}

	return !*s && (!*cur_str || *cur_str=='/');
}

inline
void   path_iterator::operator ++ ()
{
	if ( !m_cur_str ) 	return;
	
	if ( m_cur_end	==	m_path_end )
	{
		m_cur_str	=	NULL;
		return;
	}

	m_cur_str		=	m_cur_end + 1;
	m_cur_end		=	m_cur_str;

	while ( m_cur_end != m_path_end )
	{
		if ( *m_cur_end == '/' )
		{
			break;
		}
		++m_cur_end;
	}
}

inline
bool   path_iterator::operator == (const path_iterator& it) const
{
	return m_cur_str == it.m_cur_str;
}

inline
bool   path_iterator::operator != (const path_iterator& it) const
{
	return !(*this == it);
}

//-----------------------------------------------------------------------------------
// reverse_path_iterator
//-----------------------------------------------------------------------------------

inline
reverse_path_iterator::reverse_path_iterator(const char* path_str)
{
	m_path_str	=	path_str;
	m_path_end	=	path_str + (path_str ? strings::length(path_str) : 0);
	m_cur_str	=	m_path_end;
	m_cur_end	=	m_path_end;
	++*this;
}

inline
reverse_path_iterator::reverse_path_iterator(const char* path_str, const int len)
{
	m_path_str	=	path_str;
	m_path_end	=	path_str + len;
	m_cur_str	=	m_path_end;
	m_cur_end	=	m_path_end;
	++*this;
}

inline
void   reverse_path_iterator::operator ++ ()
{
	if ( !m_cur_str ) 	return;
	
	if ( m_cur_str	==	m_path_str )
	{
		m_cur_str	=	NULL;
		return;
	}

	m_cur_end		=	m_cur_str;
	m_cur_str		=	m_cur_end-1;

	while ( m_cur_str != m_path_str )
	{
		if ( *m_cur_str == '/' )
		{
			break;
		}
		--m_cur_str;
	}
}

inline
bool   reverse_path_iterator::operator == (const reverse_path_iterator& it) const
{
	return m_cur_str == it.m_cur_str;
}

inline
bool   reverse_path_iterator::operator != (const reverse_path_iterator& it) const
{
	return !(*this == it);
}

} // namespace fs
} // namespace xray

#endif // PATH_ITERATOR_INLINE_H_INCLUDED