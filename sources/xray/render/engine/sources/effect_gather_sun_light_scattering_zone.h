////////////////////////////////////////////////////////////////////////////
//	Created		: 07.06.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_MAKE_SUN_LIGHT_SCATTERING_ZONE_H_INCLUDED
#define EFFECT_MAKE_SUN_LIGHT_SCATTERING_ZONE_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_gather_sun_light_scattering_zone: public effect_descriptor
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_gather_sun_light_scattering_zone

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_MAKE_SUN_LIGHT_SCATTERING_ZONE_H_INCLUDED