////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_H_INCLUDED
#define PCH_H_INCLUDED

#define XRAY_DIALOG_EDITOR_BUILDING

#define XRAY_LOG_MODULE_INITIATOR	"dialog_editor"
#include <xray/editor/base/editor_extensions.h>

#pragma managed( push, off )
#	include "dialog_editor_memory.h"
#pragma managed( pop )

#include <xray/editor/base/images/images16x16.h>

namespace xray {
namespace dialog_editor {
#	include <xray/editor/base/unmanaged_string.h>
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef PCH_H_INCLUDED