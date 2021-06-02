////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/device.h>
#include "com_utils.h"

#if USE_DX10 
#	pragma warning( push )
#	pragma warning( disable : 4995 )
#	include <d3dx10.h>
#	pragma warning( pop )
#endif // #if !USE_DX10 

xray::command_line::key	g_debug_render_device("debug_dx", "", "render", "create d3d device with D3D_CREATE_DEVICE_DEBUG flag");

namespace xray {
namespace render {

inline void	fill_vid_mode_list			( const device* const) {;}
inline void	free_vid_mode_list			() {;}

inline void	fill_render_mode_list		() {;}
inline void	free_render_mode_list		() {;}


device::device		( ):
	m_device		( 0),
	m_context		( 0)
{
	//	Just check if initialization order is correct
	ASSERT( device::ptr());

	create();

	////	need device to be already created.
	//m_caps.update();
}

device::~device()
{
	destroy();
}

void device::create_d3d()
{
	IDXGIFactory * dxgi_factory;
	HRESULT res = CreateDXGIFactory( __uuidof( IDXGIFactory), ( void**)( &dxgi_factory));
	CHECK_RESULT(res);
	
	m_adapter = 0;
	m_use_perfhud = false;

#ifndef	MASTER_GOLD
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	UINT i = 0;
	while( dxgi_factory->EnumAdapters( i, &m_adapter) != DXGI_ERROR_NOT_FOUND)
	{
		m_adapter->GetDesc( &m_adapter_desc);
		if( !wcscmp( m_adapter_desc.Description,L"NVIDIA PerfHUD"))
		{
			m_use_perfhud = true;
			break;
		}
		else
		{
			m_adapter->Release();
			m_adapter = 0;
		}
		++i;
	}
#endif	//	MASTER_GOLD

	if ( !m_adapter)
		dxgi_factory->EnumAdapters( 0, &m_adapter);

	dxgi_factory->Release();
}

void device::destroy_d3d()
{
	log_ref_count	( "m_Adapter", m_adapter);
	safe_release	( m_adapter);
}


void device::create()
{
	create_d3d();

	// TODO: DX10: Create appropriate initialization
	// General - select adapter and device

	// Display the name of video board
	DXGI_ADAPTER_DESC Desc;
	CHECK_RESULT( m_adapter->GetDesc( &Desc));

	//	Warning: Desc.Description is wide string
	LOG_INFO ( "* GPU [vendor:%X]-[device:%X]: %S", Desc.VendorId, Desc.DeviceId, Desc.Description);

	/*
	// Display the name of video board
	D3DADAPTER_IDENTIFIER9	adapterID;
	CHECK_RESULT	( pD3D->GetAdapterIdentifier( DevAdapter,0,&adapterID));
	Msg		( "* GPU [vendor:%X]-[device:%X]: %s",adapterID.VendorId,adapterID.DeviceId,adapterID.Description);

	u16	drv_Product		= HIWORD( adapterID.DriverVersion.HighPart);
	u16	drv_Version		= LOWORD( adapterID.DriverVersion.HighPart);
	u16	drv_SubVersion	= HIWORD( adapterID.DriverVersion.LowPart);
	u16	drv_Build		= LOWORD( adapterID.DriverVersion.LowPart);
	Msg		( "* GPU driver: %d.%d.%d.%d",u32( drv_Product),u32( drv_Version),u32( drv_SubVersion), u32( drv_Build));
	*/

	/*
	m_caps.id_vendor	= adapterID.VendorId;
	m_caps.id_device	= adapterID.DeviceId;
	*/

//	m_caps.id_vendor	= Desc.VendorId;
//	m_caps.id_device	= Desc.DeviceId;

	/*
	// Retreive windowed mode
	D3DDISPLAYMODE mWindowed;
	CHECK_RESULT( pD3D->GetAdapterDisplayMode( DevAdapter, &mWindowed));

	*/

	// Need to specify depth format depend on render options.
/////////////////////////////////////////////////////////////////////////////	fDepth = D3DFMT_D24S8;

	UINT createDeviceFlags = 0;

#ifdef DEBUG
	if(g_debug_render_device.is_set())
		createDeviceFlags |= D3D_CREATE_DEVICE_DEBUG;
#endif

	HRESULT R;
	// Create the device
	//	DX10 don't need it?
	//u32 GPU		= selectGPU();

//	IDXGISwapChain* swap_chain;

#if USE_DX10

	D3D10_DRIVER_TYPE driver_type = D3D10_DRIVER_TYPE_HARDWARE;
	// driver_type = m_caps.bForceGPU_REF ? D3D_DRIVER_TYPE_REFERENCE : D3D_DRIVER_TYPE_HARDWARE;

	if ( m_use_perfhud)
		driver_type =  D3D10_DRIVER_TYPE_REFERENCE;
	
	R =  D3DX10CreateDeviceAndSwapChain
		(   m_adapter,
		driver_type,
		NULL,
		createDeviceFlags,
		&m_render_output.m_swap_chain_desc,
		&swap_chain,
		&m_device);
	if( FAILED(R) && createDeviceFlags&D3D_CREATE_DEVICE_DEBUG )
	{
		createDeviceFlags	^= D3D_CREATE_DEVICE_DEBUG;

		R =  D3DX10CreateDeviceAndSwapChain
			(   m_adapter,
			driver_type,
			NULL,
			createDeviceFlags,
			&m_render_output.m_swap_chain_desc,	
			&swap_chain,
			&m_device);
	}
	m_context = m_device;

	m_feature_level = D3D_FEATURE_LEVEL_10_0;
	if( !FAILED( R))
	{
		//D3DX10GetFeatureLevel1( m_device, &m_device1);
		//if( m_device1)
		//	m_feature_level = D3D_FEATURE_LEVEL_10_1;
	}
#else

	D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_HARDWARE;
	// driver_type = m_caps.bForceGPU_REF ? D3D_DRIVER_TYPE_REFERENCE : D3D_DRIVER_TYPE_HARDWARE;

	if ( m_use_perfhud)
		driver_type =  D3D_DRIVER_TYPE_REFERENCE;


	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	//R =  D3D11CreateDeviceAndSwapChain( 0,//m_adapter,//What wrong with adapter??? We should use another version of DXGI?????
	//	driver_type,
	//	NULL,
	//	createDeviceFlags,
	//	feature_levels,
	//	sizeof( feature_levels)/sizeof( feature_levels[0]),
	//	D3D11_SDK_VERSION,
	//	&m_render_output->m_swap_chain_desc,
	//	&swap_chain,
	//	&m_device,
	//	&m_feature_level,		
	//	&m_context);

	R =  D3D11CreateDevice( 0,//m_adapter,//What wrong with adapter??? We should use another version of DXGI?????
		driver_type,
		NULL,
		createDeviceFlags,
		feature_levels,
		sizeof( feature_levels)/sizeof( feature_levels[0]),
		D3D11_SDK_VERSION,
		&m_device,
		&m_feature_level,		
		&m_context);
#endif

	if ( FAILED( R))
	{
		// Fatal error! Cannot create rendering device AT STARTUP !!!
		LOG_INFO	( 	"Failed to initialize graphics hardware.\n"
						"Please try to restart the game.\n"
						"CreateDevice returned 0x%08x", R);

		//MessageBox			( NULL,"Failed to initialize graphics hardware.\nPlease try to restart the game.","Error!",MB_OK|MB_ICONERROR);
		TerminateProcess	( GetCurrentProcess(),0);
	};
	CHECK_RESULT( R);
	
	UINT out_sample_quality;
	
	for (u32 i = 0; i < 32; i++)
	{
		m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, i, &out_sample_quality);
	}
	for (u32 i = 0; i < 32; i++)
	{
		m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, i, &out_sample_quality);
	}	
	log_ref_count	( "* CREATE: DeviceREF:",m_device);

