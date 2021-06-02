////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "platform_initializer.h"
#include "boost_api.h"

namespace xray {
namespace network {
namespace lowlevel {

#pragma comment(lib, "ws2_32.lib")
typedef boost::asio::detail::winsock_init<> initializer_t;

platform_initializer::platform_initializer()
{
	COMPILE_ASSERT(sizeof(m_object_store) >= sizeof(initializer_t), NotEnoughSpaceToStoreObject);
	new (m_object_store) initializer_t();
}

platform_initializer::~platform_initializer()
{
	initializer_t* tmp_initer	= xray::pointer_cast<initializer_t*>(m_object_store);
	tmp_initer->~winsock_init	();
	initializer_t::finalize		();
}


} // namespace lowlevel
} // namespace network
} // namespace xray

