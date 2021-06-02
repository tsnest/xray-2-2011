////////////////////////////////////////////////////////////////////////////
//	Created		: 17.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_CORE_RENDER_OUTPUT_H_INCLUDED
#define XRAY_RENDER_CORE_RENDER_OUTPUT_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>

namespace xray {
namespace render {

class res_texture;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class res_render_output : public resource_intrusive_base {
	friend class device;
public:
	res_render_output( HWND window, bool m_windowed );
	~res_render_output( );

	void destroy_impl() const;

public:
	void		present			( );
	void		resize			( );
	void		resize			( bool m_windowed);

	xray::math::uint2	size	( ) const { return xray::math::uint2( m_swap_chain_desc.BufferDesc.Width, m_swap_chain_desc.BufferDesc.Height); }
	u32			width			( ) const { return m_swap_chain_desc.BufferDesc.Width; }
	u32			height			( ) const { return m_swap_chain_desc.BufferDesc.Height; }

	HWND		window			( )	 const				{ return m_window; }
	bool		equal			( HWND window)	const	{ return m_window == window; }

	inline bool	is_registered	( ) const	{ return m_is_registered; }
	inline void	mark_registered	( )			{ R_ASSERT( !m_is_registered); m_is_registered = true; }

private:
	void			initialize_swap_chain	( IDXGISwapChain* swap_chain = NULL);

private:
	void			select_resolution	( u32 &width, u32 &height, bool windowed, HWND window) const;
	DXGI_RATIONAL	select_refresh		( u32 width, u32 height, DXGI_FORMAT fmt) const;

	void			update_targets				( );
	void			update_depth_stencil_buffer	( );
	void			update_window_properties	( ); // Not sure if we need this in render.
	u32				select_presentation_interval( );

public:
	fixed_string<128>		m_depth_rexture_name;
	DXGI_SWAP_CHAIN_DESC	m_swap_chain_desc;	
	IDXGISwapChain*         m_swap_chain;
	ID3DRenderTargetView*	m_base_rt;		//	base render target view
	ID3DDepthStencilView*	m_base_zb;		//	depth/stencil render target view
	ref_texture				m_texture_zb;	//	depth/stencil texture
	HWND					m_window;
	u32						m_present_sync_mode;
	bool					m_windowed;
	bool					m_is_registered;
}; // class res_render_output

typedef	intrusive_ptr<res_render_output, resource_intrusive_base, threading::single_threading_policy>	ref_render_output;
typedef	intrusive_ptr<res_render_output const, resource_intrusive_base const, threading::single_threading_policy>	ref_render_output_const;

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_CORE_RENDER_OUTPUT_H_INCLUDED