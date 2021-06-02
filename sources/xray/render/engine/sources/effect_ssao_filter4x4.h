////////////////////////////////////////////////////////////////////////////
//	Created		: 11.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_SSAO_FILTER4X4_H_INCLUDED
#define EFFECT_SSAO_FILTER4X4_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_ssao_filter4x4: public effect_descriptor
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_ssao_filter4x4

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_SSAO_FILTER4X4_H_INCLUDED