////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include <xray/macro_platform.h>

#if !XRAY_PLATFORM_PS3
#	include <malloc.h>
#else // #if !XRAY_PLATFORM_PS3
#	include <stdlib.h>
#endif // #if !XRAY_PLATFORM_PS3

void test_allocation()
{
//	char* buffer = new char[128];
	void* buffer = malloc(128);
}
