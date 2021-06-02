////////////////////////////////////////////////////////////////////////////
//	Created		: 23.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_MODULE_PROXY_H_INCLUDED
#define GAME_MODULE_PROXY_H_INCLUDED

#include <xray/engine/engine_user.h>

namespace stalker2 {

class game_module_proxy : public xray::engine_user::module_proxy {
private:
	virtual	xray::engine_user::world*	create_world				(
											xray::engine_user::engine& engine, 
											xray::render::world& render_world, 
											xray::sound::world& sound,
											xray::network::world& network
										);
	virtual	void						destroy_world				( xray::engine_user::world*& world );
	virtual	void						register_memory_allocators	( );
	virtual	xray::memory::doug_lea_allocator_type& allocator		( );
}; // struct game_proxy

} // namespace stalker2

#endif // #ifndef GAME_MODULE_PROXY_H_INCLUDED