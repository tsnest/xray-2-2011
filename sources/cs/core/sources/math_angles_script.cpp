////////////////////////////////////////////////////////////////////////////
//	Module 		: math_angles_script.cpp
//	Created 	: 13.08.2007
//  Modified 	: 13.08.2007
//	Author		: Dmitriy Iassenev
//	Description : angles functions script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"

#pragma optimize("s",on)
void gtl__class_script_exporter_dummy_::script_export	(lua_State* state)
{
	module(state)
	[
		namespace_("cs")[namespace_("core")[
			def("angle_difference",			&angle_difference),
			def("angle_normalize_signed",	&angle_normalize_signed)
		]]
	];
}