////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_EFFECT_APPLY_INDIRECT_LIGHTING_H_INCLUDED
#define XRAY_RENDER_ENGINE_EFFECT_APPLY_INDIRECT_LIGHTING_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_apply_indirect_lighting: public effect_descriptor
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_apply_indirect_lighting

} // namespace xray
} // namespace render

#endif // #ifndef XRAY_RENDER_ENGINE_EFFECT_APPLY_INDIRECT_LIGHTING_H_INCLUDED