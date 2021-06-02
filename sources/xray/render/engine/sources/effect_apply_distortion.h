////////////////////////////////////////////////////////////////////////////
//	Created		: 01.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_APPLY_DISTORTION_H_INCLUDED
#define EFFECT_APPLY_DISTORTION_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_apply_distortion: public effect_descriptor
{
public:
	effect_apply_distortion() {}
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_gather_bloom

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_APPLY_DISTORTION_H_INCLUDED