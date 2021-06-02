////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_DEBUG_EDITOR_WIREFRAME_H_INCLUDED
#define EFFECT_DEBUG_EDITOR_WIREFRAME_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_debug_editor_wireframe: public effect_material_base
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_debug_editor_wireframe

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_DEBUG_EDITOR_WIREFRAME_H_INCLUDED