////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_LIGHTING_STAGE_DEFAULT_MATERIALS_H_INCLUDED
#define EFFECT_LIGHTING_STAGE_DEFAULT_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_lighting_stage_default_materials: public effect_material_base
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_lighting_stage_default_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_LIGHTING_STAGE_DEFAULT_MATERIALS_H_INCLUDED
