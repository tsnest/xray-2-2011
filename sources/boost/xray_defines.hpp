////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEFINES_HPP_INCLUDED
#define XRAY_DEFINES_HPP_INCLUDED

#undef	BOOST_ASSERT
#undef	BOOST_STATIC_ASSERT

#define BOOST_ASSERT ASSERT
#define BOOST_STATIC_ASSERT(x) COMPILE_ASSERT(x, compile_error)


#endif // #ifndef XRAY_DEFINES_HPP_INCLUDED