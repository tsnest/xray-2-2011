////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_BLUR_H_INCLUDED
#define EFFECT_BLUR_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_blur: public effect_descriptor
{
public:
	enum
	{
		horizontally = 0,
		vertically,
	};
	effect_blur() {}
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_blur

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_BLUR_H_INCLUDED