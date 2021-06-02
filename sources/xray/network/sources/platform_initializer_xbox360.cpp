////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "platform_initializer.h"

#pragma comment(lib, "Xonline.lib")
#pragma comment(lib, "Xnet.lib")

namespace xray {
namespace network {
namespace lowlevel {

typedef boost::asio::detail::winsock_init<> initializer_t;

platform_initializer::platform_initializer()
{
}

platform_initializer::~platform_initializer()
{
}


} // namespace lowlevel
} // namespace network
} // namespace xray