////////////////////////////////////////////////////////////////////////////
//	Created		: 27.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_EFFECT_DECAL_MASK_H_INCLUDED
#define XRAY_RENDER_EFFECT_DECAL_MASK_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_decal_mask: public effect_descriptor
{
public:
	enum
	{
		specific_type,
		all_types
	};
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_decal_mask

} // namespace xray
} // namespace render

#endif // #ifndef XRAY_RENDER_EFFECT_DECAL_MASK_H_INCLUDED