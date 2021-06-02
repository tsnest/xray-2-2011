////////////////////////////////////////////////////////////////////////////
//        Created                : 18.08.2010
//        Author                : Sergey Pryshchepa
//        Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_EDITOR_API_H_INCLUDED
#define XRAY_ANIMATION_EDITOR_API_H_INCLUDED

static xray::command_line::key			s_animation_editor_scene_key	("animation_editor_scene", "", "", "run animation editor with scene");

namespace xray {
namespace animation_editor {
	typedef memory::doug_lea_allocator_type allocator_type;
} // namespace animation_editor
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_EDITOR_API_H_INCLUDED