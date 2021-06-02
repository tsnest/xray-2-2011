////////////////////////////////////////////////////////////////////////////
//	Created		: 15.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_POST_PROCESS_DISTORTION_MATERIALS_H_INCLUDED
#define EFFECT_POST_PROCESS_DISTORTION_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_post_process_distortion_materials: public effect_material_base
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
	
}; // class effect_post_process_distortion_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_POST_PROCESS_DISTORTION_MATERIALS_H_INCLUDED