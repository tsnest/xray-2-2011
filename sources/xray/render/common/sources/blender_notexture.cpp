////////////////////////////////////////////////////////////////////////////
//	Created		: 24.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_notexture.h"

namespace xray {
namespace render{

void blender_notexture::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
	XRAY_UNREFERENCED_PARAMETER	( options );

	shader_defines_list	defines;
	make_defines(defines);

 	compiler.begin_technique(/*SE_R2_SHADOW*/)
 		.begin_pass("test", "color", defines)
 		.end_pass()
 	.end_technique();
 }

} // namespace render
} // namespace xray
