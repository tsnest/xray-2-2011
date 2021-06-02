////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_GBUFFER_NOMATERIAL_MATERIALS_H_INCLUDED
#define EFFECT_GBUFFER_NOMATERIAL_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_gbuffer_nomaterial_materials: public effect_material_base
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);

}; // class effect_gbuffer_nomaterial_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_GBUFFER_NOMATERIAL_MATERIALS_H_INCLUDED
