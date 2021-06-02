////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "hw_wrapper.h"
#include <xray/render/base/engine_wrapper.h>
#include <dxerr.h>
#include <boost/bind.hpp>

namespace xray {
namespace render {


inline void	fill_vid_mode_list			(const hw_wrapper* const ) {;}
inline void	free_vid_mode_list			() {;}

inline void	fill_render_mode_list		() {;}
inline void	free_render_mode_list		() {;}

hw_wrapper::hw_wrapper( xray::render::engine::wrapper& wrapper, HWND hwnd ):
	m_wrapper		( wrapper ),
	m_hwnd			( hwnd )
{
	m_wrapper.run_in_window_thread( boost::bind( &hw_wrapper::create_device, this ) );

	//	need device to be already created.
	m_caps.update();
}

hw_wrapper::~hw_wrapper()
{
	m_wrapper.run_in_window_thread( boost::bind( &hw_wrapper::destroy_device, this ) );
	//destroy_device();
}

void hw_wrapper::create_d3d()
{
//	LPCSTR		_name			= "xrd3d9-null.dll";

//#ifndef _EDITOR
//	if (!g_dedicated_server)
//#endif    
//	_name			= "d3d9.dll";

	pcstr	_name	= "d3d9.dll";


	m_hd3d	= LoadLibrary(_name);
	R_ASSERT(m_hd3d,"Can't find 'd3d9.dll'\nPlease install latest version of DirectX before running this program");

	typedef IDirect3D9 * WINAPI _Direct3DCreate9(UINT SDKVersion);
	_Direct3DCreate9* createD3D	= (_Direct3DCreate9*)GetProcAddress(m_hd3d,"Direct3DCreate9");	R_ASSERT(createD3D);

	m_d3d			= createD3D( D3D_SDK_VERSION );
	R_ASSERT(m_d3d,"Please install DirectX 9.0c");
}

void hw_wrapper::destroy_d3d()
{
	log_ref_count("m_d3d", m_d3d);
	safe_release(m_d3d);

	FreeLibrary(m_hd3d);
}

void hw_wrapper::create_device( )
{
	create_device(m_hwnd, false);
}

void hw_wrapper::create_device(HWND hwnd, bool move_window)
{
	m_move_window	= move_window;

	create_d3d();

	bool  windowed	= true;

//	if (!g_dedicated_server)
	//	TODO: OPTIONS
		//windowed			= !psDeviceFlags.is(rsFullscreen);

	m_dev_adapter	= D3DADAPTER_DEFAULT;
	m_dev_type	= m_caps.force_gpu_ref ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL;

#ifndef	XRAY_MASTER_GOLD
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT adapter_id=0; adapter_id<m_d3d->GetAdapterCount(); adapter_id++)	
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res = m_d3d->GetAdapterIdentifier(adapter_id, 0, &Identifier);
		if (SUCCEEDED(Res) && (strings::compare(Identifier.Description, "NVIDIA PerfHUD")==0) )
		{
			m_dev_adapter	= adapter_id;
			m_dev_type	= D3DDEVTYPE_REF;
			break;
		}
	}
#endif	//	#ifndef	XRAY_MASTER_GOLD


	// Display the name of video board
	D3DADAPTER_IDENTIFIER9	adapterID;
	R_CHK( m_d3d->GetAdapterIdentifier(m_dev_adapter, 0, &adapterID) );
	LOG_INFO("* gpu [vendor:%X]-[device:%X]: %s", adapterID.VendorId, adapterID.DeviceId, adapterID.Description);

	u16	drv_Product		= HIWORD(adapterID.DriverVersion.HighPart);
	u16	drv_Version		= LOWORD(adapterID.DriverVersion.HighPart);
	u16	drv_SubVersion	= HIWORD(adapterID.DriverVersion.LowPart);
	u16	drv_Build		= LOWORD(adapterID.DriverVersion.LowPart);
	LOG_INFO("* gpu driver: %d.%d.%d.%d", u32(drv_Product), u32(drv_Version), u32(drv_SubVersion), u32(drv_Build));

	m_caps.id_vendor	= adapterID.VendorId;
	m_caps.id_device	= adapterID.DeviceId;

	// Retreive windowed mode
	D3DDISPLAYMODE mWindowed;
	memory::zero(&mWindowed, sizeof(mWindowed));
	R_CHK(m_d3d->GetAdapterDisplayMode(m_dev_adapter, &mWindowed));

