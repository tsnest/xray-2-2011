////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/linkage_helper.h>

DECLARE_LINKAGE_ID(core_debug_platform)

int xray::debug::on_math_error		( struct ::_exception *exception )
{
	pcstr type				= "<no_description>";
	switch ( exception->type ) {
		case _DOMAIN : {
			type			= "Argument domain error";
			break;
		}
		case _SING : {
			type			= "Argument singulatiry";
			break;
		}
		case _OVERFLOW : {
			type			= "Overflow range error";
			break;
		}
		case _PLOSS : {
			type			= "Partial loss of significance";
			break;
		}
		case _TLOSS : {
			type			= "Total loss of significance";
			break;
		}
		case _UNDERFLOW : {
			return			1;
//			type			= "The result is too small to be represented";
//			break;
		}
		default : NODEFAULT();
	}
	
	LOG_INFO				( "error[%s] in function %s( %f [, %f] )", type, exception->name, exception->arg1, exception->arg2 );
	dump_call_stack			( "debug", xray::logging::error, 3, 0, 0 );
	DEBUG_BREAK				();
    return					( 0 );    /* Else use the default actions */
}