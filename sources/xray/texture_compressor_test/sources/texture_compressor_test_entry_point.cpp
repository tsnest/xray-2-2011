////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_compressor_test_application.h"
#include <xray/os_include.h>

#pragma comment( lib, "delayimp.lib" )

static xray::uninitialized_reference<xray::texture_compressor_test::application>	s_application;

static int s_exit_code			= 0;

struct guard {
	inline	guard				( )
	{
		XRAY_CONSTRUCT_REFERENCE( s_application, xray::texture_compressor_test::application );
		s_application->initialize ( );
	}

	inline	~guard				( )
	{
		s_exit_code				= s_application->get_exit_code();
		s_application->finalize	( );
		XRAY_DESTROY_REFERENCE	( s_application );
	}
}; // struct guard

static void	main_protected		( pvoid )
{
	guard						guard;

	if ( s_application->get_exit_code() )
		return;

#if _MSC_VER == 1500
	pcstr const library_name	= "msvcr90.dll";
#else // #if _MSC_VER == 1500
#	error define correct library name here
#endif // #if _MSC_VER == 1500

	typedef pvoid (*malloc_ptr_type)( size_t );
	typedef void (*free_ptr_type)( pvoid );
	typedef pvoid (*realloc_ptr_type)( pvoid, size_t );

	HMODULE const handle				= GetModuleHandle(library_name);
	R_ASSERT							( handle );

	malloc_ptr_type const crt_malloc	= (malloc_ptr_type)( GetProcAddress(handle, "malloc") );
	R_ASSERT							( crt_malloc );

	free_ptr_type const crt_free		= (free_ptr_type)( GetProcAddress(handle, "free") );
	R_ASSERT							( crt_free );

	realloc_ptr_type const crt_realloc	= (realloc_ptr_type)( GetProcAddress(handle, "realloc") );
	R_ASSERT							( crt_realloc );

	int* a = static_cast<int*>( (*crt_malloc)(4) );
	*a = 2;
	a = static_cast<int*>( (*crt_realloc)( a, 8 ) );
	*(u64*)a = 1;
	(*crt_free)( a );

	s_application->execute		( );
	if ( s_application->get_exit_code() )
		return;
}

int main						( int const argc, pstr const argv[ ], pstr const env[ ] )
{
	XRAY_UNREFERENCED_PARAMETERS( argc, argv, env );

	xray::debug::protected_call	( &main_protected, 0 );
	return						( s_exit_code );
}