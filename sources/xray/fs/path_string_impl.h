////////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef FS_PATH_STRING_IMPL_H_INCLUDED
#define FS_PATH_STRING_IMPL_H_INCLUDED

#include <xray/fs/path_part_iterator.h>
#include <xray/fs/platform_configuration.h>

namespace xray {
namespace fs_new {

inline
void   verify_path_string (pcstr string_to_verify, pcstr string_to_assert, char separator);

#if !XRAY_STATIC_LIBRARIES
template class XRAY_FS_API fixed_string<max_path_length>;
#endif // #if !XRAY_STATIC_LIBRARIES

class XRAY_FS_API path_string_impl
{
public:
	typedef	fixed_string<max_path_length>			string_type;
	typedef	string_type::value_type					value_type;
	typedef	string_type::size_type					size_type;
	typedef	string_type::pointer					pointer;
	typedef	string_type::difference_type			difference_type;
	typedef	string_type::self_type					self_type;
	
	typedef	string_type::iterator					iterator;
	typedef	string_type::reverse_iterator			reverse_iterator;
	typedef	string_type::const_iterator				const_iterator;
	typedef	string_type::const_reverse_iterator		const_reverse_iterator;

	static const size_type npos	=					(size_type)(-1);

	typedef	path_part_iterator						path_part_iterator;
	typedef	reverse_path_part_iterator				reverse_path_part_iterator;
public:
	//-----------------------------------------------------------------------------------
	// initializations
	//-----------------------------------------------------------------------------------
								path_string_impl	(char separator);
								path_string_impl	(char separator, path_string_impl const & src);
								template <class T>
								path_string_impl	(char separator, T const & src);
								path_string_impl	(char separator, value_type const *	src, size_type const & count);
								template <class src_iterator>
								path_string_impl	(char separator, src_iterator const & begin, src_iterator const & end);

	path_string_impl const &	operator =		(path_string_impl const & s);
	template <class src_type>
 	path_string_impl const &	operator =		(src_type const & s);

	template <class src_type>
	path_string_impl const &	assign_with_conversion	(src_type const & s);
	template <class input_iterator>
	path_string_impl const &	assign_with_conversion	(input_iterator const begin, input_iterator const end);
	path_string_impl const &	assign_with_conversion	(path_string_impl const & s) { return assign_with_conversion(s.begin(), s.end()); }
	path_string_impl const &	assignf_with_conversion	(pcstr const format, ...);
	template <class src_type>
	path_string_impl const &	append_with_conversion	(src_type const & s);
	path_string_impl const &	append_with_conversion	(path_string_impl const & s) { return append_with_conversion(s.begin(), s.end()); }
	path_string_impl const &	appendf_with_conversion	(pcstr const format, ...);
	template <class input_iterator>
	path_string_impl const &	append_with_conversion	(input_iterator const begin, input_iterator const end);

	//-----------------------------------------------------------------------------------
	// compare
	//-----------------------------------------------------------------------------------
	inline bool	operator == 	(path_string_impl const & other) const { R_ASSERT_CMP (other.m_separator, ==, m_separator, "path of wrong type passed (self: '%s', other: '%s')", c_str(), other.c_str()); return (m_string == other.m_string); }
	inline bool	operator == 	(pcstr path) const { return m_string == path; }
	inline bool	operator != 	(path_string_impl const & other) const { return !( *this == other ); }
	inline bool	operator != 	(pcstr path) const { return !( *this == path); }

	inline bool operator !		() const { return empty(); }

	//-----------------------------------------------------------------------------------
	// operations
	//-----------------------------------------------------------------------------------
	path_string_impl const &	operator +=		(path_string_impl const & s) { return append(s); }
	template <class src_type>
	path_string_impl const &	operator +=		(src_type const & s) { return append(s); }
	inline path_string_impl &   assignf			(pcstr const format, ...);
	inline path_string_impl &   appendf			(pcstr const format, ...);
	inline path_string_impl &	append			(path_string_impl const & s);
	template <class src_type>
	inline path_string_impl &	append			(src_type const & s);
	template <class input_iterator>
	inline path_string_impl &	append			(input_iterator const & begin, input_iterator const & end);
	template <class src_type>
	inline path_string_impl &	assign			(src_type const & s);
	template <class input_iterator>
	inline path_string_impl &	assign			(input_iterator const & begin, input_iterator const & end);

	inline path_string_impl &	append_repeat	(pcstr string_to_repeat, u32 count);
	path_string_impl &			appendf_va_list	(pcstr const format, va_list argptr);
	path_string_impl &			appendf_va_list_with_conversion	(pcstr const format, va_list argptr);

	template <class src_type> 
	path_string_impl &			append_path		(src_type const & s);

	template <typename input_iterator>
	inline void					erase			(input_iterator const&	begin_src,
												 input_iterator const&	end_src) { m_string.erase(begin_src, end_src); }

