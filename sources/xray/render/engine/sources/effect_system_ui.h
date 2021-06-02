////////////////////////////////////////////////////////////////////////////
//	Created		: 17.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_SYSTEM_UI_H_INCLUDED
#define BLENDER_SYSTEM_UI_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_system_ui: public effect_descriptor
{
public:
	effect_system_ui () : effect_descriptor() {}
	~effect_system_ui() {}

	enum techniques{ ui_font, ui, ui_fill, ui_line_strip };

	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);

private:

};

} // namespace render
} // namespace xray

#endif // #ifndef BLENDER_SYSTEM_UI_H_INCLUDED