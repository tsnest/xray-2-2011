////////////////////////////////////////////////////////////////////////////
//	Module 		: aabb3f_script.cpp
//	Created 	: 14.04.2007
//  Modified 	: 14.04.2007
//	Author		: Dmitriy Iassenev
//	Description : 3D float axes aligned bounding box (AABB) class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"

#pragma optimize("s",on)
void aabb3f::script_export	(lua_State* state)
{
	module(state)
	[
		namespace_("cs")[namespace_("core")[namespace_("classes")[
			class_<aabb3f>("aabb3f")
				.def(constructor<>())
				.def_readwrite("min",	&aabb3f::min)
				.def_readwrite("max",	&aabb3f::max)
				.def("invalidate",		&aabb3f::invalidate,								return_reference_to(_1))
				.def("modify",			&aabb3f::modify,									return_reference_to(_1))
				.def("grow",			(aabb3f& (aabb3f::*)(vec3f const&))&aabb3f::grow,	return_reference_to(_1))
				.def("grow",			(aabb3f& (aabb3f::*)(float const))&aabb3f::grow,	return_reference_to(_1))
				.def("merge",			&aabb3f::merge,										return_reference_to(_1))
				.def("inside",			(bool (aabb3f::*)(aabb3f const&) const)&aabb3f::inside)
				.def("inside",			(bool (aabb3f::*)(vec3f const&, vec3f const&) const)&aabb3f::inside)
				.def("over",			(bool (aabb3f::*)(aabb3f const&) const)&aabb3f::over)
				.def("over",			(bool (aabb3f::*)(vec3f const&, vec3f const&) const)&aabb3f::over)
				.def("test",			(bool (aabb3f::*)(aabb3f const&) const)&aabb3f::test)
				.def("test",			(bool (aabb3f::*)(vec3f const&, vec3f const&) const)&aabb3f::test)
		]]]
	];
}
