////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_DISTORTION_STAGE_DEFAULT_MATERIALS_H_INCLUDED
#define EFFECT_DISTORTION_STAGE_DEFAULT_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_distortion_stage_default_materials: public effect_material_base
{
public:
	effect_distortion_stage_default_materials() {}
	
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);

}; // class effect_distortion_stage_default_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_DISTORTION_STAGE_DEFAULT_MATERIALS_H_INCLUDED
