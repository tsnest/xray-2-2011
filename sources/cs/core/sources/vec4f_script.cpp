////////////////////////////////////////////////////////////////////////////
//	Module 		: vec4f_script.cpp
//	Created 	: 23.07.2005
//  Modified 	: 23.07.2005
//	Author		: Dmitriy Iassenev
//	Description : 4D float vector class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"

static vec4f& set	(vec4f* self, float x, float y, float z, float w)
{
	return	self->set(x,y,z,w);
}

#pragma optimize("s",on)
void vec4f::script_export	(lua_State* state)
{
	module(state)
	[
		namespace_("cs")[namespace_("core")[namespace_("classes")[
			class_<vec4f>("vec4f")
				.def(constructor<>())
				.def_readwrite("x",	&vec4f::x)
				.def_readwrite("y",	&vec4f::y)
				.def_readwrite("z",	&vec4f::z)
				.def_readwrite("w",	&vec4f::w)
				.def("set",	&::set,return_reference_to(_1))
		]]]
	];
}
