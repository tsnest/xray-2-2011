////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_RES_XS_HW_IMPL_H_INCLUDED
#define XRAY_RENDER_CORE_RES_XS_HW_IMPL_H_INCLUDED

#include <d3dcompiler.h>
#include "com_utils.h"
#include <xray/render/core/resource_manager.h>

namespace xray {
namespace render {

template<typename shader_data>
res_xs_hw<shader_data>::res_xs_hw() :
	m_is_registered	( false )
{

}

template<typename shader_data>
res_xs_hw<shader_data>::~res_xs_hw()
{
	safe_release( m_shader_data.hardware_shader);
}

template<typename shader_data>
void res_xs_hw<shader_data>::destroy_impl	() const
{
	resource_manager::ref().release( this );
}

template<typename shader_data>
HRESULT res_xs_hw<shader_data>::create_hw_shader( ID3DBlob* shader_code, typename shader_data::hw_interface **hardware_shader)
{
	XRAY_UNREFERENCED_PARAMETERS( shader_code, hardware_shader);
	NODEFAULT;
	return 0;
}

// template<typename shader_data>
// void res_xs_hw<shader_data>::apply()
// {
// 	NODEFAULT;
// }

template < typename shader_data >
HRESULT res_xs_hw<shader_data>::create_hw_shader( ID3DBlob* shader_code)
{
	ASSERT( shader_code);

	void const* buffer	= shader_code->GetBufferPointer();
	u32 buffer_size		= (u32)shader_code->GetBufferSize();

	HRESULT hr = create_hw_shader( shader_code, &m_shader_data.hardware_shader);

	if ( SUCCEEDED( hr))	
	{
		ID3DShaderReflection* shader_reflection;
#if USE_DX10
		hr			= D3D10ReflectShader( buffer, buffer_size, &shader_reflection);
#else
		hr			= D3DReflect( buffer, buffer_size, IID_ID3D11ShaderReflection, (void**)&shader_reflection);
#endif
		
		D3D_SHADER_DESC shader_desc;
		shader_reflection->GetDesc(&shader_desc);
		
		m_shader_data.instruction_count = static_cast<u16>( shader_desc.InstructionCount );
		
		m_shader_data.constants.parse	( shader_reflection, (enum_shader_type)shader_data::type);
		parse_resources		( shader_reflection, m_shader_data.samplers, m_shader_data.textures);
		
		safe_release( shader_reflection);
		//LPD3DXSHADER_CONSTANTTABLE	constants_table	= NULL;
		////LPCVOID data = NULL;
		//hr = D3DXFindShaderComment( ( DWORD*)shader_code->GetBufferPointer(),
		//	MAKEFOURCC( 'C','T','A','B'), ( LPCVOID*)&constants_table, NULL);

		//if ( SUCCEEDED( hr) && constants_table)
		//{
		//	//constants_table = LPD3DXSHADER_CONSTANTTABLE( data);
		//	//m_constants.parse	( pConstants,0x2);
		//} 
		//else
		//{
		//	LOG_ERROR( "! D3DXFindShaderComment hr == %08x", ( int)hr);
		//	hr = E_FAIL;
		//}
	}
	else
	{
		LOG_ERROR( "! CreateVertexShader hr == %08x", ( int)hr);
	}

	return hr;
}

template<typename shader_data>
void res_xs_hw<shader_data>::parse_resources	( ID3DShaderReflection* shader_reflection, 
												 sampler_slots &samplers, 
												 texture_slots &textures)
{
// 	ZeroMemory( samplers, sizeof(samplers));
// 	ZeroMemory( textures, sizeof(textures));

	D3D_SHADER_DESC	shader_desc;
	shader_reflection->GetDesc( &shader_desc);

	for( u32 i=0; i<shader_desc.BoundResources; ++i)
	{
		D3D_SHADER_INPUT_BIND_DESC	ResDesc;
		shader_reflection->GetResourceBindingDesc(i, &ResDesc);

		ASSERT(ResDesc.BindCount==1);

		if( ResDesc.Type == D3D10_SIT_TEXTURE)
		{
			texture_slot tex_slot;
			tex_slot.name		= ResDesc.Name;
			tex_slot.slot_id	= ResDesc.BindPoint; // ??? may be removed

			textures.resize( math::max( ResDesc.BindPoint+1, textures.size()));
			ASSERT( textures[ResDesc.BindPoint].name.length() == 0);

			textures[ResDesc.BindPoint] = tex_slot;
		}
		else if( ResDesc.Type == D3D10_SIT_SAMPLER)
		{
			sampler_slot smp;
			smp.name		= ResDesc.Name;
			smp.slot_id		= ResDesc.BindPoint; // ??? may be removed
			smp.state		= resource_manager::ref().find_registered_sampler( ResDesc.Name);

			//ASSERT( smp.state, "The sampler_slot hasn't been registared!");

			samplers.resize( math::max( ResDesc.BindPoint+1, samplers.size()));
			ASSERT( samplers[ResDesc.BindPoint].name.length() == 0);

			samplers[ResDesc.BindPoint] = smp;
		}
		else if( ResDesc.Type == D3D11_SIT_UAV_RWTYPED)
		{
			NOT_IMPLEMENTED();
		}
		else
		{
			continue;
		}
	}
	return;
}


} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_RES_XS_HW_IMPL_H_INCLUDED