	// Select back-buffer & depth-stencil format
	D3DFORMAT&	target_fmt	= m_caps.target_fmt;
	D3DFORMAT&	depth_fmt	= m_caps.depth_fmt;

	if (windowed)
	{
		target_fmt = mWindowed.Format;
		R_CHK(m_d3d->CheckDeviceType(m_dev_adapter, m_dev_type, target_fmt, target_fmt, true));
	} 
	else 
	{
		//	TODO: OPTIONS
// 		
// 		switch (psCurrentBPP) {
// 		case 32:
// 			target_fmt = D3DFMT_X8R8G8B8;
// 			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
// 				break;
// 			target_fmt = D3DFMT_A8R8G8B8;
// 			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
// 				break;
// 			target_fmt = D3DFMT_R8G8B8;
// 			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
// 				break;
// 			target_fmt = D3DFMT_UNKNOWN;
// 			break;
// 		case 16:
// 		default:
// 			target_fmt = D3DFMT_R5G6B5;
// 			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
// 				break;
// 			target_fmt = D3DFMT_X1R5G5B5;
// 			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
// 				break;
// 			target_fmt = D3DFMT_X4R4G4B4;
// 			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
// 				break;
// 			target_fmt = D3DFMT_UNKNOWN;
// 			break;
// 		}
		//	Instead of upper code.
		for(;;)
		{
			target_fmt = D3DFMT_X8R8G8B8;
			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
				break;
			target_fmt = D3DFMT_A8R8G8B8;
			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
				break;
			target_fmt = D3DFMT_R8G8B8;
			if (SUCCEEDED(m_d3d->CheckDeviceType(m_dev_adapter,m_dev_type,target_fmt,target_fmt,FALSE)))
				break;
			target_fmt = D3DFMT_UNKNOWN;
			break;
		}
	}

	depth_fmt = D3DFMT_D24S8;
	//depth_fmt  = selectDepthStencil(target_fmt);

	if ((D3DFMT_UNKNOWN==target_fmt) || (D3DFMT_UNKNOWN==depth_fmt))	{
		LOG_ERROR("Failed to initialize graphics hardware.\nPlease try to restart the game.");
		//	TODO: Flush log here!
		//FlushLog();
		//MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.", "Error!", MB_OK|MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(), 0);
	}


	// Set up the presentation parameters
	D3DPRESENT_PARAMETERS&	pparams	= m_dev_pparams;
	memory::zero( &pparams, sizeof(pparams) );

	select_resolution	(pparams.BackBufferWidth, pparams.BackBufferHeight, windowed);

	pparams.BackBufferFormat	= target_fmt;
	pparams.BackBufferCount		= 1;

	// Multisample
	pparams.MultiSampleType		= D3DMULTISAMPLE_NONE;
	pparams.MultiSampleQuality	= 0;

	// Windoze
	pparams.SwapEffect		= windowed?D3DSWAPEFFECT_COPY:D3DSWAPEFFECT_DISCARD;
	pparams.hDeviceWindow	= hwnd;
	pparams.Windowed		= windowed;

	// Depth/stencil
	pparams.EnableAutoDepthStencil	= true;
	pparams.AutoDepthStencilFormat	= depth_fmt;
	pparams.Flags					= 0;	//. D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	// Refresh rate
	pparams.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	if( windowed )
		pparams.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
	else
		pparams.FullScreen_RefreshRateInHz	= select_refresh(pparams.BackBufferWidth, pparams.BackBufferHeight, target_fmt);

	// Create the device
	u32 gpu		= select_gpu();	
	HRESULT hr	= m_d3d->CreateDevice(m_dev_adapter,
		m_dev_type,
		hwnd,
		gpu,// | D3DCREATE_MULTITHREADED,	//. ? locks at present
		&pparams,
		&m_device );

	if (FAILED(hr))	
	{
		hr	= m_d3d->CreateDevice(	m_dev_adapter,
					m_dev_type,
					hwnd,
					gpu,// | D3DCREATE_MULTITHREADED,	//. ? locks at present
					&pparams,
					&m_device );
	}
	if (D3DERR_DEVICELOST==hr)	
	{
		// Fatal error! Cannot create rendering device AT STARTUP !!!
		LOG_ERROR("Failed to initialize graphics hardware.\nPlease try to restart the game.");
		//	TODO: Flush log here!
		//FlushLog();
		//MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.", "Error!",MB_OK|MB_ICONERROR);
		TerminateProcess(GetCurrentProcess(),0);
	};
	R_CHK		(hr);

