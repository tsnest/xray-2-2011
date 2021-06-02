////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_API_H_INCLUDED
#define BOOST_API_H_INCLUDED


namespace boost
{
	void throw_exception(std::exception const & e);
	void throw_exception(std::bad_cast const & e);
	void throw_exception(std::bad_alloc const & e);
} // namespace boost

#include <xray/os_include.h>

//configuring boost asio

#define BOOST_NO_EXCEPTIONS
#define BOOST_ALL_NO_LIB
#define BOOST_ASIO_NO_DEFAULT_LINKED_LIBS
#define BOOST_ASIO_DISABLE_SERIAL_PORT 1
#define BOOST_ASIO_ENABLE_CANCELIO 1

#ifdef XRAY_PLATFORM_XBOX_360
#	define BOOST_ASIO_DISABLE_IOCP 1

#else // #ifdef XRAY_PLATFORM_XBOX_360
#endif // #ifdef XRAY_PLATFORM_XBOX_360

#include <boost/asio.hpp>		// warning: includes window.h

//#undef XRAY_EXTENSIONS_H_INCLUDED
//#include <xray/extensions.h>	// boost/asio.hpp conflicts with xray/extensions.h

#endif // #ifndef BOOST_API_H_INCLUDED