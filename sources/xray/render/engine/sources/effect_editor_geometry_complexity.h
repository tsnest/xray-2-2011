////////////////////////////////////////////////////////////////////////////
//	Created		: 25.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_EDITOR_GEOMETRY_COMPLEXITY_H_INCLUDED
#define EFFECT_EDITOR_GEOMETRY_COMPLEXITY_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_editor_geometry_complexity: public effect_material_base
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_editor_geometry_complexity

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_EDITOR_GEOMETRY_COMPLEXITY_H_INCLUDED
