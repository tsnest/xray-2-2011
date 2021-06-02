////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Description : physics API
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_PHYSICS_API_H_INCLUDED
#define XRAY_PHYSICS_API_H_INCLUDED
#include <xray/render/engine/base_classes.h>

#ifndef XRAY_PHYSICS_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_PHYSICS_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_PHYSICS_BUILDING
#			define XRAY_PHYSICS_API				XRAY_DLL_EXPORT
#		else // #ifdef XRAY_PHYSICS_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_PHYSICS_API			XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_PHYSICS_API			XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_PHYSICS_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_PHYSICS_API

namespace xray {

namespace render {
	namespace debug {
		class renderer;
	} // namespace debug
	class world;
} // namespace render

namespace physics {

struct engine;
struct world;

XRAY_PHYSICS_API	world*	create_world		( xray::memory::base_allocator* allocator, physics::engine& engine );

XRAY_PHYSICS_API	world*	create_world_bt		( xray::memory::base_allocator* allocator, physics::engine& engine );
XRAY_PHYSICS_API	void	destroy_world		( xray::memory::base_allocator* allocator, world* w );

XRAY_PHYSICS_API	void	set_memory_allocator( xray::memory::base_allocator* allocator );

} // namespace physics
} // namespace xray

#endif // #ifndef XRAY_PHYSICS_API_H_INCLUDED