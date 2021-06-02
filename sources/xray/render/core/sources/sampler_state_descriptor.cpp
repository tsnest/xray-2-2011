////////////////////////////////////////////////////////////////////////////
//	Created		: 13.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/custom_config.h>
#include <xray/render/core/sampler_state_descriptor.h>
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

effect_compiler &	sampler_state_descriptor ::end_sampler ()
{
	ASSERT( m_effect_compiler);
	return m_effect_compiler->end_sampler();
}

} // namespace render 
} // namespace xray