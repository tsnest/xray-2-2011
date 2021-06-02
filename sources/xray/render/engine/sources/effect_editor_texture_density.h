////////////////////////////////////////////////////////////////////////////
//	Created		: 13.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_EDITOR_TEXTURE_DENSITY_H_INCLUDED
#define EFFECT_EDITOR_TEXTURE_DENSITY_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_editor_texture_density: public effect_material_base
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_editor_texture_density

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_EDITOR_TEXTURE_DENSITY_H_INCLUDED
