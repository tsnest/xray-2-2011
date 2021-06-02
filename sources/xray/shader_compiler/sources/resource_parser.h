////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_PARSER_H_INCLUDED
#define RESOURCE_PARSER_H_INCLUDED

#include "shader_compiler.h"

namespace xray {
namespace shader_compiler {

/////////////////////////////////////////////////////////////////////////////
struct value_name_pair 
{
	s32 value;
	char* name;
}; // struct value_name_pair

/////////////////////////////////////////////////////////////////////////////
struct value_name_predicate
{
	value_name_predicate( s32 value ): 
		m_value (value)
	{}

	bool operator () ( value_name_pair const & other )
	{
		return other.value == m_value;
	}

	s32 m_value;
}; // struct value_name_predicate

/////////////////////////////////////////////////////////////////////////////
struct dx10_variable_desc
{
	friend bool operator < ( dx10_variable_desc A, dx10_variable_desc B)
	{
		return ( A.StartOffset < B.StartOffset || A.Offset < B.Offset);
	}

	// variable info
	LPCSTR                  Name;           // Name of the variable
	UINT                    StartOffset;    // Offset in constant buffer's backing store
	UINT                    Size;           // Size of variable (in bytes)
	UINT                    uFlags;         // Variable flags
	LPVOID                  DefaultValue;   // Raw pointer to default value

