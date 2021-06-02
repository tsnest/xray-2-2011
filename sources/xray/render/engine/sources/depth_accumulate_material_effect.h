////////////////////////////////////////////////////////////////////////////
//	Created		: 29.03.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef DEPTH_ACCUMULATE_MATERIAL_EFFECT_H_INCLUDED
#define DEPTH_ACCUMULATE_MATERIAL_EFFECT_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class depth_accumulate_material_effect : public effect_material_base 
{
private:
	virtual	void compile	( effect_compiler& compiler, custom_config_value const& config );
}; // class depth_accumulate_material_effect

} // namespace render
} // namespace xray

#endif // #ifndef DEPTH_ACCUMULATE_MATERIAL_EFFECT_H_INCLUDED