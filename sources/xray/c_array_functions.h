////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_C_ARRAY_FUNCTIONS_H_INCLUDED
#define XRAY_C_ARRAY_FUNCTIONS_H_INCLUDED

namespace xray {

template <class T, u32 Count>
inline u32 array_size ( T (&)[Count] )
{
	return	Count;
}

template <class T, u32 Count>
inline T * array_begin ( T (&array)[Count] )
{
	return	& array[0];
}

template <class T, u32 Count>
inline T * array_end ( T (&array)[Count] )
{
	return	& array[Count];
}

namespace detail {

template <typename T, size_t N>
char	( &array_size_helper(T (&array)[N]) ) [N];

} // namespace detail

template <typename T>
struct array_size_helper
{
	enum { count = sizeof( detail::array_size_helper( *(T*)0 ) ) };
}; // struct array_size_helper

} // namespace xray

#endif // #ifndef XRAY_C_ARRAY_FUNCTIONS_H_INCLUDED