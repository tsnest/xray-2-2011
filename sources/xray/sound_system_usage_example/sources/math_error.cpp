////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef MASTER_GOLD
int _matherr	( struct _exception *exception )
{
	return		( xray::debug::on_math_error( exception ) );
}
#endif // #ifndef MASTER_GOLD