	inline void					set				(size_type pos, value_type c) { verify(c); m_string.set(pos, c); }
	inline void					set_length		(size_type size) { m_string.set_length(size); }

	template <class T>
	inline size_type			find			(T const & c) const { return m_string.find(c); }
	template <class T>
	inline size_type			find			(T const & c, size_type offs) const { return m_string.find(c, offs); }
	inline size_type			rfind			(value_type const  c) const { return m_string.rfind(c); }
	inline size_type			rfind			(value_type const  c, size_type offs) const { return m_string.rfind(c, offs); }

	inline void					rtrim			(pcstr string) { m_string.rtrim(string); }
	inline void					rtrim			(value_type  c) { m_string.rtrim(c); }
	inline void					rtrim			() { m_string.rtrim(); }
	inline void					clear			() { m_string.clear(); }

	path_string_impl &			make_uppercase	() { m_string.make_uppercase(); return * this; }
    path_string_impl &			make_lowercase	() { m_string.make_lowercase(); return * this; }
	bool						is_lowercase	() const { return m_string.is_lowercase(); }

	inline u32					count_of		(value_type char_to_count) const { return m_string.count_of(char_to_count); }
	path_string_impl &			replace			(pcstr what, pcstr with) { verify(with); m_string.replace(what, with); return * this; }
	path_string_impl &			assign_replace	(pcstr source, pcstr what, pcstr with) { m_string.assign_replace(source, what, with); verify_self(); return * this; }

	void						substr			(size_type pos, size_type count, buffer_string * out_dest) const
								{ m_string.substr(pos, count, out_dest); } 
	path_string_impl			substr			(size_type pos, size_type count = buffer_string::npos) const
								{ return path_string_impl(m_separator, m_string.substr(pos, count)); }

	bool						ends_with		(pcstr string) const { return m_string.ends_with(string); }
	bool						ends_with		(char c) const { return last() == c; }
	bool						starts_with		(pcstr string) const { return m_string.starts_with(string); }

	//-----------------------------------------------------------------------------------
	// accessors
	//-----------------------------------------------------------------------------------
	inline value_type const		operator []		(size_type i) const { return m_string[i]; }
	inline value_type &			operator []		(size_type i) { return m_string[i]; }
	inline u32					length			() const { return m_string.length(); }
	inline u32					max_length		() const { return m_string.max_length(); }
	inline bool					empty			() const { return m_string.empty(); }
	inline value_type const *	c_str			() const { return m_string.c_str(); }
	inline char					last			() const { return m_string.last(); }

	//-----------------------------------------------------------------------------------
	// iterators
	//-----------------------------------------------------------------------------------
	inline iterator				begin			() { return m_string.begin(); }
	inline iterator				end				() { return m_string.end(); }
	inline const_iterator		begin			() const { return m_string.begin(); }
	inline const_iterator		end				() const { return m_string.end(); }
	inline reverse_iterator		rbegin			() { return m_string.rbegin(); }
	inline reverse_iterator		rend			() { return m_string.rend(); }
	inline const_reverse_iterator rbegin		() const { return m_string.rbegin(); }
	inline const_reverse_iterator rend			() const { return m_string.rend(); }

	inline path_part_iterator			begin_part		(include_empty_string_in_iteration_bool include = include_empty_string_in_iteration_false) const { return path_part_iterator(c_str(), length(), m_separator, include); }
	inline path_part_iterator			end_part		() const { return path_part_iterator::end(); }
	inline reverse_path_part_iterator	rbegin_part		() const { return reverse_path_part_iterator(c_str(), length(), m_separator); }
	inline reverse_path_part_iterator	rend_part		() const { return reverse_path_part_iterator::end(); }
	
	// use with caution
	buffer_string &				as_buffer_string	() { return m_string; }
	buffer_string const &		as_buffer_string	() const { return m_string; }

	typedef pcstr				( path_string_impl::*unspecified_bool_type ) ( ) const;
	operator unspecified_bool_type		() const { return length() ? (& path_string_impl::c_str) : NULL; }

	void						verify_self	()	
	{ 
		m_string.verify_self				(); 
		verify								(m_string.get_buffer()); 
	}

protected:
	void						verify		(char c)	{ char string[2] = { c, 0 }; verify(string); }
	void						verify		(pcstr string) { verify_path_string	(string, m_string.c_str(), m_separator); }

	void						convert		(iterator begin, iterator end);

private:
	fixed_string<max_path_length>	m_string;
	char							m_separator;
};

inline bool	operator <					(path_string_impl const & left, path_string_impl const & right);
inline bool	operator >					(path_string_impl const & left, path_string_impl const & right);

} // namespace fs_new
} // namespace xray

#include "path_string_impl_inline.h"

#endif // #ifndef FS_PATH_STRING_IMPL_H_INCLUDED