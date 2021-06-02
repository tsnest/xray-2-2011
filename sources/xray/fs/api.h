////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_FS_API_H_INCLUDED
#define XRAY_FS_API_H_INCLUDED

#ifndef XRAY_FS_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_FS_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_FS_BUILDING
#			define XRAY_FS_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_FS_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_FS_API	XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_FS_API	XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_FS_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_FS_API

namespace xray {
namespace fs_new {

typedef memory::doug_lea_allocator_type		allocator_type;

struct world;
struct engine;

XRAY_FS_API	world*	create_world	( engine& engine );
XRAY_FS_API	void	destroy_world	( world*& world );
XRAY_FS_API	void	memory_allocator( allocator_type& allocator );

} // namespace fs_new
} // namespace xray

#endif // #ifndef XRAY_FS_API_H_INCLUDED