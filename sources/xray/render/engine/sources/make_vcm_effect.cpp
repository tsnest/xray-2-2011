////////////////////////////////////////////////////////////////////////////
//	Created		: 05.04.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "make_vcm_effect.h"
#include <xray/render/core/effect_compiler.h>

void xray::render::make_vcm_effect::compile	( effect_compiler& compiler, custom_config_value const& config )
{
	XRAY_UNREFERENCED_PARAMETER	( config);
	compiler.begin_technique();
		compiler.begin_pass("make_vcm", "make_vcm");
			compiler.set_depth(true, false);
			compiler.set_cull_mode(D3D_CULL_NONE);
		compiler.end_pass();
	compiler.end_technique();
}