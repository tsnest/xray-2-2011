////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_GATHER_BLOOM_H_INCLUDED
#define EFFECT_GATHER_BLOOM_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_gather_bloom: public effect_descriptor
{
public:
	effect_gather_bloom() {}
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_gather_bloom

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_GATHER_BLOOM_H_INCLUDED