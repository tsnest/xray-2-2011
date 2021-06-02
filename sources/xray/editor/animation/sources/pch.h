////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_ANIMATION_EDITOR_BUILDING

#define XRAY_LOG_MODULE_INITIATOR	"animation_editor"
#include <xray/editor/base/editor_extensions.h>

#pragma managed( push, off )
#	include "animation_editor_memory.h"
#	include <xray/render/world.h>
#pragma managed( pop )

#include <xray/editor/base/images/images16x16.h>

namespace xray {
namespace animation_editor {
#	include <xray/editor/base/unmanaged_string.h>
} // namespace animation_editor
} // namespace xray


#pragma make_public (xray::animation_editor::allocator_type)
#pragma make_public (xray::render::world)

#endif // #ifndef PCH_H_INCLUDED