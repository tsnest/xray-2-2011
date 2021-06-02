////////////////////////////////////////////////////////////////////////////
//	Created		: 11.05.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_eye_adaptation.h"
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"

namespace xray {
namespace render {

void effect_eye_adaptation::compile( effect_compiler& compiler, const custom_config_value& custom_config)
{
	XRAY_UNREFERENCED_PARAMETERS(custom_config);
	
	compiler.begin_technique();
		compiler.begin_pass("eye_adaptation", NULL, "eye_adaptation", shader_configuration(), NULL);
			compiler.set_depth( false, false);
		compiler.end_pass();
	compiler.end_technique();
}

} // namespace render 
} // namespace xray 
