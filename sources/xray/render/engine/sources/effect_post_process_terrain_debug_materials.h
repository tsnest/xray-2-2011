////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_POST_PROCESS_TERRAIN_DEBUG_MATERIALS_H_INCLUDED
#define EFFECT_POST_PROCESS_TERRAIN_DEBUG_MATERIALS_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_post_process_terrain_debug_materials: public effect_descriptor
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
	
}; // class effect_post_process_terrain_debug_materials

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_POST_PROCESS_TERRAIN_DEBUG_MATERIALS_H_INCLUDED