////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_vec3f_reference.h"

using namespace System;
using System::Windows::Media::Media3D::Vector3D;

property_vec3f_reference::property_vec3f_reference	( float3& value	) :
	m_value						(NEW(value_holder<float3>)(value))
{
}

property_vec3f_reference::~property_vec3f_reference	()
{
	this->!property_vec3f_reference	();
}

property_vec3f_reference::!property_vec3f_reference	()
{
	DELETE							(m_value);
}

System::Object^		property_vec3f_reference::get_value		()
{
	float3 vec		= m_value->get();
	return Vector3D	(vec.x, vec.y, vec.z);
}

void				property_vec3f_reference::set_value		(System::Object^ value)
{
	Vector3D vec	= (Vector3D)value;
	m_value->set	(float3((Single)vec.X, (Single)vec.Y, (Single)vec.Z));
}