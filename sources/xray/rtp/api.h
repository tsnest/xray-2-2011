////////////////////////////////////////////////////////////////////////////
//	Created		: 23.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RTP_API_H_INCLUDED
#define XRAY_RTP_API_H_INCLUDED

#include <xray/render/engine/base_classes.h>

#ifndef XRAY_RTP_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_RTP_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_RTP_BUILDING
#			define XRAY_RTP_API						XRAY_DLL_EXPORT
#		else // #ifdef XRAY_RTP_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_RTP_API					XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_RTP_API					XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_RTP_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_RTP_API

#if !XRAY_PLATFORM_XBOX_360 && !XRAY_PLATFORM_PS3 && (!defined(MASTER_GOLD) )
#		define	XRAY_RTP_LEARNING	1
#	if (!XRAY_DEBUG_ALLOCATOR)
#		error	rtp uses debug allocator for learning
#	endif
#else
#		define	XRAY_RTP_LEARNING	0
#endif

namespace xray {

namespace animation {
	struct world;
} // namespace animation

namespace render {
namespace game {
	class renderer;
} // namespace game
} // namespace render

namespace rtp {

struct engine;
struct world;

typedef memory::base_allocator				allocator_type;

XRAY_RTP_API	world*	create_world		(
												engine& engine,
												animation::world& animation,
												render::scene_ptr const& scene,
												render::game::renderer* renderer,
												pcstr const resources_xray_path
											);
XRAY_RTP_API	void	destroy_world		( world*& world );
XRAY_RTP_API	void	set_memory_allocator( memory::base_allocator& allocator );

} // namespace rtp
} // namespace xray

#endif // #ifndef XRAY_RTP_API_H_INCLUDED