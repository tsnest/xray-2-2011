////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/os_include.h>

u64 windows_query_performance_counter	()
{
	u64						result;
	QueryPerformanceCounter	((PLARGE_INTEGER)&result);
	return					(result);
}
