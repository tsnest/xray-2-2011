////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_EFFECT_LIGHTING_STAGE_ORGANIC_BASE_MATERIALS_H_INCLUDED
#define XRAY_RENDER_ENGINE_EFFECT_LIGHTING_STAGE_ORGANIC_BASE_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_lighting_stage_organic_base_materials: public effect_material_base
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_lighting_stage_organic_base_materials

} // namespace render 
} // namespace xray 

#endif // #ifndef XRAY_RENDER_ENGINE_EFFECT_LIGHTING_STAGE_ORGANIC_BASE_MATERIALS_H_INCLUDED
