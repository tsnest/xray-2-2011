////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_EFFECT_EDITOR_ACCUMULATE_OVERDRAW_H_INCLUDED
#define XRAY_RENDER_ENGINE_EFFECT_EDITOR_ACCUMULATE_OVERDRAW_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_editor_accumulate_overdraw: public effect_material_base
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_editor_accumulate_overdraw

} // namespace render 
} // namespace xray 


#endif // #ifndef XRAY_RENDER_ENGINE_EFFECT_EDITOR_ACCUMULATE_OVERDRAW_H_INCLUDED