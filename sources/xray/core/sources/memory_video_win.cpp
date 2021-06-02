////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
//	Description	: code copied mostly from DirectX SDK
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/platform_extensions_win.h>
#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOGDI
#undef NOMSG
#undef NONLS
#include <xray/os_include.h>
#include <oleauto.h>
#include <d3d9.h>
#include <wbemidl.h>
#include <ddraw.h>

#pragma comment( lib, "wbemuuid.lib" )
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "dxguid.lib" )

typedef BOOL ( WINAPI* PfnCoSetProxyBlanket )( IUnknown* pProxy, DWORD dwAuthnSvc, DWORD dwAuthzSvc,
                                               OLECHAR* pServerPrincName, DWORD dwAuthnLevel, DWORD dwImpLevel,
                                               RPC_AUTH_IDENTITY_HANDLE pAuthInfo, DWORD dwCapabilities );

template < typename T >
inline void safe_release	( T*& instance )
{
	if ( !instance )
		return;

	instance->Release	( );
	instance			= 0;
}

struct DDRAW_MATCH
{
    GUID guid;
    HMONITOR hMonitor;
    CHAR strDriverName[512];
    bool bFound;
};

BOOL WINAPI DDEnumCallbackEx( GUID FAR* lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext, HMONITOR hm )
{
	XRAY_UNREFERENCED_PARAMETER	( lpDriverDescription );

	DDRAW_MATCH* pDDMatch = ( DDRAW_MATCH* ) lpContext;
	if( pDDMatch->hMonitor == hm )
	{
		pDDMatch->bFound = true;
		strcpy_s( pDDMatch->strDriverName, 512, lpDriverName );
		memcpy( &pDDMatch->guid, lpGUID, sizeof( GUID ) );
	}
	return TRUE;
}

