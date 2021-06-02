////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_EFFECT_FIX_SKIN_IRRADIANCE_TEXTURE_H_INCLUDED
#define XRAY_RENDER_EFFECT_FIX_SKIN_IRRADIANCE_TEXTURE_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_fix_irradiance_texture: public effect_descriptor
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_fix_irradiance_texture

} // namespace render 
} // namespace xray 


#endif // #ifndef XRAY_RENDER_EFFECT_FIX_SKIN_IRRADIANCE_TEXTURE_H_INCLUDED