////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_API_H_INCLUDED
#define XRAY_RENDER_BASE_API_H_INCLUDED

#include <xray/render/api.h>

#ifndef XRAY_RENDER_BASE_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_RENDER_BASE_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_RENDER_BASE_BUILDING
#			define XRAY_RENDER_BASE_API			XRAY_DLL_EXPORT
#		else // #ifdef XRAY_RENDER_BASE_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_RENDER_BASE_API		XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_RENDER_BASE_API		XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_RENDER_BASE_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_RENDER_API

namespace xray {
namespace render {

typedef memory::base_allocator										logic_allocator_type;
typedef memory::base_allocator										editor_allocator_type;
typedef memory::doug_lea_allocator_type								render_allocator_type;

XRAY_RENDER_BASE_API	void	memory_allocator	(
									render_allocator_type& render_allocator,
									logic_allocator_type& logic_allocator,
									editor_allocator_type& editor_allocator
								);

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_API_H_INCLUDED