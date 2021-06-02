////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "blender_forward_system.h"


namespace xray {
namespace render{

blender_forward_system::blender_forward_system() 
{
}

void blender_forward_system::compile(blender_compiler& compiler, const blender_compilation_options& options)
{
		XRAY_UNREFERENCED_PARAMETER	( options );

		shader_defines_list	defines;
		make_defines(defines);

		compiler.begin_technique(/*LEVEL*/);
		compiler.begin_pass("test", "test", defines )
				.set_z_test(TRUE)
				.set_alpha_blend(FALSE)
				.def_sampler("s_base", options.tex_list[0])
			.end_pass()
		.end_technique();

		return;
}

void blender_forward_system::load(memory::reader& mem_reader)
{
	blender::load(mem_reader);
}

} // namespace render
} // namespace xray
