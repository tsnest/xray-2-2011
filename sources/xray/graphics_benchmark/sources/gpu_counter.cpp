////////////////////////////////////////////////////////////////////////////
//	Created		: 24.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "gpu_counter.h"
#include "strsafe.h"
#include "winnls.h"

namespace xray {
namespace graphics_benchmark {

#if USE_DX10

	static void OutputCounterDetails( ID3DDevice* pDevice, const D3D_COUNTER& id );

	static void OutputCounters( ID3DDevice* pDevice )
	{
		D3D_COUNTER_INFO CounterInfo;
		pDevice->CheckCounterInfo( &CounterInfo );

		// Print the basic information
		WCHAR wcBasicInfo[512];
		StringCchPrintfW( wcBasicInfo, 512, L"\tBasic device information:\n"
			L"\t\tMaximum number of simultaneous counters: %d\n"
			L"\t\tDetectable parallel units: %d\n"
			L"\t\tNumber of device-specific counters: %d\n",
			CounterInfo.NumSimultaneousCounters,
			CounterInfo.NumDetectableParallelUnits,
			CounterInfo.LastDeviceDependentCounter - D3D_COUNTER_DEVICE_DEPENDENT_0
			);
		OutputDebugStringW( wcBasicInfo );

		// Iterate through all the device independent counters (18 total)
		OutputDebugStringW( L"\tDevice independent counters:\n" );

		for( UINT i = 0; i < 18; ++i )
			OutputCounterDetails( pDevice, static_cast< D3D_COUNTER >( i ) );

		if( 0 != (CounterInfo.LastDeviceDependentCounter - D3D_COUNTER_DEVICE_DEPENDENT_0) )
			OutputDebugStringW( L"\tDevice dependent counters:\n" );

		for( int i = D3D_COUNTER_DEVICE_DEPENDENT_0; i < CounterInfo.LastDeviceDependentCounter; ++i )
			OutputCounterDetails( pDevice, static_cast< D3D_COUNTER >( i ) );
	}

