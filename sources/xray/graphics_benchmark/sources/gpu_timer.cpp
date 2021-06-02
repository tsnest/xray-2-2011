////////////////////////////////////////////////////////////////////////////
//	Created		: 11.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "gpu_timer.h"
#include "benchmark.h"

namespace xray {
namespace graphics_benchmark {


gpu_timer::gpu_timer()
{
	ID3DDevice* d3d_device = xray::render::device::ref().d3d_device();
	
	D3D_QUERY_DESC desc;
	desc.MiscFlags = 0;

	desc.Query = D3D_QUERY_TIMESTAMP_DISJOINT;
	
	if ( FAILED (d3d_device->CreateQuery( &desc, &m_freq_info)))
	{
		printf("gpu_timer: initialization failed!");
		return;
	}

	desc.Query = D3D_QUERY_TIMESTAMP;

	if ( FAILED (d3d_device->CreateQuery( &desc, &m_start_time)) 
	||	 FAILED (d3d_device->CreateQuery( &desc, &m_stop_time))
	)
	{
		printf("gpu_timer: initialization failed!");
		return;
	}
}

gpu_timer::~gpu_timer()
{
	if (m_start_time) m_start_time->Release();
	if (m_stop_time) m_stop_time->Release();
	if (m_freq_info) m_freq_info->Release();
}

void gpu_timer::start()
{
#if USE_DX10
	m_freq_info->Begin();
	m_start_time->End();
#else
	xray::render::device::ref().d3d_context()->Begin(m_freq_info);
	xray::render::device::ref().d3d_context()->End(m_start_time);
#endif
}

void gpu_timer::stop()
{
#if USE_DX10
	m_stop_time->End();
	m_freq_info->End();
#else
	xray::render::device::ref().d3d_context()->End(m_stop_time);
	xray::render::device::ref().d3d_context()->End(m_freq_info);
#endif
}

void gpu_timer::get_elapsed_info(double& out_sec, u64& out_ticks, u64& out_frequency) const
{
	D3D_QUERY_DATA_TIMESTAMP_DISJOINT timer_info;

	u64 start_time	= 0,
		stop_time	= 0;
	
	// Waiting for GPU.
#if USE_DX10
	while ( m_freq_info->GetData(&timer_info, sizeof( timer_info ), 0) == S_FALSE);
	while ( m_start_time->GetData(&start_time, sizeof( start_time ), 0) == S_FALSE);
	while ( m_stop_time->GetData(&stop_time, sizeof( stop_time ), 0) == S_FALSE);
#else
	while ( xray::render::device::ref().d3d_context()->GetData(m_freq_info, &timer_info, sizeof( timer_info ), 0) == S_FALSE);
	while ( xray::render::device::ref().d3d_context()->GetData(m_start_time, &start_time, sizeof( start_time ), 0) == S_FALSE);
	while ( xray::render::device::ref().d3d_context()->GetData(m_stop_time, &stop_time, sizeof( stop_time ), 0) == S_FALSE);
#endif
	
	out_ticks = stop_time - start_time;
	
	if (timer_info.Disjoint)
	{
		out_sec =  0.0f;
		/// ?
		out_frequency = 0;
	}
	else
	{
		out_sec =  (double(out_ticks)) / double(timer_info.Frequency);
		out_frequency = timer_info.Frequency;
	}
}

} // namespace graphics_benchmark
} // namespace xray
