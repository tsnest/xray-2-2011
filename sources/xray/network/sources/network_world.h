////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef NETWORK_WORLD_H_INCLUDED
#define NETWORK_WORLD_H_INCLUDED

#include <xray/network/world.h>
#include "platform_initializer.h"

namespace xray {
namespace network {

namespace lowlevel {
	class io_service;
} // namespace lowlevel

struct engine;

class network_world :
	public network::world,
	private boost::noncopyable
{
public:
					network_world	( network::engine& engine );
	virtual			~network_world	( );
	virtual	void	tick			(  u32 const logic_frame_id);
	virtual	void	clear_resources	( );

private:
	engine&							m_engine;
	lowlevel::platform_initializer	m_net_init;
	lowlevel::io_service*			m_ioservice;
}; // class world

} // namespace network
} // namespace xray

#endif // #ifndef NETWORK_WORLD_H_INCLUDED