	log_ref_count("* CREATE: Device", m_device);
	switch (gpu)
	{
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
		LOG_INFO("* Vertex Processor: SOFTWARE");
		break;
	case D3DCREATE_MIXED_VERTEXPROCESSING:
		LOG_INFO("* Vertex Processor: MIXED");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING:
		LOG_INFO("* Vertex Processor: HARDWARE");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE:
		LOG_INFO("* Vertex Processor: PURE HARDWARE");
		break;
	}

	// Capture misc data
	R_CHK( m_device->GetRenderTarget(0, &m_base_rt) );
	R_CHK( m_device->GetDepthStencilSurface(&m_base_zb) );

	u32	memory = m_device->GetAvailableTextureMem();
	LOG_INFO("*     Texture memory: %d M",	memory/(1024*1024));
	LOG_INFO("*          DDI-level: %2.1f",	float(D3DXGetDriverLevel(m_device))/100.f);

	update_window_props(hwnd);
	fill_vid_mode_list(this);

	o.hw_smap = support(D3DFMT_D24X8, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL);
//	ASSERT(o.hw_smap);

	o.hw_smap_format = D3DFMT_D24X8;
	o.smap_size = 2048;

	D3DFORMAT null_rt_fmt = (D3DFORMAT)MAKEFOURCC('N','U','L','L');
	o.null_rt_format = support(null_rt_fmt, D3DRTYPE_SURFACE, D3DUSAGE_RENDERTARGET) ? null_rt_fmt : D3DFMT_R5G6B5;

	//o.fp16_filter = support(D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER);
	o.fp16_blend  = support(D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING);
	ASSERT(o.fp16_blend);
}

void hw_wrapper::destroy_device()
{
	log_ref_count("m_base_zb",m_base_zb);
	safe_release(m_base_zb);

	log_ref_count("m_base_rt",m_base_rt);
	safe_release(m_base_rt);

	log_ref_count("* DESTROY: Device", m_device);
	safe_release(m_device);

	free_vid_mode_list();

	destroy_d3d();
}

void hw_wrapper::update_window_props(HWND hw) const
{
	XRAY_UNREFERENCED_PARAMETER	( hw );

	/*
	BOOL	windowed				= TRUE;
#ifndef _EDITOR
	if (!g_dedicated_server)
		windowed			= !psDeviceFlags.is(rsFullscreen);
#endif	

	u32		dwWindowStyle			= 0;
	// Set window properties depending on what mode were in.
	if (windowed)		{
		if (m_move_window) {
			if (strstr(Core.Params,"-no_dialog_header"))
				SetWindowLong	( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_VISIBLE) );
			else
				SetWindowLong	( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_DLGFRAME|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX ) );
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
			if (strstr(Core.Params, "-center_screen"))	bCenter = TRUE;

#ifndef _EDITOR
			if (g_dedicated_server)
				bCenter		= TRUE;
#endif

			if(bCenter){
				RECT				DesktopRect;

				GetClientRect		(GetDesktopWindow(), &DesktopRect);

				SetRect(			&m_rcWindowBounds, 
					(DesktopRect.right-m_dev_pparams.BackBufferWidth)/2, 
					(DesktopRect.bottom-m_dev_pparams.BackBufferHeight)/2, 
					(DesktopRect.right+m_dev_pparams.BackBufferWidth)/2, 
					(DesktopRect.bottom+m_dev_pparams.BackBufferHeight)/2			);
			}else{
				SetRect(			&m_rcWindowBounds,
					0, 
					0, 
					m_dev_pparams.BackBufferWidth, 
					m_dev_pparams.BackBufferHeight );
			};

			AdjustWindowRect		(	&m_rcWindowBounds, dwWindowStyle, FALSE );

			SetWindowPos			(	m_hWnd, 
				HWND_TOP,	
				m_rcWindowBounds.left, 
				m_rcWindowBounds.top,
				( m_rcWindowBounds.right - m_rcWindowBounds.left ),
				( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
				SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_DRAWFRAME );
		}
	}
	else
	{
		SetWindowLong			( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_POPUP|WS_VISIBLE) );
	}

#ifndef _EDITOR
	if (!g_dedicated_server)
	{
		ShowCursor	(FALSE);
		SetForegroundWindow( m_hWnd );
	}
#endif
	*/
}

