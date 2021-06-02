////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_COPY_IMAGE_H_INCLUDED
#define EFFECT_COPY_IMAGE_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_copy_image: public effect_descriptor
{
public:
	enum
	{
		copy_rewrite  = 0,
		copy_additive,
	};
	effect_copy_image() {}
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_copy_image

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_COPY_IMAGE_H_INCLUDED