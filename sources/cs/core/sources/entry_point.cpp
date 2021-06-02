////////////////////////////////////////////////////////////////////////////
//	Module 		: entry_point.cpp
//	Created 	: 23.02.2008
//  Modified 	: 23.02.2008
//	Author		: Dmitriy Iassenev
//	Description : library entry point
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

void cs_core_cleanup		();

BOOL APIENTRY DllMain		(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
	CS_UNREFERENCED_PARAMETER	( hModule );
	CS_UNREFERENCED_PARAMETER	( lpReserved );

	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			break;
		}
		case DLL_PROCESS_DETACH: {
			cs_core_cleanup	();
			break;
		}
	}
	return					TRUE;
}