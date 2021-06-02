////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VFS_API_H_INCLUDED
#define XRAY_VFS_API_H_INCLUDED

#ifndef XRAY_VFS_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_VFS_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_VFS_BUILDING
#			define XRAY_VFS_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_VFS_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_VFS_API	XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_VFS_API	XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_VFS_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_VFS_API

namespace xray {
namespace vfs {

// used only in tests
typedef memory::pthreads_allocator_type		allocator_type;

struct world;
struct engine;

XRAY_VFS_API	void	temp_helper		( );

XRAY_VFS_API	world*	create_world	( engine& engine );
XRAY_VFS_API	void	destroy_world	( world*& world );
XRAY_VFS_API	void	memory_allocator( allocator_type& allocator );

} // namespace vfs
} // namespace xray

#endif // #ifndef XRAY_VFS_API_H_INCLUDED