	// type info
	D3D10_SHADER_VARIABLE_CLASS Class;          // Variable class (e.g. object, matrix, etc.)
	D3D10_SHADER_VARIABLE_TYPE  Type;           // Variable type (e.g. float, sampler, etc.)
	UINT                        Rows;           // Number of rows (for matrices, 1 for other numeric, 0 if not applicable)
	UINT                        Columns;        // Number of columns (for vectors & matrices, 1 for other numeric, 0 if not applicable)
	UINT                        Elements;       // Number of elements (0 if not an array)
	UINT                        Members;        // Number of members (0 if not a structure)
	UINT                        Offset;         // Offset from the start of structure (0 if not a structure member) 
}; // struct dx10_variable

/////////////////////////////////////////////////////////////////////////////
static value_name_pair  parameter_type_names_dx9[]	= {
	{ D3DXPT_VOID,				"void" } ,				
	{ D3DXPT_BOOL,				"bool" } ,				
	{ D3DXPT_INT,				"int" }	,				
	{ D3DXPT_FLOAT,				"float" }	,			
	{ D3DXPT_STRING,			"string" } ,			
	{ D3DXPT_TEXTURE,			"texture" } ,				
	{ D3DXPT_TEXTURE1D,			"texture1d" } ,			
	{ D3DXPT_TEXTURE2D,			"texture2d" } ,			
	{ D3DXPT_TEXTURE3D,			"texture3d" } ,			
	{ D3DXPT_TEXTURECUBE,		"texturecube" },			
	{ D3DXPT_SAMPLER,			"sampler"	} ,		
	{ D3DXPT_SAMPLER1D,			"sampler1D" } ,			
	{ D3DXPT_SAMPLER2D,			"sampler2D" } ,			
	{ D3DXPT_SAMPLER3D,			"sampler3D" } ,			
	{ D3DXPT_SAMPLERCUBE,		"samplercube" } ,			
	{ D3DXPT_PIXELSHADER,		"pixelshader" } ,			
	{ D3DXPT_VERTEXSHADER,		"vertexshader" } ,		
	{ D3DXPT_PIXELFRAGMENT,		"pixelfragment" } ,		
	{ D3DXPT_VERTEXFRAGMENT,	"vertexfragment" } ,	
	{ D3DXPT_UNSUPPORTED,		"unsupported" }		,	
};

/////////////////////////////////////////////////////////////////////////////
static value_name_pair  parameter_type_names[]	= {
	{ D3D10_SVT_VOID,						"void" },
	{ D3D10_SVT_BOOL,						"bool" },
	{ D3D10_SVT_INT,						"int" },
	{ D3D10_SVT_FLOAT,						"float" },
	{ D3D10_SVT_STRING,						"string" },
	{ D3D10_SVT_TEXTURE,					"texture" },
	{ D3D10_SVT_TEXTURE1D,					"texture1d" },
	{ D3D10_SVT_TEXTURE2D,					"texture2d" },
	{ D3D10_SVT_TEXTURE3D,					"texture3d" },
	{ D3D10_SVT_TEXTURECUBE,				"texturecube" },
	{ D3D10_SVT_SAMPLER,					"sampler" },
	{ D3D10_SVT_PIXELSHADER,				"pixelshader" },
	{ D3D10_SVT_VERTEXSHADER,				"vertexshader" },
	{ D3D10_SVT_UINT,						"uint" },
	{ D3D10_SVT_UINT8,						"uint8" },
	{ D3D10_SVT_GEOMETRYSHADER,				"geometryshader" },
	{ D3D10_SVT_RASTERIZER,					"rasterizer" },
	{ D3D10_SVT_DEPTHSTENCIL,				"depthstencil" },
	{ D3D10_SVT_BLEND,						"blend" },
	{ D3D10_SVT_BUFFER,						"buffer" },
	{ D3D10_SVT_CBUFFER,					"cbuffer" },
	{ D3D10_SVT_TBUFFER,					"tbuffer" },
	{ D3D10_SVT_TEXTURE1DARRAY,				"texture1darray" },
	{ D3D10_SVT_TEXTURE2DARRAY,				"texture2darray" },
	{ D3D10_SVT_RENDERTARGETVIEW,			"rendertargetview" },
	{ D3D10_SVT_DEPTHSTENCILVIEW,			"depthstencilview" },
	{ D3D10_SVT_TEXTURE2DMS,				"texture2dms" },
	{ D3D10_SVT_TEXTURE2DMSARRAY,			"texture2dmsarray" },
	{ D3D10_SVT_TEXTURECUBEARRAY,			"texturecubemsarray" },
	{ D3D11_SVT_HULLSHADER,					"hullshader" },
	{ D3D11_SVT_DOMAINSHADER,				"domainshader" },
	{ D3D11_SVT_INTERFACE_POINTER,			"interface_pointer" },
	{ D3D11_SVT_COMPUTESHADER,				"computeshader" },
	{ D3D11_SVT_DOUBLE,						"double" },
	{ D3D11_SVT_RWTEXTURE1D,				"rwtexture1d" },
	{ D3D11_SVT_RWTEXTURE1DARRAY,			"rwtexture1darray" },
	{ D3D11_SVT_RWTEXTURE2D,				"rwtexture2d" },
	{ D3D11_SVT_RWTEXTURE2DARRAY,			"rwtexture2darray" },
	{ D3D11_SVT_RWTEXTURE3D,				"rwtexture3d" },
	{ D3D11_SVT_RWBUFFER,					"rwbuffer" },
	{ D3D11_SVT_BYTEADDRESS_BUFFER,			"byteaddress_buffer" },
	{ D3D11_SVT_RWBYTEADDRESS_BUFFER,		"rwbyteaddress_buffer" },
	{ D3D11_SVT_STRUCTURED_BUFFER,			"structured_buffer" },
	{ D3D11_SVT_RWSTRUCTURED_BUFFER,		"rwstructured_buffer" },
	{ D3D11_SVT_APPEND_STRUCTURED_BUFFER,	"append_structured_buffer" },
	{ D3D11_SVT_CONSUME_STRUCTURED_BUFFER,	"consume_structured_buffer" },
};

/////////////////////////////////////////////////////////////////////////////
static value_name_pair  texture_type_names[]	= {
	{ D3D10_SRV_DIMENSION_UNKNOWN,			"unknown_dimension" },
	{ D3D10_SRV_DIMENSION_BUFFER,			"buffer" },
	{ D3D10_SRV_DIMENSION_TEXTURE1D,		"texture1D" },
	{ D3D10_SRV_DIMENSION_TEXTURE1DARRAY,	"texture1Darray" },
	{ D3D10_SRV_DIMENSION_TEXTURE2D,		"texture2D" },
	{ D3D10_SRV_DIMENSION_TEXTURE2DARRAY,	"texture2Darray" },
	{ D3D10_SRV_DIMENSION_TEXTURE2DMS,		"texture2Dms" },
	{ D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY,	"texture2Dmsarray" },
	{ D3D10_SRV_DIMENSION_TEXTURE3D,		"texture3D" },
	{ D3D10_SRV_DIMENSION_TEXTURECUBE,		"textureCube" },
};

/////////////////////////////////////////////////////////////////////////////

static value_name_pair  register_component_type_name[]	= {
	{ D3D10_REGISTER_COMPONENT_UNKNOWN,			"unknown" },
	{ D3D10_REGISTER_COMPONENT_UINT32,			"uint32" },
	{ D3D10_REGISTER_COMPONENT_UINT32,			"int32" },
	{ D3D10_REGISTER_COMPONENT_UINT32,			"float32" },
};


/////////////////////////////////////////////////////////////////////////////
struct resource_parser : private boost::noncopyable 
{
	static void process_dx9 ( const void* shader_buffer, const void* constants_buffer							);

