////////////////////////////////////////////////////////////////////////////
//	Module 		: script.cpp
//	Created 	: 15.06.2005
//  Modified 	: 15.06.2005
//	Author		: Dmitriy Iassenev
//	Description : library entry point
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef CS_STATIC_LIBRARIES
namespace cs {
namespace script {
namespace detail {
	void cleanup		();
} // namespace detail
} // namespace script
} // namespace cs

#ifdef LUABIND_NO_EXCEPTIONS
void boost::throw_exception(std::exception const& exception)
{
	FATAL( "boost::throw_exception: %s", exception.what() );
}
#endif // #ifdef LUABIND_NO_EXCEPTIONS

BOOL APIENTRY DllMain	(HANDLE hModule, u32 ul_reason_for_call, LPVOID lpReserved)
{
	CS_UNREFERENCED_PARAMETER	( hModule );
	CS_UNREFERENCED_PARAMETER	( lpReserved );

	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			break;
		}
		case DLL_PROCESS_DETACH: {
			cs::script::detail::cleanup	();
			break;
		}
	}
	return				TRUE;
}

#endif // #ifndef CS_STATIC_LIBRARIES