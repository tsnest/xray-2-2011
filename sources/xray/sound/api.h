////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_API_H_INCLUDED
#define XRAY_SOUND_API_H_INCLUDED

#ifndef XRAY_SOUND_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_SOUND_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_SOUND_BUILDING
#			define XRAY_SOUND_API					XRAY_DLL_EXPORT
#		else // #ifdef XRAY_SOUND_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_SOUND_API				XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_SOUND_API				XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_SOUND_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_SOUND_API

namespace xray {
namespace sound {

struct engine;
struct world;

typedef ::xray::memory::base_allocator			base_allocator_type;
typedef memory::doug_lea_allocator_type			allocator_type;

XRAY_SOUND_API	world*	create_world			( engine& engine, base_allocator_type& logic_allocator, base_allocator_type* editor_allocator );
XRAY_SOUND_API	void	destroy_world			( world*& world );
XRAY_SOUND_API	void	set_memory_allocator	( allocator_type& allocator );

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_API_H_INCLUDED