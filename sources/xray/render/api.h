////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_API_H_INCLUDED
#define XRAY_RENDER_API_H_INCLUDED

#ifndef XRAY_RENDER_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_RENDER_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_RENDER_BUILDING
#			define XRAY_RENDER_API				XRAY_DLL_EXPORT
#		else // #ifdef XRAY_RENDER_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_RENDER_API			XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_RENDER_API			XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_RENDER_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_RENDER_API

#if XRAY_PLATFORM_PS3
	typedef pvoid	HWND;
#endif // #if XRAY_PLATFORM_PS3

namespace xray {
namespace particle {
	struct world;
} // namespace particle
} // namespace xray

namespace xray {
namespace render { 

class world;

typedef memory::base_allocator					logic_allocator_type;
typedef memory::base_allocator					editor_allocator_type;
typedef memory::doug_lea_allocator_type			render_allocator_type;

XRAY_RENDER_API	void	set_memory_allocator	(
							render_allocator_type& render_allocator,
							logic_allocator_type& logic_allocator,
							editor_allocator_type& editor_allocator
						);
XRAY_RENDER_API	world*	create_world			(
							memory::base_allocator& logic_allocator,
							memory::base_allocator* editor_allocator
						);
XRAY_RENDER_API	void	destroy_world			(
							world*& world
						);

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_API_H_INCLUDED