	static void OutputCounterDetails( ID3DDevice* pDevice, const D3D_COUNTER& id )
	{
		// Extract information about this particular counter
		D3D_COUNTER_DESC  d     = { id, 0 };
		D3D_COUNTER_TYPE  type    = static_cast< D3D_COUNTER_TYPE >( 0 );
		UINT     uiSlotsRequired  = 0;
		LPSTR     sName    = NULL;
		UINT     uiNameLength  = 0;
		LPSTR     sUnits    = NULL;
		UINT     uiUnitsLength  = 0;
		LPSTR     sDesc    = NULL;
		UINT     uiDescLength  = 0;

		if( SUCCEEDED( pDevice->CheckCounter( &d, &type, &uiSlotsRequired, NULL, &uiNameLength, NULL, &uiUnitsLength, NULL, &uiDescLength ) ) )
		{
			// We now know how much space to allocate for each string:
			sName  = NEW_ARRAY(char, uiNameLength);
			sUnits  = NEW_ARRAY(char, uiUnitsLength);
			sDesc  = NEW_ARRAY(char, uiDescLength);

			// Repeat the call in order to grab the text fields as well:
			if( FAILED( pDevice->CheckCounter( &d, &type, &uiSlotsRequired, sName, &uiNameLength, sUnits, &uiUnitsLength, sDesc, &uiDescLength ) ) )
			{
				//ERR_OUT( L"Failed to retrieve counter's string fields!" );

				DELETE_ARRAY( sName );
				DELETE_ARRAY( sUnits );
				DELETE_ARRAY( sDesc );

				return;
			}

			WCHAR wcTypeName[64];
			switch( type )
			{
			case D3D_COUNTER_TYPE_FLOAT32:
				wcscpy_s( wcTypeName, 64, L"32bit float" );
				break;

			case D3D_COUNTER_TYPE_UINT16:
				wcscpy_s( wcTypeName, 64, L"unsigned 16bit integer" );
				break;

			case D3D_COUNTER_TYPE_UINT32:
				wcscpy_s( wcTypeName, 64, L"unsigned 32bit integer" );
				break;

			case D3D_COUNTER_TYPE_UINT64:
				wcscpy_s( wcTypeName, 64, L"unsigned 64bit integer" );
				break;
			}

			// Convert ASCII strings to Unicode for output...
			//int size = 0;
/*
			size = MultiByteToWideChar( 0, 0, sName, -1, NULL, 0 );
			WCHAR *wcName = new WCHAR[ size ];
			MultiByteToWideChar( CP_ACP, 0, sName, -1, wcName, size );

			size = MultiByteToWideChar( 0, 0, sUnits, -1, NULL, 0 );
			WCHAR *wcUnits = new WCHAR[ size ];
			MultiByteToWideChar( CP_ACP, 0, sUnits, -1, wcUnits, size );

			size = MultiByteToWideChar( 0, 0, sDesc, -1, NULL, 0 );
			WCHAR *wcDesc = new WCHAR[ size ];
			MultiByteToWideChar( 0, 0, sDesc, -1, wcDesc, size );
*/
			// Check if this counter type is actually supported
			HRESULT hr = pDevice->CreateCounter( &d, NULL );
			if( SUCCEEDED( hr ) )
			{
				// We can use this counter just fine!
				CHAR wcOut[512];
				StringCchPrintf( wcOut, 512, "\t\t'%s' can be used with this device.\n"
					"\t\t\t%s\n"
					"\t\t\tRequires %d counter(s)\n"
					"\t\t\t%s %s\n",
					sName, 
					sDesc,
					uiSlotsRequired,
					"", sUnits
					);
				OutputDebugString( wcOut );
			}

			// Make sure we release the temporary data
			DELETE_ARRAY( sName );
			DELETE_ARRAY( sUnits );
			DELETE_ARRAY( sDesc );
		}
	}

#endif // #if USE_DX10




gpu_counter::gpu_counter()
{
#if USE_DX10
	D3D_COUNTER_DESC desc;
	desc.Counter = D3D_COUNTER_DEVICE_DEPENDENT_0;
	desc.MiscFlags = 0;
	m_pixel_processing_counter = NULL;

	HRESULT hr = xray::render::device::ref().d3d_device()->CreateCounter( &desc, &m_pixel_processing_counter );

	if (hr==S_FALSE)
		printf("\ngpu_counter: S_FALSE");
	else if (hr==E_OUTOFMEMORY)
		printf("\ngpu_counter: E_OUTOFMEMORY");
	else if (hr==DXGI_ERROR_UNSUPPORTED)
		printf("\ngpu_counter: DXGI_ERROR_UNSUPPORTED");
	else if (hr==DXGI_ERROR_NONEXCLUSIVE)
		printf("\ngpu_counter: DXGI_ERROR_NONEXCLUSIVE");
	else if (hr==E_INVALIDARG)
		printf("\ngpu_counter: E_INVALIDARG");


	const int nCounters = 18;
	const D3D_COUNTER type[] = {
		D3D_COUNTER_VERTEX_PROCESSING,
		D3D_COUNTER_GEOMETRY_PROCESSING,
		D3D_COUNTER_PIXEL_PROCESSING,
		D3D_COUNTER_OTHER_GPU_PROCESSING,
		D3D_COUNTER_HOST_ADAPTER_BANDWIDTH_UTILIZATION,
		D3D_COUNTER_LOCAL_VIDMEM_BANDWIDTH_UTILIZATION,
		D3D_COUNTER_VERTEX_THROUGHPUT_UTILIZATION,
		D3D_COUNTER_TRIANGLE_SETUP_THROUGHPUT_UTILIZATION,
		D3D_COUNTER_FILLRATE_THROUGHPUT_UTILIZATION,
		D3D_COUNTER_VS_MEMORY_LIMITED,
		D3D_COUNTER_VS_COMPUTATION_LIMITED,
		D3D_COUNTER_GS_MEMORY_LIMITED,
		D3D_COUNTER_GS_COMPUTATION_LIMITED,
		D3D_COUNTER_PS_MEMORY_LIMITED,
		D3D_COUNTER_PS_COMPUTATION_LIMITED,
		D3D_COUNTER_POST_TRANSFORM_CACHE_HIT_RATE,
		D3D_COUNTER_TEXTURE_CACHE_HIT_RATE,
		D3D_COUNTER_DEVICE_DEPENDENT_0
	};
	printf("\nchecking device counters...");

	ID3D10Counter *Counter[ nCounters ];
	for(int i=0;i<nCounters;i++) {
		D3D_COUNTER_DESC d = { type[ i ], 0 };
		HRESULT h = xray::render::device::ref().d3d_device()->CreateCounter( &d, &(Counter[ i ]) );

		FAILED(h) ? printf("\nnot supported counter (%d)",i) : printf("\n*** supported (%d)",i);
	}
	OutputCounters(xray::render::device::ref().d3d_device());

#endif // #if USE_DX10
}

gpu_counter::~gpu_counter()
{
	if (m_pixel_processing_counter) m_pixel_processing_counter->Release();
}

void gpu_counter::start()
{
	ASSERT(m_pixel_processing_counter);

#if USE_DX10
	m_pixel_processing_counter->Begin();
#else
	xray::render::device::ref().d3d_context()->Begin(m_pixel_processing_counter);
#endif
}

void gpu_counter::stop()
{
	ASSERT(m_pixel_processing_counter);
#if USE_DX10
	m_pixel_processing_counter->End();
#else
	xray::render::device::ref().d3d_context()->End(m_pixel_processing_counter);
#endif
}

u64 gpu_counter::get_pixel_processing_time() const
{
	u64 pixel_processing_time = 0;
	// waiting for GPU
#if USE_DX10
	while ( m_pixel_processing_counter->GetData(&pixel_processing_time, sizeof(pixel_processing_time), 0) == S_FALSE );
#else
	while ( xray::render::device::ref().d3d_context()->GetData(m_pixel_processing_counter, &pixel_processing_time, sizeof(pixel_processing_time), 0) == S_FALSE );
#endif

	return pixel_processing_time;
}



} // namespace graphics_benchmark
} // namespace xray

