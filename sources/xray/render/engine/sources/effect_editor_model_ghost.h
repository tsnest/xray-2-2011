////////////////////////////////////////////////////////////////////////////
//	Created		: 28.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_EDITOR_MODEL_GHOST_H_INCLUDED
#define EFFECT_EDITOR_MODEL_GHOST_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_editor_model_ghost: public effect_material_base
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_editor_model_ghost

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_EDITOR_MODEL_GHOST_H_INCLUDED
