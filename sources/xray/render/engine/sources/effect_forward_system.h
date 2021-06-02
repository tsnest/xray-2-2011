////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_FORWARD_SYSTEM_H_INCLUDED
#define BLENDER_FORWARD_SYSTEM_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_forward_system : public effect_descriptor
{
public:
	virtual void compile( effect_compiler& compiler, custom_config_value const& config);

private:
}; // class effect_forward_system

} // namespace render
} // namespace xray

#endif // #ifndef BLENDER_FORWARD_SYSTEM_H_INCLUDED