HRESULT GetDeviceIDFromHMonitor( HMONITOR hm, WCHAR* strDeviceID, int cchDeviceID )
{

    HINSTANCE hInstDDraw;

    hInstDDraw = LoadLibrary( "ddraw.dll" );
    if( hInstDDraw )
    {
        DDRAW_MATCH match;
        ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
        match.hMonitor = hm;

        LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = NULL;
        pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

        if( pDirectDrawEnumerateEx )
            pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );

        if( match.bFound )
        {
            LONG iDevice = 0;
            DISPLAY_DEVICEA dispdev;

            ZeroMemory( &dispdev, sizeof( dispdev ) );
            dispdev.cb = sizeof( dispdev );

            while( EnumDisplayDevicesA( NULL, iDevice, ( DISPLAY_DEVICEA* )&dispdev, 0 ) )
            {
                // Skip devices that are monitors that echo another display
                if( dispdev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER )
                {
                    iDevice++;
                    continue;
                }

                // Skip devices that aren't attached since they cause problems
                if( ( dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
                {
                    iDevice++;
                    continue;
                }

                if( _stricmp( match.strDriverName, dispdev.DeviceName ) == 0 )
                {
                    MultiByteToWideChar( CP_ACP, 0, dispdev.DeviceID, -1, strDeviceID, cchDeviceID );
					FreeLibrary( hInstDDraw );
                    return S_OK;
                }

                iDevice++;
            }
        }

        FreeLibrary( hInstDDraw );
    }

    return E_FAIL;
}

HRESULT GetVideoMemoryViaWMI( HMONITOR hMonitor, u64* pdwAdapterRam )
{
    WCHAR strInputDeviceID[512];
    GetDeviceIDFromHMonitor( hMonitor, strInputDeviceID, 512 );

    HRESULT hr;
    bool bGotMemory = false;
    HRESULT hrCoInitialize = S_OK;
    IWbemLocator* pIWbemLocator = NULL;
    IWbemServices* pIWbemServices = NULL;
    BSTR pNamespace = NULL;

    *pdwAdapterRam = 0;

    hrCoInitialize = CoInitializeEx( 0, COINIT_APARTMENTTHREADED );

    hr = CoCreateInstance( CLSID_WbemLocator,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IWbemLocator,
                           ( LPVOID* )&pIWbemLocator );
#ifdef PRINTF_DEBUGGING
    if( FAILED( hr ) ) wprintf( L"WMI: CoCreateInstance failed: 0x%0.8x\n", hr );
#endif

    if( SUCCEEDED( hr ) && pIWbemLocator )
    {
        // Using the locator, connect to WMI in the given namespace.
        pNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );

        hr = pIWbemLocator->ConnectServer( pNamespace, NULL, NULL, 0L,
                                           0L, NULL, NULL, &pIWbemServices );
#ifdef PRINTF_DEBUGGING
        if( FAILED( hr ) ) wprintf( L"WMI: pIWbemLocator->ConnectServer failed: 0x%0.8x\n", hr );
#endif
        if( SUCCEEDED( hr ) && pIWbemServices != NULL )
        {
            HINSTANCE hinstOle32 = NULL;

            hinstOle32 = LoadLibraryW( L"ole32.dll" );
            if( hinstOle32 )
            {
                PfnCoSetProxyBlanket pfnCoSetProxyBlanket = NULL;

                pfnCoSetProxyBlanket = ( PfnCoSetProxyBlanket )GetProcAddress( hinstOle32, "CoSetProxyBlanket" );
                if( pfnCoSetProxyBlanket != NULL )
                {
                    // Switch security level to IMPERSONATE. 
                    pfnCoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );
                }

                FreeLibrary( hinstOle32 );
            }

            IEnumWbemClassObject* pEnumVideoControllers = NULL;
            BSTR pClassName = NULL;

            pClassName = SysAllocString( L"Win32_VideoController" );

            hr = pIWbemServices->CreateInstanceEnum( pClassName, 0,
                                                     NULL, &pEnumVideoControllers );
#ifdef PRINTF_DEBUGGING
            if( FAILED( hr ) ) wprintf( L"WMI: pIWbemServices->CreateInstanceEnum failed: 0x%0.8x\n", hr );
#endif

            if( SUCCEEDED( hr ) && pEnumVideoControllers )
            {
                IWbemClassObject* pVideoControllers[10] = {0};
                DWORD uReturned = 0;
                BSTR pPropName = NULL;

                // Get the first one in the list
                pEnumVideoControllers->Reset();
                hr = pEnumVideoControllers->Next( 5000,             // timeout in 5 seconds
                                                  10,                  // return the first 10
                                                  pVideoControllers,
                                                  &uReturned );
#ifdef PRINTF_DEBUGGING
                if( FAILED( hr ) ) wprintf( L"WMI: pEnumVideoControllers->Next failed: 0x%0.8x\n", hr );
                if( uReturned == 0 ) wprintf( L"WMI: pEnumVideoControllers uReturned == 0\n" );
#endif

                VARIANT var;
                if( SUCCEEDED( hr ) )
                {
                    bool bFound = false;
                    for( UINT iController = 0; iController < uReturned; iController++ )
                    {
                        pPropName = SysAllocString( L"PNPDeviceID" );
                        hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );
#ifdef PRINTF_DEBUGGING
                        if( FAILED( hr ) )
                            wprintf( L"WMI: pVideoControllers[iController]->Get PNPDeviceID failed: 0x%0.8x\n", hr );
#endif
                        if( SUCCEEDED( hr ) )
                        {
                            if( wcsstr( var.bstrVal, strInputDeviceID ) != 0 )
                                bFound = true;
                        }
                        VariantClear( &var );
                        if( pPropName ) SysFreeString( pPropName );

                        if( bFound )
                        {
                            pPropName = SysAllocString( L"AdapterRAM" );
                            hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );
#ifdef PRINTF_DEBUGGING
                            if( FAILED( hr ) )
                                wprintf( L"WMI: pVideoControllers[iController]->Get AdapterRAM failed: 0x%0.8x\n",
                                         hr );
#endif
                            if( SUCCEEDED( hr ) )
                            {
                                bGotMemory = true;
                                *pdwAdapterRam = var.ulVal;
                            }
                            VariantClear( &var );
                            if( pPropName ) SysFreeString( pPropName );
                            break;
                        }
                    }
                    for( UINT iController = 0; iController < uReturned; iController++ )
                        safe_release( pVideoControllers[iController] );
                }
            }

            if( pClassName )
                SysFreeString( pClassName );
            safe_release( pEnumVideoControllers );
        }

        if( pNamespace )
            SysFreeString( pNamespace );
        safe_release( pIWbemServices );
    }

    safe_release( pIWbemLocator );

    if( SUCCEEDED( hrCoInitialize ) )
        CoUninitialize();

    if( bGotMemory )
        return S_OK;
    else
        return E_FAIL;
}

typedef HRESULT ( WINAPI* LPDIRECTDRAWCREATE )( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );

