////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RENDER_DEVICE_H_INCLUDED
#define RENDER_DEVICE_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/core/device_caps.h>
#include <xray/render/core/res_render_output.h>

namespace xray {
namespace render {

class device: public quasi_singleton<device>
{
public:
									device			( );
									~device			( );

			//bool					begin_frame		();
			//void					end_frame		();
			//u32						get_frame		() {return m_frame;}

			// Need to recheck if we really need this in DX10 or DX11.
			//void					test_corrporate_level();

			ID3DDevice*				d3d_device	() const	{ return m_device; }
			ID3DDeviceContext*		d3d_context	() const	{ return m_context; }

			//	Return default back-buffer width and height.
			//	Desired with and height in global options can be different!
// 	inline	u32						get_width	() const;
// 	inline	u32						get_height	() const;
// 	inline	xray::math::uint2		get_size	() const;

//			bool					support		(DXGI_FORMAT fmt, DWORD type);

//	const	device_caps&			get_caps			() const { return m_caps;}
//			size_t					video_memory_size	();

//	inline	DXGI_ADAPTER_DESC const	adapter_desc() const { return m_adapter_desc;}

private:
			void					create_d3d		();	
			void					destroy_d3d		();	
			void					create			();	
			void					destroy			();	

private:
	enum device_state
	{
		ds_ok = 0,
		ds_lost,
		ds_need_reset
	};

private:

	// Not sure if we need this 
	//u32					select_gpu() const;	//	selectGPU
	void					setup_states			() const;
//	void					update_caps				()			{ m_caps.update();}
	device_state			get_device_state		() const;
// 	ID3DRenderTargetView*	get_render_target		() const 	{ return m_render_output.m_base_rt;}
// 	ID3DDepthStencilView*	get_depth_stencil		() const 	{ return m_render_output.m_base_zb;}

private:
	IDXGIAdapter*			m_adapter;	//	pD3D equivalent
	ID3DDevice*				m_device;	//	pDevice	//	render device
//	ID3DDevice1*			m_device1;	//	DX10.1
//	IDXGISwapChain*         m_swap_chain;
	ID3DDeviceContext*		m_context;	//	pDevice	//	render device

	// Render targets
// 	ID3DRenderTargetView*	m_base_rt;		//	base render target view
// 	ID3DDepthStencilView*	m_base_zb;		//	depth/stencil render target view
// 	ref_texture				m_texture_zb;	//	depth/stencil texture

//	DXGI_SWAP_CHAIN_DESC	m_swap_chain_desc;	
	D3D_FEATURE_LEVEL		m_feature_level;
	DXGI_ADAPTER_DESC		m_adapter_desc;

//	ref_render_output		m_render_output;

private:
//	HWND					m_hwnd;
//	device_caps				m_caps;
//	u32						m_frame;
//	device_state			m_device_state;
	bool					m_use_perfhud;
}; // class device

} // namespace render 
} // namespace xray 

#endif // #ifndef RENDER_DEVICE_H_INCLUDED