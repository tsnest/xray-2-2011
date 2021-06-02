////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_ATOMIC_H_INCLUDED
#	define RES_ATOMIC_H_INCLUDED

namespace xray {
namespace render {

//#include "tss_def.h"

//#	ifdef	USE_DX10
//#		include "../xrRenderDX10/StateManager/dx10State.h"
//#	endif	//	USE_DX10

//#pragma pack(push,4)

#	ifdef	USE_DX10
class res_input_signature :
	public res_flagged,
	public boost::noncopyable
{
public:
	res_input_signature(ID3DBlob* pBlob);
	~res_input_signature();

private:
	ID3DBlob*			m_signature;	//signature;
};
typedef xray::intrusive_ptr<res_input_signature, res_base>	ref_input_sign;
#	endif	//	USE_DX10

class res_vs : 
	public res_named,
	public res_id,
	public boost::noncopyable
{
public:
	res_vs();
	~res_vs();

	HRESULT create_hw_shader(ID3DXBuffer* shader_code);
	
	void apply() {backend::get_ref().set_vs(m_vs);}
	void merge_ctable_in(res_const_table& dest) {dest.merge(m_constants);}

private:
	ID3DVertexShader*	m_vs;			//vs;
	res_const_table		m_constants;	//constants;

#	ifdef	USE_DX10
	ref_input_sign		m_signature;	//signature;
#	endif	//	USE_DX10
};
typedef xray::intrusive_ptr<res_vs, res_base, threading::single_threading_policy>	ref_vs;

class res_ps :
	public res_named,
	public res_id,
	public boost::noncopyable
{
public:
	res_ps();
	~res_ps();

	HRESULT create_hw_shader(ID3DXBuffer* shader_code);

	void apply() {backend::get_ref().set_ps(m_ps);}
	void merge_ctable_in(res_const_table& dest) {dest.merge(m_constants);}

private:
	ID3DPixelShader*	m_ps;			//ps;
	res_const_table		m_constants;	//constants;
};
typedef xray::intrusive_ptr<res_ps, res_base, threading::single_threading_policy>	ref_ps;

#	ifdef	USE_DX10
class res_gs :
	public res_named,
	public res_id,
	public boost::noncopyable
{
public:
	~sgs();

private:
	ID3DGeometryShader*	m_gs;			//gs;
	res_const_table	m_constants;	//constants;
};
typedef xray::intrusive_ptr<res_gs, res_base>	ref_gs;
#	endif	//	USE_DX10

class res_state :
	public res_flagged,
	public res_id,
	public boost::noncopyable
{
public:
	res_state(simulator_states& state_code);
	~res_state();

public:
	void cache_states();
	void apply() {backend::get_ref().set_state(m_state);}

public:
	ID3DState*			m_state;		//state;
	simulator_states	m_state_code;	//state_code;
};
typedef xray::intrusive_ptr<res_state, res_base, threading::single_threading_policy>	ref_state;

//////////////////////////////////////////////////////////////////////////
class res_declaration :
	public res_flagged,
	public boost::noncopyable
{
public:
	res_declaration(D3DVERTEXELEMENT9 const* decl);
	~res_declaration	();
	
	bool equal(D3DVERTEXELEMENT9 const* other) const
	{
		u32 size = D3DXGetDeclLength(other);
		
		if (size != m_dcl_code.size())
			return false;

		return memcmp(&m_dcl_code[0], other, size*sizeof(D3DVERTEXELEMENT9)) == 0;
	}

	u32 get_stride(u32 stream) {return D3DXGetDeclVertexSize(&*m_dcl_code.begin(), stream);}
	
	void apply() {backend::get_ref().set_decl(m_dcl);}

private:
#	ifdef	USE_DX10
	map<ID3DBlob*, ID3D10InputLayout*>	m_vs_to_layout;		//vs_to_layout;	//Maps input signature to input layout
	render::vector<D3D10_INPUT_ELEMENT_DESC>	m_dx10_dcl_code;	//dx10_dcl_code;
#	else	//	USE_DX10
	//	Don't need it: use ID3D10InputLayout instead which is per ( declaration, VS input layout) pair
	IDirect3DVertexDeclaration9*	m_dcl;				//dcl;
#	endif	//	USE_DX10
	
	//	Use this for DirectX10 to cache DX9 declaration for comparison purpose only
	render::vector<D3DVERTEXELEMENT9>		m_dcl_code;				//dcl_code;
};
typedef xray::intrusive_ptr<res_declaration, res_base, threading::single_threading_policy>	ref_declaration;

//#pragma pack(pop)

} // namespace render 
} // namespace xray 


#endif // #ifndef RES_ATOMIC_H_INCLUDED
