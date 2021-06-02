////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_EDITOR_BASE_BUILDING

#define XRAY_LOG_MODULE_INITIATOR	"editor_base"
#include <xray/editor/base/editor_extensions.h>

#include "property_holder_delegates_types.h"

#pragma managed( push, off )
#include "editor_base_memory.h"
#pragma managed( pop )

namespace xray {
namespace editor_base {
#	include <xray/editor/base/unmanaged_string.h>
} // namespace editor_base
} // namespace xray

#include "make_public_helper.h"

#endif // #ifndef PCH_H_INCLUDED