	/*
	switch ( GPU)
	{
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
	Log	( "* Vertex Processor: SOFTWARE");
	break;
	case D3DCREATE_MIXED_VERTEXPROCESSING:
	Log	( "* Vertex Processor: MIXED");
	break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING:
	Log	( "* Vertex Processor: HARDWARE");
	break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE:
	Log	( "* Vertex Processor: PURE HARDWARE");
	break;
	}
	*/

//	m_render_output->initialize_swap_chain( swap_chain);

	size_t	memory		= Desc.DedicatedVideoMemory;
	LOG_INFO			( "*     Texture memory: %d M",		memory/( 1024*1024));
#ifndef _EDITOR
	fill_vid_mode_list							( this);
#endif



/*
	m_present_sync_mode = select_presentation_interval();

	o.hw_smap = support( D3DFMT_D24X8, D3DRTYPE_TEXTURE, D3DUSAGE_DEPTHSTENCIL);
	//	ASSERT( o.hw_smap);

	o.hw_smap_format = D3DFMT_D24X8;
	o.smap_size = 2048;

	D3DFORMAT null_rt_fmt = ( D3DFORMAT)MAKEFOURCC( 'N','U','L','L');
	o.null_rt_format = support( null_rt_fmt, D3DRTYPE_SURFACE, D3DUSAGE_RENDERTARGET) ? null_rt_fmt : D3DFMT_R5G6B5;

	//o.fp16_filter = support( D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_FILTER);
	o.fp16_blend  = support( D3DFMT_A16B16G16R16F, D3DRTYPE_TEXTURE, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING);
	ASSERT( o.fp16_blend);

*/
}


void device::destroy()
{
	// moved to backend destructor
	//backend::ref().on_device_destroy();
	
	m_context->ClearState();
	
	log_ref_count( "* DESTROY: Device", m_device);
	safe_release( m_device);

	free_vid_mode_list();

	destroy_d3d();
}



