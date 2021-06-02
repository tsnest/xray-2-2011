////////////////////////////////////////////////////////////////////////////
//	Created		: 29.07.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_NAVIGATION_API_H_INCLUDED
#define XRAY_AI_NAVIGATION_API_H_INCLUDED

#include <xray/render/engine/base_classes.h>

#ifndef XRAY_AI_NAVIGATION_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_AI_NAVIGATION_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_AI_NAVIGATION_BUILDING
#			define XRAY_AI_NAVIGATION_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_AI_NAVIGATION_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_AI_NAVIGATION_API	XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_AI_NAVIGATION_API	XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_AI_NAVIGATION_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_AI_NAVIGATION_API

namespace xray {

namespace render {
namespace debug {
	class renderer;
} // namespace game
} // namespace render

namespace ai {
namespace navigation {

typedef memory::base_allocator	allocator_type;

struct world;
struct engine;

XRAY_AI_NAVIGATION_API	world*	create_world		( engine& engine, render::scene_ptr const& scene, xray::render::debug::renderer& renderer );
XRAY_AI_NAVIGATION_API	void	destroy_world		( world*& world );
XRAY_AI_NAVIGATION_API	void	set_memory_allocator( allocator_type& allocator );

XRAY_AI_NAVIGATION_API bool is_triangle_inside_triangle ( 
	u32 const (&coordinate_indices)[2],
	math::float3 const& v0,
	math::float3 const& v1,
	math::float3 const& v2,
	math::float3 const& u0,
	math::float3 const& u1,
	math::float3 const& u2
);

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_NAVIGATION_API_H_INCLUDED
