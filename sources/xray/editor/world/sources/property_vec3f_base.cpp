////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_vec3f_base.h"
#include "property_container.h"
#include "property_float_limited.h"

using System::Object;
using System::String;
using Flobbster::Windows::Forms::PropertySpec;
using System::Collections::DictionaryEntry;

ref class property_converter_float;

vec3f_components::vec3f_components			(property_vec3f_base^ holder) :
	m_holder			(holder)
{
}

float vec3f_components::x_getter			()
{
	return				(m_holder->get_value_raw().x);
}

void vec3f_components::x_setter			(float value)
{
	m_holder->x			(value);
}

float vec3f_components::y_getter			()
{
	return				(m_holder->get_value_raw().y);
}

void vec3f_components::y_setter			(float value)
{
	m_holder->y			(value);
}

float vec3f_components::z_getter			()
{
	return				(m_holder->get_value_raw().z);
}

void vec3f_components::z_setter				(float value)
{
	m_holder->z			(value);
}

property_vec3f_base::property_vec3f_base	(float3 const% vec3f, 
											 property_holder::readonly_enum read_only,
											 property_holder::notify_parent_on_change_enum notify_parent,
											 property_holder::password_char_enum password,
											 property_holder::refresh_grid_on_change_enum refresh_grid)

{
	XRAY_UNREFERENCED_PARAMETER		( notify_parent );
	m_container			= gcnew property_container_Vec3f(nullptr, this);
	m_components		= gcnew vec3f_components(this);

	m_container->add_property	(
		gcnew PropertySpec(
			"x",
			float::typeid,
			"components",
			"X component",
			vec3f.x,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float(
			gcnew float_getter_type(m_components, &vec3f_components::x_getter),
			gcnew float_setter_type(m_components, &vec3f_components::x_setter),
			.01f
		),
		read_only,
		property_holder::notify_parent_on_change,
		password,
		refresh_grid
	);

	m_container->add_property	(
		gcnew PropertySpec(
			"y",
			float::typeid,
			"components",
			"Y component",
			vec3f.y,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float(
			gcnew float_getter_type(m_components, &vec3f_components::y_getter),
			gcnew float_setter_type(m_components, &vec3f_components::y_setter),
			.01f
		),
		read_only,
		property_holder::notify_parent_on_change,
		password,
		refresh_grid 
	);

	m_container->add_property	(
		gcnew PropertySpec(
			"z",
			float::typeid,
			"components",
			"Z component",
			vec3f.z,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float(
			gcnew float_getter_type(m_components, &vec3f_components::z_getter),
			gcnew float_setter_type(m_components, &vec3f_components::z_setter),
			.01f
		),
		read_only,
		property_holder::notify_parent_on_change,
		password,
		refresh_grid
	);
}

property_vec3f_base::~property_vec3f_base	()
{
	this->!property_vec3f_base();
}

property_vec3f_base::!property_vec3f_base	()
{
	delete				(m_container);
}

Object^ property_vec3f_base::get_value		()
{
	return				(m_container);
}

void property_vec3f_base::set_value			(Object ^object)
{
	Vec3f				vec3f = safe_cast<Vec3f>(object);
	float3				value;
	value.x				= vec3f.x;
	value.y				= vec3f.y;
	value.z				= vec3f.z;
	set_value_raw		(value);
}

void property_vec3f_base::x					(float value)
{
	float3 current		= get_value_raw();
	current.x			= value;
	set_value_raw		(current);
}

void property_vec3f_base::y					(float value)
{
	float3 current		= get_value_raw();
	current.y			= value;
	set_value_raw		(current);
}

void property_vec3f_base::z					(float value)
{
	float3 current		= get_value_raw();
	current.z			= value;
	set_value_raw		(current);
}


//vec2f
vec2f_components::vec2f_components			(property_vec2f_base^ holder) :
	m_holder			(holder)
{
}

float vec2f_components::x_getter			()
{
	return				(m_holder->get_value_raw().x);
}

void vec2f_components::x_setter			(float value)
{
	m_holder->x			(value);
}

float vec2f_components::y_getter			()
{
	return				(m_holder->get_value_raw().y);
}

void vec2f_components::y_setter			(float value)
{
	m_holder->y			(value);
}

property_vec2f_base::property_vec2f_base	(float2 const% vec2f, 
											 property_holder::readonly_enum read_only,
											 property_holder::notify_parent_on_change_enum notify_parent,
											 property_holder::password_char_enum password,
											 property_holder::refresh_grid_on_change_enum refresh_grid)

{
	XRAY_UNREFERENCED_PARAMETER		( notify_parent );
	m_container			= gcnew property_container_Vec2f(nullptr, this);
	m_components		= gcnew vec2f_components(this);

	m_container->add_property	(
		gcnew PropertySpec(
			"x",
			float::typeid,
			"components",
			"X component",
			vec2f.x,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float(
			gcnew float_getter_type(m_components, &vec2f_components::x_getter),
			gcnew float_setter_type(m_components, &vec2f_components::x_setter),
			.01f
		),
		read_only,
		property_holder::notify_parent_on_change,
		password,
		refresh_grid
	);

	m_container->add_property	(
		gcnew PropertySpec(
			"y",
			float::typeid,
			"components",
			"Y component",
			vec2f.y,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float(
			gcnew float_getter_type(m_components, &vec2f_components::y_getter),
			gcnew float_setter_type(m_components, &vec2f_components::y_setter),
			.01f
		),
		read_only,
		property_holder::notify_parent_on_change,
		password,
		refresh_grid 
	);
}

property_vec2f_base::~property_vec2f_base	()
{
	this->!property_vec2f_base();
}

property_vec2f_base::!property_vec2f_base	()
{
	delete				(m_container);
}

Object^ property_vec2f_base::get_value		()
{
	return				(m_container);
}

void property_vec2f_base::set_value			(Object ^object)
{
	Vec2f				vec2f = safe_cast<Vec2f>(object);
	float2				value;
	value.x				= vec2f.x;
	value.y				= vec2f.y;
	set_value_raw		(value);
}

void property_vec2f_base::x					(float value)
{
	float2 current		= get_value_raw();
	current.x			= value;
	set_value_raw		(current);
}

void property_vec2f_base::y					(float value)
{
	float2 current		= get_value_raw();
	current.y			= value;
	set_value_raw		(current);
}