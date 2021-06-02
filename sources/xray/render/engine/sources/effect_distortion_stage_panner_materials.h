////////////////////////////////////////////////////////////////////////////
//	Created		: 31.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_DISTORTION_STAGE_PANNER_MATERIALS_H_INCLUDED
#define EFFECT_DISTORTION_STAGE_PANNER_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_distortion_stage_panner_materials: public effect_material_base
{
public:
	effect_distortion_stage_panner_materials() {}
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
	
}; // class effect_distortion_stage_panner_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_DISTORTION_STAGE_PANNER_MATERIALS_H_INCLUDED