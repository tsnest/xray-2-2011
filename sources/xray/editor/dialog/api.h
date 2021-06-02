////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DIALOG_EDITOR_API_H_INCLUDED
#define XRAY_DIALOG_EDITOR_API_H_INCLUDED

#define XRAY_DIALOG_EDITOR_API

namespace xray {
namespace dialog_editor {
	typedef memory::doug_lea_allocator_type allocator_type;

} // namespace dialog_editor
} // namespace xray

#pragma make_public (xray::dialog_editor::allocator_type)

#endif // #ifndef XRAY_DIALOG_EDITOR_API_H_INCLUDED