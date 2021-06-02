////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

//#include "resource.h"
#include "hw_wrapper.h"
#include "resource_manager.h"
//#include "dxRenderDeviceRender.h"

namespace xray {
namespace render {


res_vs::res_vs() : m_vs(0)
#ifdef	USE_DX10
	,signature(0)
#endif	//	USE_DX10
{}

res_vs::~res_vs()
{
	resource_manager::get_ref().delete_vs(this);
#ifdef	USE_DX10
	//_RELEASE(signature);
	//	Now it is release automatically
#endif	//	USE_DX10
	safe_release(m_vs);
}

HRESULT res_vs::create_hw_shader(ID3DXBuffer* shader_code)
{
	ASSERT(shader_code);

	HRESULT hr = hw_wrapper::get_ref().device()->CreateVertexShader	((DWORD*)shader_code->GetBufferPointer(), &m_vs);

	if (SUCCEEDED(hr))	
	{
		m_constants.parse(shader_code, res_const_table::st_vertex_shader);
		//LPD3DXSHADER_CONSTANTTABLE	constants_table	= NULL;
		////LPCVOID data = NULL;
		//hr = D3DXFindShaderComment((DWORD*)shader_code->GetBufferPointer(),
		//	MAKEFOURCC('C','T','A','B'), (LPCVOID*)&constants_table, NULL);

		//if (SUCCEEDED(hr) && constants_table)
		//{
		//	//constants_table = LPD3DXSHADER_CONSTANTTABLE(data);
		//	//m_constants.parse	(pConstants,0x2);
		//} 
		//else
		//{
		//	LOG_ERROR("! D3DXFindShaderComment hr == %08x", (int)hr);
		//	hr = E_FAIL;
		//}
	}
	else
	{
		LOG_ERROR("! CreateVertexShader hr == %08x", (int)hr);
	}

	return hr;
}

res_ps::res_ps() : m_ps(0)
{}

res_ps::~res_ps()
{
	resource_manager::get_ref().delete_ps(this);
	safe_release(m_ps);
}

HRESULT res_ps::create_hw_shader(ID3DXBuffer* shader_code)
{
	ASSERT(shader_code);

	HRESULT hr = hw_wrapper::get_ref().device()->CreatePixelShader((DWORD*)shader_code->GetBufferPointer(), &m_ps);

	if (SUCCEEDED(hr))	
	{
		m_constants.parse(shader_code, res_const_table::st_pixel_shader);
		//LPD3DXSHADER_CONSTANTTABLE	constants_table	= NULL;
		////LPCVOID data = NULL;
		//hr = D3DXFindShaderComment((DWORD*)shader_code->GetBufferPointer(),
		//	MAKEFOURCC('C','T','A','B'), (LPCVOID*)&constants_table, NULL);

		//if (SUCCEEDED(hr) && constants_table)
		//{
		//	//constants_table = LPD3DXSHADER_CONSTANTTABLE(data);
		//	//m_constants.parse	(pConstants,0x2);
		//} 
		//else
		//{
		//	LOG_ERROR("! D3DXFindShaderComment hr == %08x", (int)hr);
		//	hr = E_FAIL;
		//}
	}
	else
	{
		LOG_ERROR("! CreateVertexShader hr == %08x", (int)hr);
	}

	return hr;
}

#ifdef	USE_DX10
res_gs::~res_gs()
{
	safe_release(m_gs);
	//DEV->_DeleteGS(this);
}

res_input_signature::res_input_signature(ID3DBlob* pBlob)
{
	VERIFY(pBlob);
	m_signature = pBlob;
	m_signature->AddRef();
};

res_input_signature::~res_input_signature()
{
	safe_release(m_signature);
	//DEV->_DeleteInputSignature(this);
}
#endif	//	USE_DX10

res_state::res_state(simulator_states& state_code): m_state(0), m_state_code(state_code)
{
}

res_state::~res_state()
{
	safe_release(m_state);
	resource_manager::get_ref().delete_state(this);
}

void res_state::cache_states()
{
#ifdef	USE_DX10
	ASSERT(!"SimulatorStates::record not implemented!");
#else	//	USE_DX10
	ASSERT(!m_state);
	ID3DDevice*	device = hw_wrapper::get_ref().device();
	R_CHK(device->BeginStateBlock());
	for (u32 it=0; it<m_state_code.m_states.size(); it++)
	{
		simulator_states::state& st	= m_state_code.m_states[it];
		switch (st.type)
		{
		case 0:
			R_CHK(device->SetRenderState((D3DRENDERSTATETYPE)st.v1, st.v2));
			break;
		case 2: 
			R_CHK(device->SetSamplerState(st.v1, (D3DSAMPLERSTATETYPE)st.v2,
					((D3DSAMPLERSTATETYPE)st.v2==D3DSAMP_MAGFILTER && st.v3==D3DTEXF_ANISOTROPIC) ? D3DTEXF_LINEAR : st.v3));
			break;
		}
	}
	IDirect3DStateBlock9*	state = 0;
	R_CHK(device->EndStateBlock(&state));
	m_state = state;
#endif	//	USE_DX10
}

res_declaration::res_declaration(D3DVERTEXELEMENT9 const* decl)
{
	R_CHK(hw_wrapper::get_ref().device()->CreateVertexDeclaration(decl, &m_dcl));
	u32 dcl_size = D3DXGetDeclLength(decl)+1;
	m_dcl_code.assign(decl, decl+dcl_size);
}

res_declaration::~res_declaration()
{	
	resource_manager::get_ref().delete_decl(this);
#ifdef	USE_DX10
	map<ID3DBlob*, ID3D10InputLayout*>::iterator it_layout;
	it_layout = m_vs_to_layout.begin();
	for( ; it_layout != m_vs_to_layout.end(); ++it_layout)
	{
		//	Release vertex layout
		safe_release(it_layout->second);
	}
#else	//	USE_DX10
	//	Release vertex layout
	safe_release(m_dcl);
#endif	//	USE_DX10
}

} // namespace render 
} // namespace xray 
