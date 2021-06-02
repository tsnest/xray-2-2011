////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2010
//	Author		: Alexander Maniluk
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ASIO_TYPES_MULTI_H_INCLUDED
#define ASIO_TYPES_MULTI_H_INCLUDED

#include "boost_api.h"

namespace xray {
namespace network {
namespace lowlevel {

typedef boost::asio::io_service					asio_io_service;
typedef boost::asio::deadline_timer				deadline_timer;
typedef boost::asio::io_service::work			io_service_work;
typedef boost::system::error_code				error_code;
typedef boost::asio::ip::address_v4				ip_address_v4;
typedef boost::asio::ip::address_v6				ip_address_v6;

typedef unsigned short							port_type;

#if XRAY_PLATFORM_XBOX_360

typedef boost::asio::ip::xsp::udp::endpoint		datagram_endpoint;
typedef boost::asio::ip::xsp::udp::socket		datagram_socket;
typedef	boost::asio::ip::xsp::udp::resolver		datagram_address_resolver;

typedef boost::asio::ip::xsp::tcp::endpoint		stream_endpoint;
typedef boost::asio::ip::xsp::tcp::socket		stream_socket;	
typedef boost::asio::ip::xsp::tcp::acceptor		stream_acceptor;
typedef	boost::asio::ip::xsp::tcp::resolver		stream_address_resolver;

typedef boost::asio::ip::xsp::xbox_lan_address_server	lan_address_server;

#else // #if XRAY_PLATFORM_XBOX_360

typedef boost::asio::ip::udp::endpoint			datagram_endpoint;
typedef boost::asio::ip::udp::socket			datagram_socket;
typedef	boost::asio::ip::udp::resolver			datagram_address_resolver;
												
typedef boost::asio::ip::tcp::endpoint			stream_endpoint;
typedef boost::asio::ip::tcp::socket			stream_socket;	
typedef boost::asio::ip::tcp::acceptor			stream_acceptor;
typedef	boost::asio::ip::tcp::resolver			stream_address_resolver;

#endif // #if XRAY_PLATFORM_XBOX_360

} // namespace lowlevel
} // namespace network
} // namespace xray

#endif // #ifndef ASIO_TYPES_MULTI_H_INCLUDED