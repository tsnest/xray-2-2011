////////////////////////////////////////////////////////////////////////////
//	Created		: 08.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_SOUND_EDITOR_BUILDING

#define XRAY_LOG_MODULE_INITIATOR	"sound_editor"
#include <xray/editor/base/editor_extensions.h>

#pragma managed( push, off )
#include "sound_editor_memory.h"
#pragma managed( pop )

namespace xray {
namespace sound_editor {
#include <xray/editor/base/unmanaged_string.h>
} // namespace sound_editor
} // namespace xray

#include <xray/editor/base/images/images16x16.h>
#include <xray/sound/world.h>
#include <xray/render/world.h>

#pragma make_public (xray::sound_editor::allocator_type)
#pragma make_public (xray::sound::world)
#pragma make_public (xray::render::world)

#endif // #ifndef PCH_H_INCLUDED