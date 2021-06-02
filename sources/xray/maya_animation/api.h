////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.01.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEFAULT_API_H_INCLUDED
#define XRAY_DEFAULT_API_H_INCLUDED

#ifndef XRAY_DEFAULT_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_MAYA_ANIMATION_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_MAYA_ANIMATION_BUILDING
#			define XRAY_MAYA_ANIMATION_API			XRAY_DLL_EXPORT
#		else // #ifdef XRAY_MAYA_ANIMATION_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_MAYA_ANIMATION_API		XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_MAYA_ANIMATION_API		XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_MAYA_ANIMATION_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_DEFAULT_API

namespace xray {
namespace maya_animation {

struct engine;
struct world;

typedef memory::doug_lea_allocator_type				allocator_type;

XRAY_MAYA_ANIMATION_API	world*	create_world		( engine& engine );
XRAY_MAYA_ANIMATION_API	void	destroy_world		( world*& world );
XRAY_MAYA_ANIMATION_API	void	set_memory_allocator( allocator_type& allocator );

} // namespace maya_animation
} // namespace xray

#endif // #ifndef XRAY_DEFAULT_API_H_INCLUDED