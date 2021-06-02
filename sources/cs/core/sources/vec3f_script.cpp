////////////////////////////////////////////////////////////////////////////
//	Module 		: vec3f_script.cpp
//	Created 	: 23.07.2005
//  Modified 	: 23.07.2005
//	Author		: Dmitriy Iassenev
//	Description : 3D float vector class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include <cs/gtl/random32.h>

static vec3f&	random_dir0		(vec3f& v)
{
	static gtl::random32 random;
	v.x		= random.random_f();
	v.y		= random.random_f();
	v.z		= random.random_f();
	return	v;
}

//static vec3f& random_dir1	(vec3f& v,vec3f const& cone_axis, float cone_angle)
//{
//	return	v.random_dir(cone_axis,cone_angle);
//}

//static vec3f&	random_point_box(vec3f& v,vec3f const& box_size)
//{
//	return	v.random_point_box(box_size);
//}

//static vec3f&	random_point_sphere(vec3f& v, float r)
//{
//	return	v.random_point_sphere(r);
//}

#pragma optimize("s",on)
void vec3f::script_export	(lua_State* state)
{
	module(state)
	[
		namespace_("cs")[namespace_("core")[namespace_("classes")[
			class_<vec3f>("vec3f")
				.def(constructor<>())
				.def_readwrite("x",					&vec3f::x)
				.def_readwrite("y",					&vec3f::y)
				.def_readwrite("z",					&vec3f::z)
				.def("set",	(vec3f& (vec3f::*)(float const , float const , float const ))(&vec3f::set),return_reference_to(_1))
				.def("set",	(vec3f& (vec3f::*)(vec3f const&))(&vec3f::set),return_reference_to(_1))
				.def("add",	(vec3f& (vec3f::*)(vec3f const&))(&vec3f::add),return_reference_to(_1))
				.def("add",	(vec3f& (vec3f::*)(float const ))(&vec3f::add),return_reference_to(_1))
				.def("add",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::add),return_reference_to(_1))
				.def("add",	(vec3f& (vec3f::*)(vec3f const&, float const ))(&vec3f::add),return_reference_to(_1))
				.def("sub",	(vec3f& (vec3f::*)(vec3f const&))(&vec3f::sub),return_reference_to(_1))
				.def("sub",	(vec3f& (vec3f::*)(float const))(&vec3f::sub),return_reference_to(_1))
				.def("sub",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::sub),return_reference_to(_1))
				.def("sub",	(vec3f& (vec3f::*)(vec3f const&, float const ))(&vec3f::sub),return_reference_to(_1))
				.def("mul",	(vec3f& (vec3f::*)(vec3f const&))(&vec3f::mul),return_reference_to(_1))
				.def("mul",	(vec3f& (vec3f::*)(float const))(&vec3f::mul),return_reference_to(_1))
				.def("mul",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::mul),return_reference_to(_1))
				.def("mul",	(vec3f& (vec3f::*)(vec3f const&,float const))(&vec3f::mul),return_reference_to(_1))
				.def("div",	(vec3f& (vec3f::*)(vec3f const&))(&vec3f::div),return_reference_to(_1))
				.def("div",	(vec3f& (vec3f::*)(float const))(&vec3f::div),return_reference_to(_1))
				.def("div",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::div),return_reference_to(_1))
				.def("div",	(vec3f& (vec3f::*)(vec3f const&,float const))(&vec3f::div),return_reference_to(_1))
				.def("mad",	(vec3f& (vec3f::*)(vec3f const&,float const))(&vec3f::mad),return_reference_to(_1))
				.def("mad",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&, float const ))(&vec3f::mad),return_reference_to(_1))
				.def("mad",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::mad),return_reference_to(_1))
				.def("mad",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&,vec3f const&))(&vec3f::mad),return_reference_to(_1))
//				.def("negate",		(vec3f& (vec3f::*)())(&vec3f::negate),return_reference_to(_1))
//				.def("negate",		(vec3f& (vec3f::*)(vec3f const&))(&vec3f::negate),return_reference_to(_1))
				.def("min",			(vec3f& (vec3f::*)(vec3f const&))(&vec3f::min),return_reference_to(_1))
//				.def("min",			(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::min),return_reference_to(_1))
				.def("max",			(vec3f& (vec3f::*)(vec3f const&))(&vec3f::max),return_reference_to(_1))
//				.def("max",			(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::max),return_reference_to(_1))
				.def("abs",			(vec3f& (vec3f::*)(vec3f const&))(&vec3f::abs),return_reference_to(_1))
				.def("similar",		(bool (vec3f::*)(vec3f const&, float const ) const)(&vec3f::similar))
				.def("similar",		(bool (vec3f::*)(vec3f const&, vec3f const& ) const)(&vec3f::similar))
				.def("clamp",		(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::clamp),return_reference_to(_1))
//				.def("lerp",		(vec3f& (vec3f::*)(vec3f const&,vec3f const&, float const ))(&vec3f::lerp),return_reference_to(_1))
				.def("magnitude_sqr",(float (vec3f::*)()const)(&vec3f::magnitude_sqr))
				.def("magnitude",	(float (vec3f::*)()const)(&vec3f::magnitude))
//				.def("normalize_magn",	(float (vec3f::*)())(&vec3f::normalize_magn))
				.def("normalize",	(vec3f& (vec3f::*)())(&vec3f::normalize_safe),return_reference_to(_1))
				.def("normalize",	(vec3f& (vec3f::*)(vec3f const&))(&vec3f::normalize_safe),return_reference_to(_1))
				.def("dot_product",		(float (vec3f::*)(vec3f const&))(&vec3f::dot_product))
				.def("cross_product",		(vec3f& (vec3f::*)(vec3f const&))(&vec3f::cross_product),return_reference_to(_1))
				.def("distance_to_sqr",	(float (vec3f::*)(vec3f const&)const)(&vec3f::distance_to_sqr))
				.def("distance_to",	(float (vec3f::*)(vec3f const&)const)(&vec3f::distance_to))
//				.def("from_bary",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&,vec3f const&, float const , float const , float const ))(&vec3f::from_bary),return_reference_to(_1))
//				.def("from_bary",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&,vec3f const&,vec3f const&))(&vec3f::from_bary),return_reference_to(_1))
//				.def("from_bary",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&,vec3f const&,vec3f const&, float const , float const , float const , float const ))(&vec3f::from_bary4),return_reference_to(_1))
//				.def("mknormal_non_normalized",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&,vec3f const&))(&vec3f::mknormal_non_normalized),return_reference_to(_1))
//				.def("mknormal",	(vec3f& (vec3f::*)(vec3f const&,vec3f const&,vec3f const&))(&vec3f::mknormal),return_reference_to(_1))
				.def("set_hp",		(vec3f& (vec3f::*)(float const , float const ))(&vec3f::set_angles_hp),return_reference_to(_1))
				.def("get_hp",		&vec3f::get_angles_hp,out_value(_2) + out_value(_3))
				.def("geth",		(float (vec3f::*)())(&vec3f::get_angles_h))
				.def("getp",		(float (vec3f::*)())(&vec3f::get_angles_p))
//				.def("reflect",		(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::reflect),return_reference_to(_1))
//				.def("slide",		(vec3f& (vec3f::*)(vec3f const&,vec3f const&))(&vec3f::slide),return_reference_to(_1))
				.def("random_dir",	&random_dir0,return_reference_to(_1))
//				.def("random_dir",	&random_dir1,return_reference_to(_1))
//				.def("random_point_box",&random_point_box,return_reference_to(_1))
//				.def("random_dir",	&random_point_sphere,return_reference_to(_1))
		]]]
	];
}