// Not sure if we need this in DX10 ========
//
// u32 device::select_gpu() const
// {
// 	if ( m_caps.force_gpu_sw) return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
// 
// 	D3DCAPS9	caps;
// 	m_d3d->GetDeviceCaps( m_dev_adapter, m_dev_type, &caps);
// 
// 	if( caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
// 	{
// 		if ( m_caps.force_gpu_non_pure)	
// 			return D3DCREATE_HARDWARE_VERTEXPROCESSING;
// 		else 
// 		{
// 			if ( caps.DevCaps&D3DDEVCAPS_PUREDEVICE) 
// 				return D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;
// 			else 
// 				return D3DCREATE_HARDWARE_VERTEXPROCESSING;
// 		}
// 	} else return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
// }

//bool device::support( DXGI_FORMAT fmt, DWORD usage)
//{
//	//	TODO:DX10: implement stub for this code.
//	//	ASSERT( !"Implement CHW::support");
//	u32 res;
//	m_device->CheckFormatSupport( fmt, &res);
//
//	return res == ( res&usage);
//}


//bool device::begin_frame()
//{
//	//	TODO: SYSTEM
//	//RCache.OnFrameBegin		();
//	//if ( HW.m_caps.SceneMode)	overdrawBegin	();
//
//	return true;
//}

//void device::end_frame()
//{
//	m_render_output->present();
//	++m_frame;
//
//
//#ifndef DEDICATED_SERVER
//
////	if ( dwPrecacheFrame)
////	{
////		::Sound->set_master_volume	( 0.f);
////		dwPrecacheFrame	--;
////		pApp->load_draw_internal	();
////		if ( 0==dwPrecacheFrame)
////		{
////			m_pRender->updateGamma();
////
////			if( precache_light) precache_light->set_active	( false);
////			if( precache_light) precache_light.destroy		();
////			::Sound->set_master_volume						( 1.f);
////			pApp->destroy_loading_shaders					();
////
////			m_pRender->ResourcesDestroyNecessaryTextures	();
////			Memory.mem_compact								();
////			Msg												( "* MEMORY USAGE: %d K",Memory.mem_usage()/1024);
////			Msg												( "* End of synchronization A[%d] R[%d]",b_is_Active, b_is_Ready);
////
////#ifdef FIND_CHUNK_BENCHMARK_ENABLE
////			g_find_chunk_counter.flush();
////#endif // FIND_CHUNK_BENCHMARK_ENABLE
////
////			CheckPrivilegySlowdown							();
////
////			if( g_pGamePersistent->GameType()==1)//haCk
////			{
////				WINDOWINFO	wi;
////				GetWindowInfo( m_hWnd,&wi);
////				if( wi.dwWindowStatus!=WS_ACTIVECAPTION)
////					Pause( TRUE,TRUE,TRUE,"application start");
////			}
////		}
////	}
//
//	// end scene
//	//	Present goes here, so call OA Frame end.
//	//if ( g_SASH.IsBenchmarkRunning())
//	//	g_SASH.DisplayFrame( Device.fTimeGlobal);
//
//
//	//if ( HW.m_caps.SceneMode)	overdrawEnd();
//
//	//RCache.OnFrameEnd	();
//	//Memory.dbg_check		();
//
//#endif
//
//}

void device::setup_states() const
{
	// General Render States
	//mView.identity			();
	//mProject.identity		();
	//mFullTransform.identity	();
	//vCameraPosition.set		( 0,0,0);
	//vCameraDirection.set	( 0,0,1);
	//vCameraTop.set			( 0,1,0);
	//vCameraRight.set		( 1,0,0);

//	device::ref().update_caps();

	// --Porting to DX10_
	// setup_states_platform();
}

//void device::test_corrporate_level()
//{
//}

// This may be removed for DX10 
device::device_state device::get_device_state() const
{
// 	HRESULT	_hr		= pdevice->TestCooperativeLevel();
// 	if (FAILED(_hr))
// 	{
// 		// If the device was lost, do not render until we get it back
// 		if		(D3DERR_DEVICELOST==_hr)
// 			return ds_lost;
// 
// 		// Check if the device is ready to be reset
// 		if		(D3DERR_DEVICENOTRESET==_hr)
// 			return ds_need_reset;
// 	}
// 
 	return ds_ok;
}

// ID3DRenderTargetView * device::get_base_render_target()
// {
// 	return m_render_output.m_base_rt;
// }
// 
// ID3DDepthStencilView * device::get_depth_stencil_target()
// {
// 	return m_render_output.m_base_zb;
// }

//size_t device::video_memory_size	()
//{
//	return m_adapter_desc.DedicatedVideoMemory;
//}



} // namespace render 
} // namespace xray 
