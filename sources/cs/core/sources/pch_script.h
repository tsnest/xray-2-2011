////////////////////////////////////////////////////////////////////////////
//	Module 		: pch_script.h
//	Created 	: 20.05.2007
//  Modified 	: 20.05.2007
//	Author		: Dmitriy Iassenev
//	Description : precompiled header for scripts
////////////////////////////////////////////////////////////////////////////

#ifndef PCH_SCRIPT_H_INCLUDED
#define PCH_SCRIPT_H_INCLUDED

#include "pch.h"

#ifndef CS_NO_SCRIPTS
#	include <luabind/luabind.hpp>
#	include <luabind/return_reference_to_policy.hpp>
#	include <luabind/out_value_policy.hpp>
	
	using namespace luabind;
#endif // #ifndef CS_NO_SCRIPTS

#endif // #ifndef PCH_SCRIPT_H_INCLUDED