////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef MASTER_GOLD
int _matherr	( struct _exception *exception )
{
	return		( xray::debug::on_math_error( exception ) );
}
#endif // #ifndef MASTER_GOLD