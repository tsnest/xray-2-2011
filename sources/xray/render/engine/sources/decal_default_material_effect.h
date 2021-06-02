////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_DECAL_DEFAULT_MATERIAL_EFFECT_H_INCLUDED
#define XRAY_RENDER_DECAL_DEFAULT_MATERIAL_EFFECT_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class decal_default_material_effect: public effect_material_base
{
public:
	decal_default_material_effect(bool is_forward): m_is_forward(is_forward) {}
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
private:
	bool m_is_forward;
}; // class decal_default_material_effect

} // namespace render 
} // namespace xray 


#endif // #ifndef XRAY_RENDER_DECAL_DEFAULT_MATERIAL_EFFECT_H_INCLUDED