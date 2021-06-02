////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_GAME_API_H_INCLUDED
#define XRAY_GAME_API_H_INCLUDED

#ifndef XRAY_GAME_API
#	define XRAY_GAME_API
#endif // #ifndef XRAY_GAME_API

namespace xray {

namespace engine_user {
	struct engine;
	struct world;
} // namespace engine_user

namespace render {
namespace game {
	class renderer;
} // namespace game
} // namespace render

namespace sound {
	struct world;
} // namespace sound

namespace network {
	struct world;
} // namespace network

} // namespace xray

namespace stalker2 {
namespace game_module {

XRAY_GAME_API	xray::engine_user::world*	create_world				( 
												xray::engine_user::engine& engine, 
												xray::render::world& render_world, 
												xray::sound::world& sound,
												xray::network::world& network
											);

XRAY_GAME_API	void						destroy_world				( xray::engine_user::world*& world );
XRAY_GAME_API	void						register_memory_allocators	( );
XRAY_GAME_API	xray::memory::doug_lea_allocator_type&	allocator		( );

} // namespace game_module
} // namespace stalker2

#endif // #ifndef XRAY_GAME_API_H_INCLUDED