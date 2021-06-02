////////////////////////////////////////////////////////////////////////////
//	Created		: 18.10.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_SHADER_DECLARED_MACROS_H_INCLUDED
#define XRAY_RENDER_CORE_SHADER_DECLARED_MACROS_H_INCLUDED

namespace xray {
namespace render {

#define shader_declarated_macros_max_cout 128

typedef fixed_vector< fs_new::virtual_path_string, shader_declarated_macros_max_cout> shader_declarated_macroses_list;

inline bool cut_defines(pcstr text, pcstr start, pcstr& out_end)
{
	if (!start || !*start || !text || !*text)
		return false;
	
	pcstr defines_start = start;
	out_end				= start;
	
	bool is_block_comment = false,
		is_comment		  = false;
	
	while (identity(true))
	{
		if (*--defines_start=='*' && *--defines_start=='/')
		{
			is_block_comment = true;
			is_comment = true;
			break;
		}
		else if (*defines_start=='/' && *--defines_start=='/')
		{
			is_comment = true;
			break;
		}
		if (!*defines_start)
			return false;
	}
	
	if (!is_comment)
		return false;
	
	if (is_block_comment)
	{
		while (identity(true))
		{
			if (*++out_end=='*' && *++out_end=='/')
			{
				--out_end;
				break;
			}
			if (!*out_end)
				return false;
		}
	}
	else
		while (identity(true))
			if (!*++out_end || *++out_end=='\n')
			{
				--out_end;
				break;
			}
	
	return true;
}

inline bool found_shader_declarated_macroses(pcstr shader_source_code, shader_declarated_macroses_list& out_found_macros_names)
{
	pcstr defines_key	= "$DEFINES$:",
		  defines_pos	= strstr(shader_source_code, defines_key),
		  defines_end   = 0;
	
#if !XRAY_PLATFORM_PS3
#pragma message (XRAY_TODO("Lain 2 IronNick: no strtok_s on PS3, please fix"))

	if (defines_pos && cut_defines(shader_source_code, defines_pos, defines_end) && defines_end)
	{
		defines_pos += strlen(defines_key);
		
		u32 size = static_cast_checked<u32>(defines_end - defines_pos) + 1;
		pstr defines_buffer = (pstr)ALLOCA(size);
		xray::memory::copy(defines_buffer, size, defines_pos, size);
		defines_buffer[size - 1] = 0;
		
		char seps[]		=  " ,\t\n\r\n";
		char* context	= NULL;
		char *token		= strtok_s(defines_buffer, seps, &context);
		
		while(token!=NULL)
		{
			if (xray::strings::compare(token,"")!=0)
				out_found_macros_names.push_back(token);
			
			token = strtok_s(NULL, seps, &context);
		}
		return out_found_macros_names.size()!=0;
	}
	else
		return false;

#endif // #if !XRAY_PLATFORM_PS3
}
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_SHADER_DECLARED_MACROS_H_INCLUDED