u32 hw_wrapper::select_gpu() const
{
	if (m_caps.force_gpu_sw) return D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DCAPS9	caps;
	m_d3d->GetDeviceCaps(m_dev_adapter, m_dev_type, &caps);

	if(caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		if (m_caps.force_gpu_non_pure)	
			return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else 
		{
			if (caps.DevCaps&D3DDEVCAPS_PUREDEVICE) 
				return D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;
			else 
				return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
	} else return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}

u32 hw_wrapper::select_refresh(u32 width, u32 height, D3DFORMAT fmt) const
{
	XRAY_UNREFERENCED_PARAMETERS	( width, height, fmt );

	return D3DPRESENT_RATE_DEFAULT;
	//	TODO: OPTIONS
	/*
	if (psDeviceFlags.is(rsRefresh60hz))	return D3DPRESENT_RATE_DEFAULT;
	else
	{
		u32 selected	= D3DPRESENT_RATE_DEFAULT;
		u32 count		= pD3D->GetAdapterModeCount(m_dev_adapter,fmt);
		for (u32 I=0; I<count; I++)
		{
			D3DDISPLAYMODE	Mode;
			pD3D->EnumAdapterModes(m_dev_adapter,fmt,I,&Mode);
			if (Mode.Width==dwWidth && Mode.Height==dwHeight)
			{
				if (Mode.RefreshRate>selected) selected = Mode.RefreshRate;
			}
		}
		return selected;
	}
	*/
}

bool hw_wrapper::support(D3DFORMAT fmt, DWORD type, DWORD usage)
{
	HRESULT hr = m_d3d->CheckDeviceFormat(
		m_dev_adapter,
		m_dev_type,
		m_caps.target_fmt,
		usage,
		(D3DRESOURCETYPE)type,fmt);

	return SUCCEEDED(hr);
}

void hw_wrapper::select_resolution(u32 &width, u32 &height, bool windowed) const
{
	fill_vid_mode_list(this);

	if(windowed)
	{
		//	TODO: OPTIONS
		//width		= psCurrentVidMode[0];
		//height	= psCurrentVidMode[1];
		width = 0;
		height = 0;
	}
	else
	{
		ASSERT(!"hw_wrapper::select_resolution: Not implemented for fullscreen.");
		//	TODO: OPTIONS
		//string64					buff;
		//sprintf_s					(buff,sizeof(buff),"%dx%d",psCurrentVidMode[0],psCurrentVidMode[1]);

		//if(_ParseItem(buff,vid_mode_token)==u32(-1)) //not found
		//{ //select safe
		//	sprintf_s				(buff,sizeof(buff),"vid_mode %s",vid_mode_token[0].name);
		//	Console->Execute		(buff);
		//}

		//dwWidth						= psCurrentVidMode[0];
		//dwHeight					= psCurrentVidMode[1];
	}
}

void hw_wrapper::reset()
{
	log_ref_count("reset: m_base_zb",m_base_zb);
	safe_release(m_base_zb);

	log_ref_count("reset: m_base_rt",m_base_rt);
	safe_release(m_base_rt);

	bool windowed = true;

	//	TODO: OPTIONS
	//windowed		= !psDeviceFlags.is	(rsFullscreen);

	select_resolution(m_dev_pparams.BackBufferWidth, m_dev_pparams.BackBufferHeight, windowed);
	// Windoze
	m_dev_pparams.SwapEffect = windowed ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_DISCARD;
	m_dev_pparams.Windowed = windowed;
	m_dev_pparams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	if( !windowed )		
		m_dev_pparams.FullScreen_RefreshRateInHz = select_refresh(m_dev_pparams.BackBufferWidth,m_dev_pparams.BackBufferHeight, m_caps.target_fmt);
	else
		m_dev_pparams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	for(;;)
	{
		HRESULT _hr = m_device->Reset(&m_dev_pparams);
		if (SUCCEEDED(_hr))
			break;
		//LOG_ERROR("[%dx%d]: %s",m_dev_pparams.BackBufferWidth,m_dev_pparams.BackBufferHeight,Debug.error2string(_hr));
		LOG_ERROR("[%dx%d]: %s",m_dev_pparams.BackBufferWidth,m_dev_pparams.BackBufferHeight,DXGetErrorString(_hr));
		threading::yield(100);
	}
	R_CHK(m_device->GetRenderTarget			(0,&m_base_rt));
	R_CHK(m_device->GetDepthStencilSurface	(&m_base_zb));

	update_window_props(m_dev_pparams.hDeviceWindow);
}

} // namespace render 
} // namespace xray 