	template<class TYPE, const bool is_dx10> static void process_dx10_dx11( const void* shader_buffer, u32 shader_buffer_size, u32& out_instruction_number	)
	{
		TYPE::ID3DShaderReflection *pReflection = 0;
		HRESULT result;
		
#pragma warning (push)
#pragma warning (disable:4127) // warning C4127: conditional expression is constant
		if ( is_dx10 )
			result	= D3D10ReflectShader( shader_buffer, shader_buffer_size, (ID3D10ShaderReflection**)&pReflection);
		else
			result = D3DReflect( shader_buffer, shader_buffer_size, IID_ID3D11ShaderReflection, (void**)&pReflection);
#pragma warning (pop)

		if (result!=S_OK)
			return;

		TYPE::D3D_SHADER_DESC	ShaderDesc;
		pReflection->GetDesc(&ShaderDesc);

		u32 num_textures = 0;
		u32 num_samplers = 0;

		for( u32 i=0; i<ShaderDesc.BoundResources; ++i)
		{
			TYPE::D3D_SHADER_INPUT_BIND_DESC	ResDesc;
			pReflection->GetResourceBindingDesc(i, &ResDesc);

			if( ResDesc.Type == D3D10_SIT_TEXTURE)
				num_textures++;
			else if( ResDesc.Type == D3D10_SIT_SAMPLER)
				num_samplers++;
		}
		bool show_textures		= g_textures.is_set(),
			 show_samlers		= g_samplers.is_set(),
			 show_constants		= g_constants.is_set(),
			 show_fasm			= g_full_asm.is_set(),
			 show_asm			= g_asm.is_set(),
			 show_input			= g_input.is_set(),
			 show_output		= g_output.is_set();
		
		if ( show_input && ShaderDesc.InputParameters )
		{
			printf("\n\n      INPUT");
			for( u32 i=0; i < ShaderDesc.InputParameters; ++i)
			{
				TYPE::D3D_SIGNATURE_PARAMETER_DESC	input_desc;
				pReflection->GetInputParameterDesc(i, &input_desc);

				printf("\n         %s\t%s%d,\tR:%d;",register_component_type_name[input_desc.ComponentType].name, input_desc.SemanticName, input_desc.SemanticIndex, input_desc.Register);
			}
			printf("\n");
		}

		if ( show_output && ShaderDesc.OutputParameters )
		{
			printf("\n\n      OUTPUT");
			for( u32 i=0; i < ShaderDesc.OutputParameters; ++i)
			{
				TYPE::D3D_SIGNATURE_PARAMETER_DESC	output_desc;
				pReflection->GetOutputParameterDesc(i, &output_desc);

				printf("\n         %s\t%s%d,\tR:%d;",register_component_type_name[output_desc.ComponentType].name, output_desc.SemanticName, output_desc.SemanticIndex, output_desc.Register);
			}
			printf("\n");
		}

		if ( show_output && ShaderDesc.OutputParameters )
		{
		}

		if ( show_textures && num_textures )
		{
			printf("\n\n      TEXTURES");

			for( u32 i=0; i<ShaderDesc.BoundResources; ++i)
			{
				TYPE::D3D_SHADER_INPUT_BIND_DESC	ResDesc;
				pReflection->GetResourceBindingDesc(i, &ResDesc);

				if( ResDesc.Type == D3D10_SIT_TEXTURE)
					printf("\n         %s\t%s;",texture_type_names[ResDesc.Dimension].name, ResDesc.Name);
			}
			printf("\n");
		}
		if ( show_samlers && num_samplers )
		{
			printf("\n\n      SAMPLERS");

			for( u32 i=0; i<ShaderDesc.BoundResources; ++i)
			{
				TYPE::D3D_SHADER_INPUT_BIND_DESC	ResDesc;
				pReflection->GetResourceBindingDesc(i, &ResDesc);

				if( ResDesc.Type == D3D10_SIT_SAMPLER)
					printf("\n         sampler %s;",ResDesc.Name);
			}
			printf("\n");
		}



		if( show_constants && ShaderDesc.ConstantBuffers)
		{
			printf("\n\n      CONSTANTS");
			//	Parse single constant table
			TYPE::ID3DShaderReflectionConstantBuffer *pTable=0;

			for( u32 iBuf = 0; iBuf<ShaderDesc.ConstantBuffers; ++iBuf )
			{
				std::vector<dx10_variable_desc> variables;

				pTable = pReflection->GetConstantBufferByIndex(iBuf);
				if (pTable)
				{
					TYPE::D3D_SHADER_BUFFER_DESC	TableDesc;

					if( FAILED(pTable->GetDesc(&TableDesc)))
						continue;

					printf("\n         cbuffer %s\n         {",TableDesc.Name);


					for ( u32 i = 0; i < TableDesc.Variables; ++i)
					{
						TYPE::ID3DShaderReflectionVariable* pVar;
						TYPE::D3D_SHADER_VARIABLE_DESC		VarDesc;
						TYPE::ID3DShaderReflectionType*		pType;
						TYPE::D3D_SHADER_TYPE_DESC			TypeDesc;

						pVar = pTable->GetVariableByIndex(i);

						pVar->GetDesc(&VarDesc);
						pType = pVar->GetType();

						pType->GetDesc(&TypeDesc);

						dx10_variable_desc var_desc;

						var_desc.Name = VarDesc.Name;
						var_desc.StartOffset = VarDesc.StartOffset;
						var_desc.Size = VarDesc.Size;
						var_desc.uFlags = VarDesc.uFlags;
						var_desc.DefaultValue = VarDesc.DefaultValue;
						var_desc.Class = TypeDesc.Class;
						var_desc.Type = TypeDesc.Type;
						var_desc.Rows = TypeDesc.Rows;
						var_desc.Columns = TypeDesc.Columns;
						var_desc.Elements = TypeDesc.Elements;
						var_desc.Members = TypeDesc.Members;
						var_desc.Offset = TypeDesc.Offset;

						variables.push_back(var_desc);
					}

					// sort?
					for ( vector<dx10_variable_desc>::const_iterator it = variables.begin(); it!=variables.end(); ++it)
					{
						printf("\n            [%4d]: ",it->StartOffset,it->Size);
						switch (it->Class)
						{
						case D3D10_SVC_SCALAR:
							printf("   %s\t%s;",parameter_type_names[it->Type].name,it->Name);
							if ( it->Elements )
								printf("[%d]",it->Elements);
							break;

						case D3D10_SVC_VECTOR:
							if ( it->Columns>1 )
								printf("   %s%d\t%s;",parameter_type_names[it->Type].name,it->Columns,it->Name);
							else
								printf("   %s%s;",parameter_type_names[it->Type].name,it->Name);
							if ( it->Elements )
								printf("[%d]",it->Elements);
							break;

							//TODO: if row major RxC else CxR ?

						case D3D10_SVC_MATRIX_ROWS:
							printf("   float%dx%d\t%s;",it->Rows,it->Columns,it->Name);
							if ( it->Elements )
								printf("[%d]",it->Elements);
							break;

						case D3D10_SVC_MATRIX_COLUMNS:
							printf("   float%dx%d\t%s;",it->Rows,it->Columns,it->Name);
							if ( it->Elements )
								printf("[%d]",it->Elements);
							break;

						case D3D10_SVC_STRUCT:
							printf("   struct\t%s;",it->Name);
							if ( it->Elements )
								printf("[%d]",it->Elements);
							break;
						default:
							printf("UNKNOWN_VARIABLE");
						};
					}
					printf("\n         }");
				}
			}
			printf("\n");
		}

		if ( show_fasm || show_asm )
		{
			ID3DBlob* pDisassembly = 0;
			
#pragma warning (push)
#pragma warning (disable:4127) // warning C4127: conditional expression is constant
			if (is_dx10)
				D3D10DisassembleShader(shader_buffer,shader_buffer_size,0,0,&pDisassembly);
			else
				D3DDisassemble(shader_buffer,shader_buffer_size,0,0,&pDisassembly);
#pragma warning (pop)

			if (pDisassembly)
			{
				std::string asm_text;
				asm_text.assign((pstr)pDisassembly->GetBufferPointer(),(pstr)pDisassembly->GetBufferPointer()+pDisassembly->GetBufferSize());

				while ( show_asm && !show_fasm )
				{
					s32 start_rem_index = asm_text.find("//");
					s32 end_rem_index = asm_text.find("\n",start_rem_index+1);

					if ( start_rem_index!=-1 && end_rem_index!=-1)
						asm_text.erase(start_rem_index,end_rem_index);
					else
						break;
				}
				std::vector<std::string> lines;
				u32 curr_pos = 0;
				for (u32 i=0; i<asm_text.size(); i++)
				{
					if (asm_text[i]=='\n')
					{
						std::string line;
						for (u32 s=curr_pos; s<=i; s++)
							line+=asm_text[s];
						curr_pos = i+1;
						lines.push_back(line);
					}
				}
				if (lines.size())
				{
					printf("\n");
				}
				for (u32 i=0; i<lines.size(); i++)
				{
					printf("      %s",lines[i].c_str());
				}
			}
		}

		out_instruction_number = ShaderDesc.InstructionCount;
	}
	static std::string parse_dx9_10_instructions_str( const std::string& asm_code );
}; // struct resource_parser

} // namespace shader_compiler
} // namespace xray

#endif // #ifndef RESOURCE_PARSER_H_INCLUDED