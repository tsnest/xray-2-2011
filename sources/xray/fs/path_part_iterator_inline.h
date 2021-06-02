////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_PATH_PART_ITERATOR_INLINE_H_INCLUDED
#define XRAY_FS_PATH_PART_ITERATOR_INLINE_H_INCLUDED

namespace xray {
namespace fs_new {

//-----------------------------------------------------------------------------------
// path_part_iterator
//-----------------------------------------------------------------------------------

inline
path_part_iterator::path_part_iterator(pcstr path_str, char separator, include_empty_string_in_iteration_bool include_empty_string_in_iteration) 
	: m_separator(separator), m_include_empty_string_in_iteration(include_empty_string_in_iteration)
{
	m_path_str							=	path_str;
	m_path_end							=	path_str + (path_str ? strings::length(path_str) : 0);
	m_cur_str							=	path_str;
	m_cur_end							=	path_str;
	++ * this;
}

inline
path_part_iterator::path_part_iterator(pcstr path_str, int const len, char separator, include_empty_string_in_iteration_bool include_empty_string_in_iteration) 
	: m_separator(separator), m_include_empty_string_in_iteration(include_empty_string_in_iteration)
{
	m_path_str							=	path_str;
	m_path_end							=	path_str + len;
	m_cur_str							=	path_str;
	m_cur_end							=	path_str;
	++ * this;
}

template <class out_string_type>
void   path_part_iterator::append_to_string (out_string_type & out_string) const
{
	ASSERT									(m_cur_str);
	if ( m_cur_str == m_cur_end )
		return;

	out_string.append						(m_cur_str + ((*m_cur_str == m_separator) ? 1 : 0), m_cur_end);
}

inline
bool   path_part_iterator::equal		(pcstr s) const
{
	if ( !m_cur_str )
		return								!*s;
	
	pcstr cur_str	=	m_cur_str + ((*m_cur_str == m_separator) ? 1 : 0);
	while ( *s && *cur_str )
	{
		if ( *s != *cur_str )
			break;

		++s;
		++cur_str;
	}

	return									!*s && (!*cur_str || *cur_str==m_separator);
}

inline
void   path_part_iterator::operator ++	()
{
	if ( !m_cur_str ) 	
		return;
	
	if ( m_include_empty_string_in_iteration == include_empty_string_in_iteration_true )
	{
		m_include_empty_string_in_iteration	=	include_empty_string_in_iteration_false;
		return;
	}

	if ( m_cur_end == m_path_end )
	{
		m_cur_str						=	NULL;
		return;
	}

	if ( m_cur_end != m_path_str )
	{
		m_cur_str						=	m_cur_end + 1;
		m_cur_end						=	m_cur_str;
	}

	while ( m_cur_end != m_path_end )
	{
		if ( *m_cur_end == m_separator )
			break;

		++m_cur_end;
	}
}

inline
bool   path_part_iterator::operator ==	(path_part_iterator const & it) const
{
	if ( m_include_empty_string_in_iteration != it.m_include_empty_string_in_iteration )
		return								false;

	return									m_cur_str == it.m_cur_str;
}

inline
bool   path_part_iterator::operator != (path_part_iterator const & it) const
{
	return									!(*this == it);
}

//-----------------------------------------------------------------------------------
// reverse_path_part_iterator
//-----------------------------------------------------------------------------------

inline
reverse_path_part_iterator::reverse_path_part_iterator (pcstr path_str, char separator) 
	: path_part_iterator(separator)
{
	m_path_str							=	path_str;
	m_path_end							=	path_str + (path_str ? strings::length(path_str) : 0);
	m_cur_str							=	m_path_end;
	m_cur_end							=	m_path_end;
	++ * this;
}

inline
reverse_path_part_iterator::reverse_path_part_iterator (pcstr path_str, int const len, char separator) 
	: path_part_iterator(separator)
{
	m_path_str							=	path_str;
	m_path_end							=	path_str + len;
	m_cur_str							=	m_path_end;
	m_cur_end							=	m_path_end;
	++ * this;
}

inline
void   reverse_path_part_iterator::operator ++ ()
{
	if ( !m_cur_str ) 	
		return;
	
	if ( m_cur_str == m_path_str )
	{
		m_cur_str						=	NULL;
		return;
	}

	m_cur_end							=	m_cur_str;
	m_cur_str							=	m_cur_end - 1;

	while ( m_cur_str != m_path_str )
	{
		if ( * m_cur_str == m_separator )
			break;

		--m_cur_str;
	}
}

inline
bool   reverse_path_part_iterator::operator == (const reverse_path_part_iterator& it) const
{
	return									m_cur_str == it.m_cur_str;
}

inline
bool   reverse_path_part_iterator::operator != (const reverse_path_part_iterator& it) const
{
	return									 !(* this == it);
}

} // namespace fs_new
} // namespace xray

#endif // #ifndef XRAY_FS_PATH_PART_ITERATOR_INLINE_H_INCLUDED