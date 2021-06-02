////////////////////////////////////////////////////////////////////////////
//	Created		: 10.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STD_STRING_H_INCLUDED
#define STD_STRING_H_INCLUDED

#include <xray/fixed_string.h>
#include <string>

namespace xray {
namespace network {

class std_string : public fixed_string512
{
	typedef fixed_string512	base_class;
public:
				std_string		();
				std_string		(std_string const& src);
				std_string		(buffer_string const& src);
				std_string		(value_type const* src);
				std_string		(value_type	src);
				std_string		(value_type const* src, size_type const& count);
				~std_string		();
	
	value_type const*			data			() const;
	void						erase			(size_t	pos, size_type count = npos);
	std_string	&				operator+		(std_string const & right);
	operator					std::string		() const;
	u32							size			() const;
}; // class std_string

} // namespace network
} // namespace xray

#include "std_string_inline.h"

#endif // #ifndef STD_STRING_H_INCLUDED