//-----------------------------------------------------------------------------
HRESULT GetVideoMemoryViaDirectDraw( HMONITOR hMonitor, u64* pdwAvailableVidMem )
{
    LPDIRECTDRAW pDDraw = NULL;
    LPDIRECTDRAWENUMERATEEXA pDirectDrawEnumerateEx = NULL;
    HRESULT hr;
    bool bGotMemory = false;
    *pdwAvailableVidMem = 0;

    HINSTANCE hInstDDraw;
    LPDIRECTDRAWCREATE pDDCreate = NULL;

    hInstDDraw = LoadLibrary( "ddraw.dll" );
    if( hInstDDraw )
    {
        DDRAW_MATCH match;
        ZeroMemory( &match, sizeof( DDRAW_MATCH ) );
        match.hMonitor = hMonitor;

        pDirectDrawEnumerateEx = ( LPDIRECTDRAWENUMERATEEXA )GetProcAddress( hInstDDraw, "DirectDrawEnumerateExA" );

        if( pDirectDrawEnumerateEx )
        {
            hr = pDirectDrawEnumerateEx( DDEnumCallbackEx, ( VOID* )&match, DDENUM_ATTACHEDSECONDARYDEVICES );
        }

        pDDCreate = ( LPDIRECTDRAWCREATE )GetProcAddress( hInstDDraw, "DirectDrawCreate" );
        if( pDDCreate )
        {
            pDDCreate( &match.guid, &pDDraw, NULL );

            LPDIRECTDRAW7 pDDraw7;
            if( SUCCEEDED( pDDraw->QueryInterface( IID_IDirectDraw7, ( VOID** )&pDDraw7 ) ) )
            {
                DDSCAPS2 ddscaps;
                ZeroMemory( &ddscaps, sizeof( DDSCAPS2 ) );
                ddscaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
				DWORD temp;
                hr = pDDraw7->GetAvailableVidMem( &ddscaps, &temp, NULL );
				*pdwAvailableVidMem	= temp;
                if( SUCCEEDED( hr ) )
                    bGotMemory = true;
                pDDraw7->Release();
            }
        }
        FreeLibrary( hInstDDraw );
    }


    if( bGotMemory )
        return S_OK;
    else
        return E_FAIL;
}

static u64 guess_exact_memory_size			( u64 const value )
{
	u64 i		= 1;
	while ( i < value )
		i		*= 2;

	u64 const j	= i/2;
	u64 const k	= j/2;
	u64 const m	= k/2;
	if ( j + m >= value )
		return	j + m;

	if ( j + k >= value )
		return	j + k;

	return		i;
}

static u64 get_local_video_memory_size_impl	( )
{
    HRESULT const hrCoInitialize = CoInitializeEx( 0, COINIT_APARTMENTTHREADED );

    IDirect3D9* pD3D9 = NULL;

    pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( !pD3D9 ) {
		if( SUCCEEDED( hrCoInitialize ) )
			CoUninitialize();

		return				0;
	}

    UINT dwAdapterCount = pD3D9->GetAdapterCount();
	u64 maximum_video_memory_size	= 0;
    for( UINT iAdapter = 0; iAdapter < dwAdapterCount; iAdapter++ )
    {
        D3DADAPTER_IDENTIFIER9 id;
        ZeroMemory( &id, sizeof( D3DADAPTER_IDENTIFIER9 ) );
        pD3D9->GetAdapterIdentifier( iAdapter, 0, &id );

        HMONITOR hMonitor = pD3D9->GetAdapterMonitor( iAdapter );

        MONITORINFOEX mi;
        mi.cbSize = sizeof( MONITORINFOEX );
        GetMonitorInfo( hMonitor, &mi );

		u64 dwAdapterRAM;
		if( SUCCEEDED( GetVideoMemoryViaWMI( hMonitor, &dwAdapterRAM ) ) ) {
			maximum_video_memory_size	= xray::math::max( maximum_video_memory_size, guess_exact_memory_size(dwAdapterRAM) );
			continue;
		}

        u64 dwAvailableVidMem;
        if( SUCCEEDED( GetVideoMemoryViaDirectDraw( hMonitor, &dwAvailableVidMem ) ) )
			maximum_video_memory_size	= xray::math::max( maximum_video_memory_size, guess_exact_memory_size(dwAvailableVidMem) );
    }

    safe_release		( pD3D9 );

	if( SUCCEEDED( hrCoInitialize ) )
		CoUninitialize	( );

	return				maximum_video_memory_size;
}

u64 xray::platform::get_local_video_memory_size	( )
{
	u64 const result	= get_local_video_memory_size_impl( );
	unload_delay_loaded_library	( "d3d9.dll" );

	if ( !result )
		return			get_minimal_local_video_memory_size();

	CHECK_OR_EXIT		( result >= get_minimal_local_video_memory_size(), "X-Ray Engine v2.0 doesn't support video cards with onboard memory less than 256 Mb, please upgrade your hardware." );
	return				result;
}

u64 xray::platform::get_minimal_local_video_memory_size	( )
{
	return				256*Mb;
}