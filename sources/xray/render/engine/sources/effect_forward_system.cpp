////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2010
//	Author		: Armen Abroyan
//	Copyright ( C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_forward_system.h"
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render {

void effect_forward_system::compile( effect_compiler& compiler, custom_config_value const& config)
{
	compiler.begin_technique();
		compiler.begin_pass("test", NULL, "test", shader_configuration(), NULL);
	 		compiler.set_depth(true, true);
	 		compiler.set_alpha_blend( false);
	 		compiler.set_texture("t_base", pcstr(config["texture0"]));
		compiler.end_pass();
	 compiler.end_technique();
}

} // namespace render
} // namespace xray
