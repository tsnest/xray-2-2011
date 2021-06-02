////////////////////////////////////////////////////////////////////////////
//	Module 		: script_export.cpp
//	Created 	: 25.09.2006
//  Modified 	: 25.09.2006
//	Author		: Dmitriy Iassenev
//	Description : script export
////////////////////////////////////////////////////////////////////////////
#include "internal.h"

#pragma warning(disable:4996)

#pragma optimize("s",on)

#define CS_SCRIPT_EXPORT_HEADER	"cs/core/sources/script_export.h"
#define CS_SCRIPT_USER				core
#include <cs/gtl/script_export.h>

#include <cs/core/core_extensions.h>

void CS_CORE_API cs::core::script_export	(lua_State* state)
{
	msg					(cs::message_initiator_core, cs::core::message_type_information,"exporting classes to script...");
	gtl::script_export	(state);
}