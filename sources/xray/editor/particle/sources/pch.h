////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_PARTICLE_EDITOR_BUILDING

#define XRAY_LOG_MODULE_INITIATOR	"particle_editor"
#include <xray/editor/base/editor_extensions.h>


#pragma managed( push, off )
#include "particle_editor_memory.h"
#pragma managed( pop )

namespace xray {
namespace particle_editor {
#include <xray/editor/base/unmanaged_string.h>
} // namespace particle_editor
} // namespace xray

#include <xray/editor/base/images/images16x16.h>
#include <xray/render/facade/editor_renderer.h>

#pragma make_public (xray::particle_editor::allocator_type)
#include <xray/render/world.h>
#pragma make_public (xray::render::world)

#endif // #ifndef PCH_H_INCLUDED