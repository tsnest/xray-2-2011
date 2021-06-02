////////////////////////////////////////////////////////////////////////////
//	Module 		: vec2f_script.cpp
//	Created 	: 13.08.2007
//  Modified 	: 13.08.2007
//	Author		: Dmitriy Iassenev
//	Description : 2D float vector class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"

#pragma optimize("s",on)
void vec2f::script_export	(lua_State* state)
{
	module(state)
	[
		namespace_("cs")[namespace_("core")[namespace_("classes")[
			class_<vec2f>("vec2f")
				.def(constructor<>())
				.def_readwrite("x",					&vec2f::x)
				.def_readwrite("y",					&vec2f::y)
				.def("set",	(vec2f& (vec2f::*)(float const , float const ))(&vec2f::set),return_reference_to(_1))
				.def("set",	(vec2f& (vec2f::*)(vec2f const&))(&vec2f::set),return_reference_to(_1))
				.def("add",	(vec2f& (vec2f::*)(vec2f const&))(&vec2f::add),return_reference_to(_1))
				.def("add",	(vec2f& (vec2f::*)(float const ))(&vec2f::add),return_reference_to(_1))
				.def("add",	(vec2f& (vec2f::*)(vec2f const&,vec2f const&))(&vec2f::add),return_reference_to(_1))
				.def("add",	(vec2f& (vec2f::*)(vec2f const&, float const ))(&vec2f::add),return_reference_to(_1))
				.def("sub",	(vec2f& (vec2f::*)(vec2f const&))(&vec2f::sub),return_reference_to(_1))
				.def("sub",	(vec2f& (vec2f::*)(float const))(&vec2f::sub),return_reference_to(_1))
				.def("sub",	(vec2f& (vec2f::*)(vec2f const&,vec2f const&))(&vec2f::sub),return_reference_to(_1))
				.def("sub",	(vec2f& (vec2f::*)(vec2f const&, float const ))(&vec2f::sub),return_reference_to(_1))
				.def("mul",	(vec2f& (vec2f::*)(vec2f const&))(&vec2f::mul),return_reference_to(_1))
				.def("mul",	(vec2f& (vec2f::*)(float const))(&vec2f::mul),return_reference_to(_1))
				.def("mul",	(vec2f& (vec2f::*)(vec2f const&,vec2f const&))(&vec2f::mul),return_reference_to(_1))
				.def("mul",	(vec2f& (vec2f::*)(vec2f const&,float const))(&vec2f::mul),return_reference_to(_1))
				.def("div",	(vec2f& (vec2f::*)(vec2f const&))(&vec2f::div),return_reference_to(_1))
				.def("div",	(vec2f& (vec2f::*)(float const))(&vec2f::div),return_reference_to(_1))
				.def("div",	(vec2f& (vec2f::*)(vec2f const&,vec2f const&))(&vec2f::div),return_reference_to(_1))
				.def("div",	(vec2f& (vec2f::*)(vec2f const&,float const))(&vec2f::div),return_reference_to(_1))
				.def("mad",	(vec2f& (vec2f::*)(vec2f const&,float const))(&vec2f::mad),return_reference_to(_1))
				.def("mad",	(vec2f& (vec2f::*)(vec2f const&,vec2f const&, float const ))(&vec2f::mad),return_reference_to(_1))
				.def("mad",	(vec2f& (vec2f::*)(vec2f const&,vec2f const&))(&vec2f::mad),return_reference_to(_1))
				.def("mad",	(vec2f& (vec2f::*)(vec2f const&,vec2f const&,vec2f const&))(&vec2f::mad),return_reference_to(_1))
				.def("similar",		(bool (vec2f::*)(vec2f const&, float const ))(&vec2f::similar))
				.def("magnitude_sqr",(float (vec2f::*)()const)(&vec2f::magnitude_sqr))
				.def("magnitude",	(float (vec2f::*)()const)(&vec2f::magnitude))
				.def("dot_product",		(float (vec2f::*)(vec2f const&))(&vec2f::dot_product))
				.def("cross_product",		(vec2f& (vec2f::*)(vec2f const&))(&vec2f::cross_product),return_reference_to(_1))
				.def("distance_to_sqr",	(float (vec2f::*)(vec2f const&)const)(&vec2f::distance_to_sqr))
				.def("distance_to",	(float (vec2f::*)(vec2f const&)const)(&vec2f::distance_to))
				.def("geth",		(float (vec2f::*)())(&vec2f::geth))
		]]]
	];
}
