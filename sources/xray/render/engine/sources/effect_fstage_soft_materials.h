////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_FSTAGE_SOFT_MATERIALS_H_INCLUDED
#define EFFECT_FSTAGE_SOFT_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_fstage_soft_materials: public effect_material_base
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_fstage_soft_materials

} // namespace render 
} // namespace xray 

#endif // #ifndef EFFECT_FSTAGE_SOFT_MATERIALS_H_INCLUDED
