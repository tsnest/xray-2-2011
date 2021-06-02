////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PLATFORM_EXTENSIONS_H_INCLUDED
#define XRAY_PLATFORM_EXTENSIONS_H_INCLUDED

namespace xray {
namespace platform {

inline bool	little_endian	( )
{
	u16	const word	= 0x0001;
	return			( !!*( u8 const* )&word );
}

inline bool	big_endian		( )
{
	return			( !little_endian( ) );
}

inline pcstr platform_name ( )
{
#if XRAY_PLATFORM_PS3
	return			"ps3";
#elif XRAY_PLATFORM_XBOX_360
	return			"xbox360";
#elif XRAY_PLATFORM_WINDOWS
	return			"pc_dx11";
#else
	#error			"Platform macro is not defined!"
#endif
}

} // namespace platform
} // namespace xray

#endif // #ifndef XRAY_PLATFORM_EXTENSIONS_H_INCLUDED