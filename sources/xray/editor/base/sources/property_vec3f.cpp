////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_vec3f.h"

using namespace System;

using System::Windows::Media::Media3D::Vector3D;
using System::Windows::Vector;

property_vec3f::property_vec3f		(
		float3_getter_type^ getter,
		float3_setter_type^ setter	) :
	m_getter				( getter ),
	m_setter				( setter )
{
}

property_vec3f::property_vec3f		(
		Object^ obj,
		PropertyInfo^ prop_info)
{
	m_getter = safe_cast<float3_getter_type^>(System::Delegate::CreateDelegate(float3_getter_type::typeid, obj, prop_info->GetGetMethod()));
	m_setter = safe_cast<float3_setter_type^>(System::Delegate::CreateDelegate(float3_setter_type::typeid, obj, prop_info->GetSetMethod()));
}

System::Object^ property_vec3f::get_value	()
{
	Float3 vec		= m_getter();
	return Vector3D	(vec.x, vec.y, vec.z);
}

void property_vec3f::set_value	(System::Object^ value)
{
	Vector3D vec	= (Vector3D)value;
 	m_setter		(Float3(float3((Single)vec.X, (Single)vec.Y, (Single)vec.Z)));
}

//vec2f
property_vec2f::property_vec2f		(
		float2_getter_type^ getter,
		float2_setter_type^ setter
		) :

	m_getter				(getter),
	m_setter				(setter)
{
}

property_vec2f::property_vec2f		(
		Object^ obj,
		PropertyInfo^ prop_info
		)
{
	m_getter =	safe_cast<float2_getter_type^>(System::Delegate::CreateDelegate(float2_getter_type::typeid, obj, prop_info->GetGetMethod()));
	m_setter =	safe_cast<float2_setter_type^>(System::Delegate::CreateDelegate(float2_setter_type::typeid, obj, prop_info->GetSetMethod()));
}

System::Object^ property_vec2f::get_value	()
{
	Float2 vec		= m_getter();
	return Vector	(vec.x, vec.y);
}

void property_vec2f::set_value	(System::Object^ value)
{
	Vector vec		= (Vector)value;
	m_setter		(Float2(float2((Single)vec.X, (Single)vec.Y)));
}