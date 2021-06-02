////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_line_parameters.h"

namespace xray {
namespace shader_compiler {


shader_compiler_key* shader_compiler_key::first_key = 0;
shader_compiler_key* shader_compiler_key::last_key  = 0;

shader_compiler_key::shader_compiler_key(pcstr full_name, pcstr short_name, pcstr category, pcstr description, pcstr argument_description):
	command_line::key(full_name, short_name, category, description, argument_description)
{
	if (!first_key)
	{
		first_key    = this;
		last_key	 = this;
	}
	else
	{
		last_key->m_next = this;
		last_key		 = this;
	}
}

bool shader_compiler_key::is_no_one_set()
{
	for (shader_compiler_key* key = shader_compiler_key::first_key; key!=0; key = key->m_next)
		if (key->is_set())
			return false;
	return true;
}

void shader_compiler_key::print_parameters()
{
	u32 max_name_length = 0;
	for (shader_compiler_key* key = shader_compiler_key::first_key; key!=0; key = key->m_next)
	{
		u32 current_length = strlen(key->full_name());
		if (current_length>max_name_length)
			max_name_length = current_length;
	}
	
	if (!max_name_length)
		return;
	
	for (shader_compiler_key* key = shader_compiler_key::first_key; key!=0; key = key->m_next)
	{
		xray::fixed_string<128> spaces_line;
		for (u32 i=0; i<max_name_length - strlen(key->full_name()); i++)
			spaces_line.append(" ");
		
		printf("  -%s%s - %s\n", key->full_name(), spaces_line.c_str(), key->description());
	}
}

command_line::key   g_help				("help",				"", "", "show all command line parameters");
shader_compiler_key g_show_includes		("show_includes",		"", "", "show includes");
shader_compiler_key g_ext				("ext",					"", "", "file extensions that will compile");
shader_compiler_key g_show_define_names	("show_define_names",	"", "", "show all define names");
shader_compiler_key g_debug_mode		("debug_mode",			"", "", "enable debug compiler mode");
shader_compiler_key g_asm				("asm",					"", "", "show cleared asm shader code");
shader_compiler_key g_full_asm			("full_asm",			"", "", "show full asm shader code");
shader_compiler_key g_instruction_number("instruction_number",	"", "", "show number of intruction in compiled shader");
shader_compiler_key g_textures			("textures",			"", "", "show number of used textures");
shader_compiler_key g_samplers			("samplers",			"", "", "show number of used samplers");
shader_compiler_key g_constants			("constants",			"", "", "show number of used constants");
shader_compiler_key g_dx10				("dx10",				"", "", "directx 10 mode");
shader_compiler_key g_dx11				("dx11",				"", "", "directx 11 mode");
shader_compiler_key g_recurse			("recurse",				"", "", "recursive scaning of compiled directory");
shader_compiler_key g_time				("time",				"", "", "show duration of compilation");
shader_compiler_key g_size				("size",				"", "", "show size of compiled shader");
shader_compiler_key g_input				("input",				"", "", "show shader inputs");
shader_compiler_key g_output			("output",				"", "", "show shader outputs");
shader_compiler_key g_shader_model		("shader_model",		"", "", "set shader model");

shader_compiler_key g_use_declarated_defines ("use_declarated_defines",		"", "", "use declarated defines");
shader_compiler_key g_show_not_affect_defines("show_not_affect_defines",	"", "", "show not affect defines");


} // namespace shader_compiler
} // namespace xray