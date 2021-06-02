////////////////////////////////////////////////////////////////////////////
//	Created		: 04.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef HW_WRAPPER_BASE_DX9_H_INCLUDED
#define HW_WRAPPER_BASE_DX9_H_INCLUDED

namespace xray {
namespace render {


class hw_wrapper_base : boost::noncopyable
{
public:
	hw_wrapper_base();

protected:

	HINSTANCE 				m_hd3d;		//	hD3D

	IDirect3D9* 			m_d3d;		//	pD3D	//	D3D
	ID3DDevice*				m_device;	//	pDevice	//	render device

	ID3DRenderTargetView*	m_base_rt;	//	pBaseRT
	ID3DDepthStencilView*	m_base_zb;	//	pBaseZB

	UINT					m_dev_adapter;	//	DevAdapter
	D3DDEVTYPE				m_dev_type;		//	DevT
	D3DPRESENT_PARAMETERS	m_dev_pparams;	//	DevPP
}; // class hw_wrapper_base_dx9

inline hw_wrapper_base::hw_wrapper_base() :
	m_hd3d(0),
	m_d3d(0),
	m_device(0),
	m_base_rt(0),
	m_base_zb(0),
	m_dev_adapter(0),
	m_dev_type(D3DDEVTYPE_FORCE_DWORD)
{
	memory::zero(&m_dev_pparams, sizeof(m_dev_pparams));
}

} // namespace render 
} // namespace xray 


#endif // #ifndef HW_WRAPPER_BASE_DX9_H_INCLUDED