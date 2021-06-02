////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STD_STRING_INLINE_H_INCLUDED
#define STD_STRING_INLINE_H_INCLUDED

namespace xray {
namespace network {


inline std_string::std_string()
{
}

inline std_string::std_string(std_string const& src) : 
	base_class(src)
{
}

inline std_string::std_string(buffer_string const& src) :
	base_class(src)
{
}
inline std_string::std_string(value_type const* src) :
	base_class(src)
{
};
inline std_string::std_string(value_type	src) :
	base_class(src)
{
};
inline std_string::std_string(value_type const* src, size_type const& count) : 
	base_class(src, count)
{
};

inline std_string::~std_string()
{
}

inline std_string::value_type const* std_string::data() const
{
	return begin();
};

inline void std_string::erase(size_t pos, size_type count)
{
	iterator tmp_iter		= begin();
	std::advance			(tmp_iter, pos);
	iterator tmp_iter_end	= (count == npos) ? end() : ++tmp_iter;
	base_class::erase(tmp_iter, tmp_iter_end);
}

inline std_string::operator std::string() const
{
	FATAL("don't use std::string !");
	return std::string();
}

inline std_string & std_string::operator+(std_string const & right)
{
	base_class::append(right.c_str());
	return *this;
}

inline std_string operator+ (char const * left, std_string const & right)
{
	return std_string(left) + right;
}

inline u32 std_string::size( ) const
{
	return length();
}

} // namespace network
} // namespace xray

#endif // #ifndef STD_STRING_INLINE_H_INCLUDED