////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

bool is_batch_mode()
{
	return (MGlobal::kInteractive != MGlobal::mayaState());
}

void display_info( pcstr str)
{
	MGlobal::displayInfo	( str );
	if(!is_batch_mode())
		LOG_INFO			( str );
}

void display_info( MString str)
{
	display_info( str.asChar() );
}

void display_warning( pcstr str)
{
	MGlobal::displayWarning	( str );
	if(!is_batch_mode())
		LOG_WARNING				( str );
}

void display_warning( MString str)
{
	display_warning( str.asChar() );
}

void display_error( MString str)
{
	MGlobal::displayWarning	( str );
	if(!is_batch_mode())
		LOG_ERROR			( str.asChar() );
}

