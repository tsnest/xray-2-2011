#ifndef BUFFER_STRING_H_INCLUDED
#define BUFFER_STRING_H_INCLUDED

namespace xray {
namespace core {

class buffer_string
{
public:
	typedef	char									value_type;
	typedef	size_t									size_type;
	typedef	char*									pointer;
	typedef	ptrdiff_t								difference_type;
	typedef	buffer_string							self_type;
	
	typedef	value_type*								iterator;
	typedef std::reverse_iterator<iterator>			reverse_iterator;
	typedef	const value_type*						const_iterator;
	typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;

	static const size_type npos	=					(size_type)(-1);

public:
	//-----------------------------------------------------------------------------------
	// initializations
	//-----------------------------------------------------------------------------------
							buffer_string	(value_type* buffer, size_type const& max_count);
							buffer_string	(value_type* buffer, size_type const& max_count, size_type const& count);
							buffer_string	(value_type* buffer, size_type const& max_count, const value_type* src);
							template <typename input_iterator>
							buffer_string	(value_type*			buffer, 
											 size_type const&		max_count, 
											 input_iterator const&	begin_src, 
											 input_iterator const&	end_src);

	//-----------------------------------------------------------------------------------
	// operations
	//-----------------------------------------------------------------------------------
	void					operator +=		(const value_type* s);
	void					operator +=		(value_type  c);
	void					operator +=		(const buffer_string& s);

	template <typename input_iterator>
	void					append			(input_iterator const&	begin_src,
											 input_iterator const&	end_src);
	template <typename input_iterator>
	void					erase			(input_iterator const&	begin_src,
											 input_iterator const&	end_src);

	void					set				(size_type pos, value_type c); // more secure then operator []
	void					set_size		(size_type size);

	size_type				find			(const value_type  c) const;
	size_type				find			(const value_type  c, size_type offs) const;
	size_type				find			(const value_type* s) const;
	size_type				find			(const value_type* s, size_type offs) const;

	void					rtrim			(value_type  c);
	void					clear			();

	void					substr			(size_t pos, size_t count, buffer_string& dest) const;

	//-----------------------------------------------------------------------------------
	// accessors
	//-----------------------------------------------------------------------------------
	value_type&				operator []		(size_type i); // when placing NULL, call set_size
	value_type				operator []		(size_type i) const;
	u32						size			() const;
	u32						max_size		() const;
	bool					empty			() const;
	value_type*				c_str			();
	const value_type*		c_str			() const;

	//-----------------------------------------------------------------------------------
	// iterators
	//-----------------------------------------------------------------------------------
	iterator				begin			();
	iterator				end				();
	const_iterator			begin			() const;
	const_iterator			end				() const;
	reverse_iterator		rbegin			();
	reverse_iterator		rend			();
	const_reverse_iterator	rbegin			() const;
	const_reverse_iterator	rend			() const;

private:
	pointer					m_begin;
	pointer					m_end;
	pointer					m_max_end;

	//-----------------------------------------------------------------------------------
	// private
	//-----------------------------------------------------------------------------------
	buffer_string							();
	void					construct		(value_type* buffer, size_type const& max_count);
	void					construct		(value_type* buffer, size_type const& max_count, size_type const& count);
};

//-----------------------------------------------------------------------------------
// buffer_string outer operators
//-----------------------------------------------------------------------------------
bool	operator <  (const buffer_string&				s1,	const buffer_string&				s2);
bool	operator <  (const buffer_string::value_type*	s1, const buffer_string&				s2);
bool	operator <  (const buffer_string&				s1, const buffer_string::value_type*	s2);
bool	operator == (const buffer_string&				s1,	const buffer_string&				s2);
bool	operator == (const buffer_string::value_type*	s1, const buffer_string&				s2);
bool	operator == (const buffer_string&				s1, const buffer_string::value_type*	s2);
bool	operator >  (const buffer_string&				s1,	const buffer_string&				s2);
bool	operator >  (const buffer_string::value_type*	s1, const buffer_string&				s2);
bool	operator >  (const buffer_string&				s1, const buffer_string::value_type*	s2);

} // namespace core
} // namespace xray

#include "buffer_string_inline.h"

#endif // BUFFER_STRING_H_INCLUDED