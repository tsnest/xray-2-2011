#ifndef FIXED_STRING_H_INCLUDED
#define FIXED_STRING_H_INCLUDED

#include "buffer_string.h"

namespace xray {
namespace core {

template <int Size>
class fixed_string : public buffer_string
{
public:
	typedef	fixed_string	self_type;

public:
	//-----------------------------------------------------------------------------------
	// initializations
	//-----------------------------------------------------------------------------------
					fixed_string	();
					fixed_string	(const fixed_string& src);
					fixed_string	(const value_type*	 src);
					fixed_string	(value_type			 src);
					fixed_string	(const value_type*   src, size_type const& count);

					template <int SrcSize>
					fixed_string	(const fixed_string<SrcSize>& src);

					template <typename input_iterator>
					fixed_string	(input_iterator const& begin, input_iterator const& end);

	fixed_string&	operator =		(const value_type*	src);
	fixed_string&	operator =		(value_type			ch);
	fixed_string&	operator =		(const fixed_string& src);
	template <int SrcSize>
	fixed_string&	operator =		(const fixed_string<SrcSize>& src);

	char*			get_buffer		() { return m_buffer; } // when writing NULL, call set_size

	//-----------------------------------------------------------------------------------
	// operations
	//-----------------------------------------------------------------------------------
	void			substr			(size_t pos, size_t count, fixed_string& dest) const;
	fixed_string	substr			(size_t pos, size_t count=npos) const;

private:
	char			m_buffer[Size];
};

} // namespace core
} // namespace xray

#include "fixed_string_inline.h"

#endif // FIXED_STRING_H_INCLUDED