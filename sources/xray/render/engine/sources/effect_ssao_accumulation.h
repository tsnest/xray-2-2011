////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_SSAO_ACCUMULATION_H_INCLUDED
#define EFFECT_SSAO_ACCUMULATION_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_ssao_accumulation: public effect_descriptor
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_ssao_accumulation

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_SSAO_ACCUMULATION_H_INCLUDED