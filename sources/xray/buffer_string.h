////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BUFFER_STRING_H_INCLUDED
#define BUFFER_STRING_H_INCLUDED

#include <xray/strings_functions.h>
#include <iterator>

namespace xray {

class XRAY_CORE_API buffer_string
{
public:
	typedef	char									value_type;
	typedef	u32										size_type;
	typedef	char *									pointer;
	typedef	ptrdiff_t								difference_type;
	typedef	buffer_string							self_type;
	
	typedef	value_type *							iterator;
	typedef std::reverse_iterator<iterator>			reverse_iterator;
	typedef	const value_type *						const_iterator;
	typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;

	static const size_type npos	=					(size_type)(-1);

public:
	//-----------------------------------------------------------------------------------
	// initializations
	//-----------------------------------------------------------------------------------
	inline 						buffer_string	(value_type* buffer, size_type const& max_count);
	inline 						buffer_string	(value_type* buffer, size_type const& max_count, size_type const& count);
	inline 						buffer_string	(value_type* buffer, size_type const& max_count, const value_type* src);
	template <typename input_iterator>
	inline 						buffer_string	(value_type*			buffer, 
												 size_type const&		max_count, 
												 input_iterator const&	begin_src, 
												 input_iterator const&	end_src);

	inline buffer_string const&	operator =		(value_type const* s);
	inline buffer_string const&	operator =		(value_type  c);
	inline buffer_string const&	operator =		(buffer_string const& s);

	//-----------------------------------------------------------------------------------
	// operations
	//-----------------------------------------------------------------------------------

		   buffer_string const&	operator +=		(value_type const* s);
	inline buffer_string const&	operator +=		(value_type  c);
	inline buffer_string const&	operator +=		(buffer_string const& s);

	inline buffer_string &		append			(value_type const c);
	inline buffer_string &		append			(pcstr begin_src, pcstr end_src);
	inline buffer_string &		append			(pcstr c_string);
	template <typename input_iterator>
	inline buffer_string &		append			(input_iterator const&	begin_src,
												 input_iterator const&	end_src);
		   buffer_string &		append_repeat	(pcstr string_to_repeat, u32 count);

	template <typename input_iterator>
	inline buffer_string &		assign			(input_iterator const&	begin_src,
											 	 input_iterator const&	end_src);

		   buffer_string &		assignf			(const char* format, ...);
		   buffer_string &		appendf			(const char* format, ...);
		   buffer_string &		appendf_va_list	(pcstr const format, va_list argptr);

	template <typename input_iterator>
	inline void					erase			(input_iterator const&	begin_src,
												 input_iterator const&	end_src);

	inline void					set				(size_type pos, value_type c); // more secure then operator []
	inline void					set_length		(size_type size);

	inline size_type			find			(value_type const  c) const;
	inline size_type			find			(value_type const  c, size_type offs) const;
	inline size_type			find			(value_type const* s) const;
	inline size_type			find			(value_type const* s, size_type offs) const;
	inline size_type			rfind			(value_type const  c) const;
	inline size_type			rfind			(value_type const  c, size_type offs) const;
	inline bool					ends_with		(pcstr string) const; 
	inline bool					starts_with		(pcstr string) const; 


	inline void					rtrim			(pcstr string);
	inline void					rtrim			(value_type  c);
	inline void					rtrim			();
	inline void					clear			();

		   void					substr			(size_type pos, size_type count, buffer_string * out_dest) const;

		   buffer_string &		make_uppercase	();
		   buffer_string &		make_lowercase	();
		   bool					is_lowercase	() const;

	inline u32					count_of		(value_type char_to_count) const;
	buffer_string &				replace			(pcstr what, pcstr with);
	buffer_string &				assign_replace	(pcstr source, pcstr what, pcstr with);

	//-----------------------------------------------------------------------------------
	// accessors
	//-----------------------------------------------------------------------------------
	inline value_type&			operator []		(size_type i); // when placing NULL, call set_size
	inline value_type			operator []		(size_type i) const;
	inline u32					length			() const;
	inline u32					max_length		() const;
	inline bool					empty			() const;
	inline value_type*			c_str			();
	inline value_type const*	c_str			() const;
	inline char					last			() const;

	//-----------------------------------------------------------------------------------
	// iterators
	//-----------------------------------------------------------------------------------
	inline iterator				begin			();
	inline iterator				end				();
	inline const_iterator		begin			() const;
	inline const_iterator		end				() const;
	inline reverse_iterator		rbegin			();
	inline reverse_iterator		rend			();
	inline const_reverse_iterator rbegin		() const;
	inline const_reverse_iterator rend			() const;

	inline pointer				get_max_end		() const { return m_max_end; }
private:
	pointer						m_begin;
	pointer						m_end;
	pointer						m_max_end;

	//-----------------------------------------------------------------------------------
	// private
	//-----------------------------------------------------------------------------------
	inline buffer_string						();
	inline void					construct		(value_type* buffer, size_type const& max_count);
	inline void					construct		(value_type* buffer, size_type const& max_count, size_type const& count);
};

void	get_common_prefix (buffer_string * out_string, pcstr first, pcstr second);

//-----------------------------------------------------------------------------------
// buffer_string outer operators
//-----------------------------------------------------------------------------------
inline bool	operator <  (const buffer_string&				s1,	const buffer_string&				s2);
inline bool	operator <  (const buffer_string::value_type*	s1, const buffer_string&				s2);
inline bool	operator <  (const buffer_string&				s1, const buffer_string::value_type*	s2);
inline bool	operator == (const buffer_string&				s1,	const buffer_string&				s2);
inline bool	operator == (const buffer_string::value_type*	s1, const buffer_string&				s2);
inline bool	operator == (const buffer_string&				s1, const buffer_string::value_type*	s2);
inline bool	operator != (const buffer_string&				s1,	const buffer_string&				s2);
inline bool	operator != (const buffer_string::value_type*	s1, const buffer_string&				s2);
inline bool	operator != (const buffer_string&				s1, const buffer_string::value_type*	s2);
inline bool	operator >  (const buffer_string&				s1,	const buffer_string&				s2);
inline bool	operator >  (const buffer_string::value_type*	s1, const buffer_string&				s2);
inline bool	operator >  (const buffer_string&				s1, const buffer_string::value_type*	s2);

} // namespace xray

#include <xray/buffer_string_inline.h>

#endif // BUFFER_STRING_H_INCLUDED