////////////////////////////////////////////////////////////////////////////
//	Created		: 17.09.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/res_render_output.h>
#include "com_utils.h"
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/device.h>

static pcstr const depth_render_target_texture_name	= "$user$depth";

namespace xray {
namespace render {

res_render_output::res_render_output( HWND window, bool windowed):
m_swap_chain	(NULL),
m_base_rt		(NULL),
m_base_zb		(NULL),
m_window		( window),
m_present_sync_mode	( D3DPRESENT_INTERVAL_DEFAULT),
m_windowed		( windowed),
m_is_registered	( false )
{
	ZeroMemory	( &m_swap_chain_desc, sizeof( m_swap_chain_desc));
	
	m_window	= window;
	m_windowed	= windowed;

	//	TODO: DX10: implement dynamic format selection
	//m_swap_chain_desc.BufferDesc.Format		= fTarget;

	m_swap_chain_desc.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	m_swap_chain_desc.BufferCount		= 1;
	m_swap_chain_desc.BufferUsage		= DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set up the presentation parameters
	select_resolution	( m_swap_chain_desc.BufferDesc.Width, m_swap_chain_desc.BufferDesc.Height, m_windowed, m_window);

	if( m_windowed)
	{
		m_swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		m_swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}
	else
	{
		m_swap_chain_desc.BufferDesc.RefreshRate = select_refresh( m_swap_chain_desc.BufferDesc.Width, m_swap_chain_desc.BufferDesc.Height, m_swap_chain_desc.BufferDesc.Format);
	}

	// Multisample
	m_swap_chain_desc.SampleDesc.Count = 1;
	m_swap_chain_desc.SampleDesc.Quality = 0;

	//P.SwapEffect			= bWindowed?D3DSWAPEFFECT_COPY:D3DSWAPEFFECT_DISCARD;
	//P.hDeviceWindow			= m_hWnd;
	//P.Windowed				= bWindowed;
	m_swap_chain_desc.SwapEffect		= DXGI_SWAP_EFFECT_DISCARD;
	m_swap_chain_desc.OutputWindow	= m_window;
	m_swap_chain_desc.Windowed		= m_windowed;

	// This may be useful !	
	m_swap_chain_desc.Flags			= 0;

	initialize_swap_chain			( );
}

res_render_output::~res_render_output	( )
{
	log_ref_count( "m_base_zb",m_base_zb);
	safe_release( m_base_zb);

	log_ref_count( "m_base_rt",m_base_rt);
	safe_release( m_base_rt);
}

void res_render_output::destroy_impl() const
{
	resource_manager::ref().release( this );
}

void res_render_output::initialize_swap_chain	( IDXGISwapChain* swap_chain)
{
	ASSERT( !m_swap_chain);

	if( !swap_chain)
	{
		IDXGIDevice * pDXGIDevice;
		HRESULT hr = device::ref().d3d_device()->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
		R_ASSERT( hr == S_OK );
		      
		IDXGIAdapter * pDXGIAdapter;
		hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
		R_ASSERT( hr == S_OK );

		IDXGIFactory * dxgi_factory;
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&dxgi_factory);
		CHECK_RESULT(dxgi_factory);

		hr = dxgi_factory->CreateSwapChain( device::ref().d3d_device(), &m_swap_chain_desc, &m_swap_chain);
		R_ASSERT( hr == S_OK );
	}
	else
		m_swap_chain = swap_chain;

