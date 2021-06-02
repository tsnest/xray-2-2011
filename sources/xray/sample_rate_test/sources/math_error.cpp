////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#ifndef MASTER_GOLD
int _matherr	( struct _exception *exception )
{
	return		( xray::debug::on_math_error( exception ) );
}
#endif // #ifndef MASTER_GOLD