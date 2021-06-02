////////////////////////////////////////////////////////////////////////////
//	Created		: 24.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/effect_notexture.h>
#include <xray/render/core/effect_compiler.h>

namespace xray {
namespace render_dx10 {

void effect_notexture::compile(effect_compiler& compiler, const effect_compilation_options& options)
{
	XRAY_UNREFERENCED_PARAMETER	( options );

	shader_defines_list	defines;
	make_defines(defines);

 	compiler.begin_technique(/*SE_R2_SHADOW*/)
 		.begin_pass	("test", "color_fixed", defines)
			.set_fill_mode	( D3D_FILL_WIREFRAME)
 		.end_pass	()
 	.end_technique();
}

} // namespace render
} // namespace xray
