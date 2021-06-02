////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_GATHER_LUMINANCE_HISTOGRAM_H_INCLUDED
#define EFFECT_GATHER_LUMINANCE_HISTOGRAM_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_gather_luminance_histogram: public effect_descriptor
{
public:
	enum
	{
		gather_luminance_in_range,
		gather_luminance_count,
		gather_luminance_histogram,
	};
	effect_gather_luminance_histogram() {}
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_gather_luminance_histogram

} // namespace render 
} // namespace xray 

#endif // #ifndef EFFECT_GATHER_LUMINANCE_HISTOGRAM_H_INCLUDED