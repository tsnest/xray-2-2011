////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_PARTICLE_SELECTION_H_INCLUDED
#define EFFECT_PARTICLE_SELECTION_H_INCLUDED

#include "effect_material_base.h"

namespace xray {
namespace render {

class effect_particle_selection: public effect_material_base
{
public:
	enum
	{
		billboard		= 0,
		billboard_subuv,
		beamtrail,
	};
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config);
}; // class effect_particle_selection

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_PARTICLE_SELECTION_H_INCLUDED