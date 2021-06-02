////////////////////////////////////////////////////////////////////////////
//	Created		: 25.03.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/core/pix_event_wrapper.h>

#ifdef DEBUG

// using DX9 functions for PIX events
#include "d3d9.h"

pix_event_wrapper_dx11::pix_event_wrapper_dx11(LPCWSTR wszName)	
{	
	D3DPERF_BeginEvent	( D3DCOLOR_RGBA(127,0,0,255), wszName );
}

pix_event_wrapper_dx11::~pix_event_wrapper_dx11()				
{	
	D3DPERF_EndEvent	();	
}

#endif // #ifdef DEBUG