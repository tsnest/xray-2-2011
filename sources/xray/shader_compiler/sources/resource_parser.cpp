////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_parser.h"

namespace xray {
namespace shader_compiler {



std::string resource_parser::parse_dx9_10_instructions_str( const std::string& asm_code )
{
	std::string left("pproximately ");
	std::string rigth(" instruction");

	s32 left_pos = asm_code.find(left.c_str());
	s32 right_pos = asm_code.find(rigth.c_str());

	std::string result;
	if (left_pos!=-1 && right_pos!=-1)
		for (s32 i=left_pos+left.length(); i<right_pos; i++)
			result+=asm_code[i];
	return result;
}

void resource_parser::process_dx9( const void* shader_buffer, const void* constants_buffer )
{
	(void)&shader_buffer;
	(void)&constants_buffer;

#if 0
	D3DXSHADER_CONSTANTTABLE* desc	= (D3DXSHADER_CONSTANTTABLE*) constants_buffer;
	D3DXSHADER_CONSTANTINFO* it		= (D3DXSHADER_CONSTANTINFO*) (LPBYTE(desc)+desc->ConstantInfo);
	LPBYTE					 ptr	= LPBYTE(desc);
	for ( u32 dwCount = desc->Constants; dwCount; dwCount--,it++)
	{
		// Name
		LPCSTR	name		=	LPCSTR(ptr+it->Name);

		// TypeInfo + class
		D3DXSHADER_TYPEINFO*	T	= (D3DXSHADER_TYPEINFO*)(ptr+it->TypeInfo);

		string_path constant_str;
		string_path tmp_str;

		value_name_pair tmp_pair;
		tmp_pair.value = T->Type;
		value_name_pair* res = std::find_if(&parameter_type_names_dx9[0], &parameter_type_names_dx9[sizeof(parameter_type_names_dx9)], value_name_predicate( T->Type));
		if( res == &parameter_type_names_dx9[sizeof(parameter_type_names_dx9)])
			continue;

		strcpy_s( constant_str, res->name );

		if( T->Rows > 1)
		{
			_itoa_s( T->Rows, tmp_str, sizeof(tmp_str), 10);
			strcat_s( constant_str, tmp_str);
		}

		if( T->Columns > 1)
		{
			if( T->Rows > 1)
				strcat_s( constant_str, "x");

			_itoa_s( T->Columns, tmp_str, sizeof(tmp_str), 10);
			strcat_s( constant_str, tmp_str);
		}

		strcat_s( constant_str, "\t");
		strcat_s( constant_str, name);

		if( T->Elements > 1)
		{
			strcat_s( constant_str, "[");
			_itoa_s( T->Elements, tmp_str, sizeof(tmp_str), 10);
			strcat_s( constant_str, tmp_str);
			strcat_s( constant_str, "]");
		}
		strcat_s( constant_str, ";");

	}

	if (!shader_buffer)
		return;

	bool show_asm = g_asm.is_set();
	bool show_number_of_instructions = g_instruction_number.is_set();
	bool show_number_of_textures = false;

	if (show_asm || show_number_of_instructions || show_number_of_textures)
	{
		LPD3DXBUFFER pDisassembly = 0;
		D3DXDisassembleShader((const DWORD*)shader_buffer,0,0,&pDisassembly);
		if (pDisassembly)
		{
			if (show_asm)
				printf("\n%s",pDisassembly->GetBufferPointer());

			//...
			//Approximately XXX instruction
			//used (XXX texture
			//...

			std::string asm_text;
			asm_text.assign((pstr)pDisassembly->GetBufferPointer(),(pstr)pDisassembly->GetBufferPointer()+pDisassembly->GetBufferSize());

			//TODO: make shorter
			//if (show_number_of_instructions)
			//{
			//	printf("\n\t%s instructions used\n",parse_dx9_10_instructions_str(asm_text).c_str());
			//}

			if (show_number_of_textures)
			{
				std::string left("used (");
				std::string rigth(" texture");

				s32 left_pos = asm_text.find(left.c_str());
				s32 right_pos = asm_text.find(rigth.c_str());

				std::string str_value;
				if (left_pos!=-1 && right_pos!=-1)
					for (s32 i=left_pos+left.length(); i<right_pos; i++)
						str_value+=asm_text[i];

				printf("\n\t%s textures used\n",str_value.c_str());
			}
		}
	}
	//return desc->Constants;
#endif // #if 0

}

} // namespace shader_compiler
} // namespace xray