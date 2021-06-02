////////////////////////////////////////////////////////////////////////////
//	Created		: 14.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_FSTAGE_SMOKE_CUSTOM_LIGHTED_MATERIALS_H_INCLUDED
#define EFFECT_FSTAGE_SMOKE_CUSTOM_LIGHTED_MATERIALS_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_fstage_smoke_custom_lighted_materials: public effect_material_base
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_fstage_smoke_custom_lighted_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_FSTAGE_SMOKE_CUSTOM_LIGHTED_MATERIALS_H_INCLUDED