////////////////////////////////////////////////////////////////////////////
//	Created		: 22.11.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_API_H_INCLUDED
#define XRAY_AI_API_H_INCLUDED

#ifndef XRAY_AI_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_AI_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_AI_BUILDING
#			define XRAY_AI_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_AI_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_AI_API	XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_AI_API	XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_AI_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_AI_API

namespace xray {
namespace ai {

typedef memory::doug_lea_allocator_type	allocator_type;

struct world;
struct engine;

XRAY_AI_API	world*	create_world		( engine& engine );
XRAY_AI_API	void	destroy_world		( world*& world );
XRAY_AI_API	void	set_memory_allocator( allocator_type& allocator );

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_API_H_INCLUDED