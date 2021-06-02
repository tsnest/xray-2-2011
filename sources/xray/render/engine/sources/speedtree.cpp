////////////////////////////////////////////////////////////////////////////
//	Created 	: 03.02.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "speedtree.h"
#include "speedtree_cook.h"
#include "material.h"

#if XRAY_PLATFORM_WINDOWS_32
#	if XRAY_STATIC_LIBRARIES
#		pragma comment( lib, "SpeedTreeCore_v5.2_VC80MT_Static.lib" )
#		pragma comment( lib, "SpeedTreeForest_v5.2_VC80MT_Static.lib" )
#	else // #endif // #if XRAY_STATIC_LIBRARIES
#		pragma comment( lib, "SpeedTreeCore_v5.2_VC80MTDLL_Static.lib" )
#		pragma comment( lib, "SpeedTreeForest_v5.2_VC80MTDLL_Static.lib" )
#	endif // #if XRAY_STATIC_LIBRARIES
#else // #if XRAY_PLATFORM_WINDOWS_32
#	if XRAY_STATIC_LIBRARIES
#		pragma comment( lib, "SpeedTreeCore_v5.2_VC80MT64_Static.lib" )
#		pragma comment( lib, "SpeedTreeForest_v5.2_VC80MT64_Static.lib" )
#	else // #endif // #if XRAY_STATIC_LIBRARIES
#		pragma comment( lib, "SpeedTreeCore_v5.2_VC80MTDLL64_Static.lib" )
#		pragma comment( lib, "SpeedTreeForest_v5.2_VC80MTDLL64_Static.lib" )
#	endif // #if XRAY_STATIC_LIBRARIES
#endif // #if XRAY_PLATFORM_WINDOWS_32

// #pragma comment(lib, "nvtristrip.lib")
// 
// #include "NvTriStrip.h"


#include <xray/engine/console.h>
#include <xray/console_command.h>
#include <xray/console_command_processor.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_texture.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/untyped_buffer.h>

static u32 num_speedtree_memory_used = 0;

namespace xray {
namespace render {

using namespace SpeedTree;

struct speed_tree_allocator : public CAllocator
{
	virtual void* Alloc(size_t size)
	{
		void* block = MALLOC(size, "speed_tree_allocator");
		
		num_speedtree_memory_used += size;
		
		if (block == 0)
			throw std::bad_alloc();
		else
			return block;
	}
	virtual void Free(void* block)
	{
		if (block)
			FREE(block);
	}
};

static xray::uninitialized_reference<xray::render::speedtree_cook>				s_speedtree_cook;
static xray::uninitialized_reference<xray::render::speedtree_instance_cook>		s_speedtree_instance_cook;

void initialize_speedtree		()
{
	static speed_tree_allocator s_speed_tree_allocator;
	
	if (!CCore::IsAuthorized())
	{
		ASSERT(0, "The SpeedTree SDK has not been authorized; expired evaluation key?");
	}
	
	CAllocatorInterface cOn		(&s_speed_tree_allocator);
	
	CCoordSys::SetCoordSys		(CCoordSys::COORD_SYS_LEFT_HANDED_Y_UP);
	
	XRAY_CONSTRUCT_REFERENCE	(s_speedtree_cook, speedtree_cook);
	resources::register_cook	(s_speedtree_cook.c_ptr());
	
	XRAY_CONSTRUCT_REFERENCE	(s_speedtree_instance_cook, speedtree_instance_cook);
	resources::register_cook	(s_speedtree_instance_cook.c_ptr());
}

void finalize_speedtree			()
{
	resources::unregister_cook	(resources::speedtree_class);
	XRAY_DESTROY_REFERENCE		(s_speedtree_cook);
	
	resources::unregister_cook	(resources::speedtree_instance_class);
	XRAY_DESTROY_REFERENCE		(s_speedtree_instance_cook);
}

void	print_speedtree_errors	()
{
	const char* pError = SpeedTree::CCore::GetError();
	while (pError)
	{
		LOG_ERROR("SpeedTree Error: %s\n", pError);
		pError = CCore::GetError();
	}
}

} // namespace render
} // namespace xray