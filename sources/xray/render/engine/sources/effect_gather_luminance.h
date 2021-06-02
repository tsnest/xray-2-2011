////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_GATHER_LUMINANCE_H_INCLUDED
#define EFFECT_GATHER_LUMINANCE_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_gather_luminance: public effect_descriptor
{
public:
	enum 
	{
		gather_log_luminance,
		gather_luminance,
		gather_exp_luminance,
	};
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_gather_luminance

} // namespace render 
} // namespace xray 

#endif // #ifndef EFFECT_GATHER_LUMINANCE_H_INCLUDED