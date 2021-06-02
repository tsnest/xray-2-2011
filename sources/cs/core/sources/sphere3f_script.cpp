////////////////////////////////////////////////////////////////////////////
//	Module 		: sphere3f_script.cpp
//	Created 	: 23.07.2005
//  Modified 	: 23.07.2005
//	Author		: Dmitriy Iassenev
//	Description : 3D sphere class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"

#pragma optimize("s",on)
void sphere3f::script_export(lua_State* state)
{
	module(state)
	[
		namespace_("cs")[namespace_("core")[namespace_("classes")[
			class_<sphere3f>("sphere3f")
				.def(constructor<>())
				.def("set",			(sphere3f& (sphere3f::*)(float const , float const , float const , float const ))&sphere3f::set)
				.def("set",			(sphere3f& (sphere3f::*)(vec3f const& , float const ))&sphere3f::set)
				.def("intersect",	&sphere3f::intersect)
		]]]
	];
}
