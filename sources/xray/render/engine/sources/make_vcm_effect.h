////////////////////////////////////////////////////////////////////////////
//	Created		: 05.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MAKE_VCM_EFFECT_H_INCLUDED
#define MAKE_VCM_EFFECT_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class make_vcm_effect : public effect_descriptor {
private:
	virtual	void compile	( effect_compiler& compiler, custom_config_value const& config );
}; // class make_vcm_effect

} // namespace render
} // namespace xray

#endif // #ifndef MAKE_VCM_EFFECT_H_INCLUDED