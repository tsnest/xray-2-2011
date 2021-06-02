////////////////////////////////////////////////////////////////////////////
//	Created		: 11.01.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_EDITOR_GBUFFER_TO_SCREEN_H_INCLUDED
#define EFFECT_EDITOR_GBUFFER_TO_SCREEN_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class effect_editor_gbuffer_to_screen: public effect_descriptor
{
public:
	virtual	void compile(effect_compiler& compiler, custom_config_value const& config);
}; // class effect_editor_gbuffer_to_screen

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_EDITOR_GBUFFER_TO_SCREEN_H_INCLUDED