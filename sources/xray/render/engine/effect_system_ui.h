////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_SYSTEM_UI_H_INCLUDED
#define BLENDER_SYSTEM_UI_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render_dx10 {

class effect_system_ui: public effect
{
public:
	effect_system_ui () : effect() {}
	~effect_system_ui() {}

	enum techniques{ ui_font, ui, ui_fill, ui_line_strip };

	virtual void compile( effect_compiler& compiler, const effect_compilation_options& options);

private:

};

} // namespace render_dx10
} // namespace xray

#endif // #ifndef BLENDER_SYSTEM_UI_H_INCLUDED