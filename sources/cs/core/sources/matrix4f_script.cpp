////////////////////////////////////////////////////////////////////////////
//	Module 		: matrix4f_script.cpp
//	Created 	: 12.10.2006
//  Modified 	: 12.10.2006
//	Author		: Dmitriy Iassenev
//	Description : float matrix with 4x4 components class script export
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include <cs/core/matrix4f.h>

static vec4f* get_i(matrix4f* self)
{
	return	&self->i;
}

static vec4f* get_j(matrix4f* self)
{
	return	&self->j;
}

static vec4f* get_k(matrix4f* self)
{
	return	&self->k;
}

static vec4f* get_c(matrix4f* self)
{
	return	&self->c;
}

static void set_i(matrix4f* self, vec4f* object)
{
	self->i	= *object;
//	return	&self->i;
}

static void set_j(matrix4f* self, vec4f* object)
{
	self->j	= *object;
//	return	&self->j;
}

static void set_k(matrix4f* self, vec4f* object)
{
	self->k	= *object;
//	return	&self->k;
}

static void set_c(matrix4f* self, vec4f* object)
{
	self->c	= *object;
//	return	&self->c;
}

#pragma optimize("s",on)
void matrix4f::script_export	(lua_State* state)
{
	module(state)
	[
		namespace_("cs")[namespace_("core")[namespace_("classes")[
			class_<matrix4f>("matrix4f")
				.def(constructor<>())
				.property("i",	&get_i, &set_i)
				.property("j",	&get_j,	&set_j)
				.property("k",	&get_k,	&set_k)
				.property("c",	&get_c,	&set_c)
		]]]
	];
}
