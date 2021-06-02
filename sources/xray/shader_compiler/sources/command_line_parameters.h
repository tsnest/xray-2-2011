////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_LINE_PARAMETERS_H_INCLUDED
#define COMMAND_LINE_PARAMETERS_H_INCLUDED

namespace xray {
namespace shader_compiler {

class shader_compiler_key: public command_line::key
{
public:
	shader_compiler_key			(pcstr full_name, pcstr short_name, pcstr category, pcstr description, pcstr argument_description = "");
	
	static bool is_no_one_set	();
	static void print_parameters();
	
	static shader_compiler_key* first_key;
	static shader_compiler_key* last_key;
	
private:
	shader_compiler_key*		m_next;
};

extern shader_compiler_key g_show_includes;
extern shader_compiler_key g_ext;
extern shader_compiler_key g_show_define_names;
extern shader_compiler_key g_debug_mode;
extern shader_compiler_key g_asm;
extern shader_compiler_key g_full_asm;
extern shader_compiler_key g_instruction_number;
extern shader_compiler_key g_textures;
extern shader_compiler_key g_samplers;
extern shader_compiler_key g_constants;
extern shader_compiler_key g_dx10;
extern shader_compiler_key g_dx11;
extern shader_compiler_key g_recurse;
extern shader_compiler_key g_time;
extern shader_compiler_key g_size;
extern shader_compiler_key g_input;
extern shader_compiler_key g_output;
extern shader_compiler_key g_shader_model;
extern shader_compiler_key g_use_declarated_defines;
extern shader_compiler_key g_show_not_affect_defines;

} // namespace shader_compiler
} // namespace xray

#endif // #ifndef COMMAND_LINE_PARAMETERS_H_INCLUDED