	//	Create render target and depth-stencil views here
	update_targets();
}

void res_render_output::present	( )
{
	HRESULT res = m_swap_chain->Present( m_present_sync_mode, 0 );
	CHECK_RESULT( res);
}

DXGI_RATIONAL res_render_output::select_refresh( u32 width, u32 height, DXGI_FORMAT fmt) const
{
	XRAY_UNREFERENCED_PARAMETERS	( width, height, fmt);

	DXGI_RATIONAL res;
	res.Numerator	= 60;
	res.Denominator = 1;

	return res;
	//	TODO: OPTIONS
	/*
	if ( psDeviceFlags.is( rsRefresh60hz))	return D3DPRESENT_RATE_DEFAULT;
	else
	{
	u32 selected	= D3DPRESENT_RATE_DEFAULT;
	u32 count		= pD3D->GetAdapterModeCount( m_dev_adapter,fmt);
	for ( u32 I=0; I<count; I++)
	{
	D3DDISPLAYMODE	Mode;
	pD3D->EnumAdapterModes( m_dev_adapter,fmt,I,&Mode);
	if ( Mode.Width==dwWidth && Mode.Height==dwHeight)
	{
	if ( Mode.RefreshRate>selected) selected = Mode.RefreshRate;
	}
	}
	return selected;
	}
	*/
}

void res_render_output::select_resolution( u32 & width, u32 & height, bool windowed, HWND window) const
{
	if( windowed)
	{
		RECT rect;
		if (GetClientRect( window, &rect))
		{
			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
		}
		else
		{
			DWORD error_code = GetLastError();
			XRAY_UNREFERENCED_PARAMETER(error_code);
		}
	}
	else
	{
		width	= 1024;
		height	= 768;
	}
}

void	res_render_output::resize				( )
{
	resize( m_windowed);
}

void res_render_output::resize( bool windowed)
{
	m_windowed = windowed;
	m_swap_chain_desc.Windowed	= m_windowed;

	CHECK_RESULT( m_swap_chain->SetFullscreenState( !m_windowed, NULL));

	DXGI_MODE_DESC	&buffer_desc = m_swap_chain_desc.BufferDesc;

	math::uint2	new_size = math::uint2(0, 0);
	select_resolution( new_size.x, new_size.y, m_windowed, m_window);

	if( buffer_desc.Width == new_size.x && buffer_desc.Height == new_size.y)
		return;

	if ( new_size.x < 16 )
		return;

	if ( new_size.y < 16 )
		return;

	buffer_desc.Width = new_size.x;
	buffer_desc.Height = new_size.y;

	if( m_windowed)
	{
		buffer_desc.RefreshRate.Numerator = 60;
		buffer_desc.RefreshRate.Denominator = 1;
	}
	else
		buffer_desc.RefreshRate = select_refresh( buffer_desc.Width, buffer_desc.Height, buffer_desc.Format);

	CHECK_RESULT( m_swap_chain->ResizeTarget(&buffer_desc));


#ifdef DEBUG
	//	_RELEASE			(dwDebugSB);
#endif
	log_ref_count		("refCount:pBaseZB",m_base_zb);
	log_ref_count		("refCount:pBaseRT",m_base_rt);

	//safe_release(m_base_zb);
	safe_release(m_base_rt);
	m_texture_zb->set_hw_texture(NULL);

	CHECK_RESULT(	m_swap_chain->ResizeBuffers(
		m_swap_chain_desc.BufferCount,
		buffer_desc.Width,
		buffer_desc.Height,
		buffer_desc.Format,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	update_targets();

	update_window_properties	( );

	m_present_sync_mode = select_presentation_interval();

	// --Porting to DX10_
	//setup_states();
}


void res_render_output::update_targets()
{
	HRESULT res;

	// Create a render target view
	ID3DTexture2D *buffer;
	res =  m_swap_chain->GetBuffer( 0, __uuidof( ID3DTexture2D), ( void**)&buffer);
	CHECK_RESULT(res);

	res = device::ref().d3d_device()->CreateRenderTargetView( buffer, NULL, &m_base_rt);
	buffer->Release();
	CHECK_RESULT(res);

	update_depth_stencil_buffer();
}

void res_render_output::update_depth_stencil_buffer()
{
	ID3DTexture2D * depth_texture;

	//	HACK: DX10: hard depth buffer format
	D3D_TEXTURE2D_DESC desc_depth;
	desc_depth.Width				= m_swap_chain_desc.BufferDesc.Width;
	desc_depth.Height				= m_swap_chain_desc.BufferDesc.Height;
	desc_depth.MipLevels			= 1;
	desc_depth.ArraySize			= 1;
	desc_depth.Format				= DXGI_FORMAT_R24G8_TYPELESS; //*/ DXGI_FORMAT_D24_UNORM_S8_UINT;// m_caps.depth_fmt
	desc_depth.SampleDesc.Count		= 1;
	desc_depth.SampleDesc.Quality	= 0;
	desc_depth.Usage				= D3D_USAGE_DEFAULT;
	desc_depth.BindFlags			= D3D_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	desc_depth.CPUAccessFlags		= 0;
	desc_depth.MiscFlags			= 0;

	HRESULT res = device::ref().d3d_device()->CreateTexture2D( &desc_depth,		// Texture desc
		NULL,				// Initial data
		&depth_texture);	// [out] Texture
	CHECK_RESULT( res);

	D3D_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV));

	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	//	Create Depth/stencil view
	res = device::ref().d3d_device()->CreateDepthStencilView( depth_texture, &descDSV, &m_base_zb);
	CHECK_RESULT( res);

	static u32 depth_texture_id = 0;
	m_depth_rexture_name.assignf("%s%d", depth_render_target_texture_name, depth_texture_id++);

	m_texture_zb = resource_manager::ref().create_texture( m_depth_rexture_name.get_buffer());
	m_texture_zb->set_hw_texture( depth_texture);

	depth_texture->Release();
}


u32	res_render_output::select_presentation_interval	()
{
	// Todo: consider render options to turn on vsync
	// 	if ( psDeviceFlags.test( rsVSync)) 
	// 	{
	// 		// 		if ( caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
	// 		// 			return D3DPRESENT_INTERVAL_IMMEDIATE;
	// 		return D3DPRESENT_INTERVAL_ONE;
	// 	}
	return D3DPRESENT_INTERVAL_DEFAULT;
}


void res_render_output::update_window_properties() 
{
	/*
	BOOL	windowed				= TRUE;
	#ifndef _EDITOR
	if ( !g_dedicated_server)
	windowed			= !psDeviceFlags.is( rsFullscreen);
	#endif	

	u32		dwWindowStyle			= 0;
	// Set window properties depending on what mode were in.
	if ( windowed)		{
	if ( m_move_window) {
	if ( strstr( Core.Params,"-no_dialog_header"))
	SetWindowLong	( m_hWnd, GWL_STYLE, dwWindowStyle=( WS_BORDER|WS_VISIBLE));
	else
	SetWindowLong	( m_hWnd, GWL_STYLE, dwWindowStyle=( WS_BORDER|WS_DLGFRAME|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX));
	// When moving from fullscreen to windowed mode, it is important to
	// adjust the window size after recreating the device rather than
	// beforehand to ensure that you get the window size you want.  For
	// example, when switching from 640x480 fullscreen to windowed with
	// a 1000x600 window on a 1024x768 desktop, it is impossible to set
	// the window size to 1000x600 until after the display mode has
	// changed to 1024x768, because windows cannot be larger than the
	// desktop.

	RECT			m_rcWindowBounds;
	BOOL			bCenter = FALSE;
	if ( strstr( Core.Params, "-center_screen"))	bCenter = TRUE;

	#ifndef _EDITOR
	if ( g_dedicated_server)
	bCenter		= TRUE;
	#endif

	if( bCenter){
	RECT				DesktopRect;

	GetClientRect		( GetDesktopWindow(), &DesktopRect);

	SetRect( 			&m_rcWindowBounds, 
	( DesktopRect.right-m_dev_pparams.BackBufferWidth)/2, 
	( DesktopRect.bottom-m_dev_pparams.BackBufferHeight)/2, 
	( DesktopRect.right+m_dev_pparams.BackBufferWidth)/2, 
	( DesktopRect.bottom+m_dev_pparams.BackBufferHeight)/2			);
	}else{
	SetRect( 			&m_rcWindowBounds,
	0, 
	0, 
	m_dev_pparams.BackBufferWidth, 
	m_dev_pparams.BackBufferHeight);
	};

	AdjustWindowRect		( 	&m_rcWindowBounds, dwWindowStyle, FALSE);

	SetWindowPos			( 	m_hWnd, 
	HWND_TOP,	
	m_rcWindowBounds.left, 
	m_rcWindowBounds.top,
	( m_rcWindowBounds.right - m_rcWindowBounds.left),
	( m_rcWindowBounds.bottom - m_rcWindowBounds.top),
	SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_DRAWFRAME);
	}
	}
	else
	{
	SetWindowLong			( m_hWnd, GWL_STYLE, dwWindowStyle=( WS_POPUP|WS_VISIBLE));
	}

	#ifndef _EDITOR
	if ( !g_dedicated_server)
	{
	ShowCursor	( FALSE);
	SetForegroundWindow( m_hWnd);
	}
	#endif
	*/
}


} // namespace render
} // namespace xray
