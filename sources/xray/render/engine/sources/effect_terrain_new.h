////////////////////////////////////////////////////////////////////////////
//	Created		: 04.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_TERRAIN_NEW_H_INCLUDED
#define BLENDER_TERRAIN_NEW_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_terrain_NEW : public effect_descriptor {
public:
	effect_terrain_NEW();

	virtual void compile( effect_compiler& compiler, custom_config_value const& config);
	//virtual	void load(memory::reader& mem_reader);

private:
}; // class effect_terrain_NEW

} // namespace render
} // namespace xray


#endif // #ifndef BLENDER_TERRAIN_NEW_H_INCLUDED