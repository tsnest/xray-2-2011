////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Igor Lobanchikov
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_device.h"

#include "hw_wrapper.h"

namespace xray {
namespace render {


render_device::render_device():
	m_frame_started(false), 
	m_frame(0),
	m_device_state(ds_ok)
{
	//	Just check if initialization order is correct
	ASSERT(hw_wrapper::get_ptr());
}

render_device::~render_device()
{
	;
}

bool render_device::begin_frame()
{
#ifndef DEDICATED_SERVER
	switch (m_device_state/*get_device_state()*/)
	{
	case ds_ok:
		break;

	case ds_lost:
		// If the device was lost, do not render until we get it back
		threading::yield(33);
		return false;

	case ds_need_reset:
		// Check if the device is ready to be reset
		reset();
		break;

	default:
		NODEFAULT();
	}

	begin_scene_platform();

	//	TODO: SYSTEM
	//RCache.OnFrameBegin		();
	//RCache.set_CullMode		(CULL_CW);
	//RCache.set_CullMode		(CULL_CCW);
	//if (HW.Caps.SceneMode)	overdrawBegin	();

	m_frame_started = true;

#endif	//	DEDICATED_SERVER
	return true;
}

void render_device::end_frame()
{
	if (!m_frame_started) return;

#ifndef DEDICATED_SERVER

//	if (dwPrecacheFrame)
//	{
//		::Sound->set_master_volume	(0.f);
//		dwPrecacheFrame	--;
//		pApp->load_draw_internal	();
//		if (0==dwPrecacheFrame)
//		{
//			m_pRender->updateGamma();
//
//			if(precache_light) precache_light->set_active	(false);
//			if(precache_light) precache_light.destroy		();
//			::Sound->set_master_volume						(1.f);
//			pApp->destroy_loading_shaders					();
//
//			m_pRender->ResourcesDestroyNecessaryTextures	();
//			Memory.mem_compact								();
//			Msg												("* MEMORY USAGE: %d K",Memory.mem_usage()/1024);
//			Msg												("* End of synchronization A[%d] R[%d]",b_is_Active, b_is_Ready);
//
//#ifdef FIND_CHUNK_BENCHMARK_ENABLE
//			g_find_chunk_counter.flush();
//#endif // FIND_CHUNK_BENCHMARK_ENABLE
//
//			CheckPrivilegySlowdown							();
//
//			if(g_pGamePersistent->GameType()==1)//haCk
//			{
//				WINDOWINFO	wi;
//				GetWindowInfo(m_hWnd,&wi);
//				if(wi.dwWindowStatus!=WS_ACTIVECAPTION)
//					Pause(TRUE,TRUE,TRUE,"application start");
//			}
//		}
//	}

	m_frame_started = false;
	// end scene
	//	Present goes here, so call OA Frame end.
	//if (g_SASH.IsBenchmarkRunning())
	//	g_SASH.DisplayFrame(Device.fTimeGlobal);


	//if (HW.Caps.SceneMode)	overdrawEnd();

	//RCache.OnFrameEnd	();
	//Memory.dbg_check		();

	end_scene_platform();
#endif

}

void render_device::reset( bool precache ) const
{
	XRAY_UNREFERENCED_PARAMETER	( precache );

	hw_wrapper &hw = hw_wrapper::get_ref();

	bool b_16_before = (float)hw.get_width()/(float)hw.get_height() > (1024.0f/768.0f+0.01f);

	//ShowCursor				(TRUE);
	timing::timer	reset_timer;
	reset_timer.start();

#ifdef DEBUG
	log_ref_count("*ref -CRenderDevice::ResetTotal: DeviceREF:",hw.device());
#endif // DEBUG	

	//	TODO: RESOURCES
	//Resources->reset_begin	();
	//Memory.mem_compact		();
	hw.reset();

	//Resources->reset_end	();

#ifdef DEBUG
	log_ref_count("*ref +CRenderDevice::ResetTotal: DeviceREF:",hw.device());
#endif // DEBUG

//	if (g_pGamePersistent)
//	{
//		g_pGamePersistent->Environment().bNeed_re_create_env = TRUE;
//	}

	setup_states();

//	if (precache)
//		PreCache			(20);

	LOG_INFO("*** RESET [%d ms]", reset_timer.get_elapsed_ms());

	//Memory.mem_compact();

	//seqDeviceReset.Process(rp_DeviceReset);

	bool b_16_after	= (float)hw.get_width()/(float)hw.get_width() > (1024.0f/768.0f+0.01f);

	if(b_16_after!=b_16_before) 
	{
		//if(g_pGameLevel && g_pGameLevel->pHUD) 
		//	g_pGameLevel->pHUD->OnScreenRatioChanged();
	}
}

void render_device::setup_states() const
{
	// General Render States
	//mView.identity			();
	//mProject.identity		();
	//mFullTransform.identity	();
	//vCameraPosition.set		(0,0,0);
	//vCameraDirection.set	(0,0,1);
	//vCameraTop.set			(0,1,0);
	//vCameraRight.set		(1,0,0);

	hw_wrapper::get_ref().update_caps();

	setup_states_platform();
}

void render_device::test_corrporate_level()
{
	m_device_state = get_device_state();
}


} // namespace render 
} // namespace xray 
