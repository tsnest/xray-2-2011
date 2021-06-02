////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container.h"
#include "property_float.h"
#include "property_float_reference.h"
#include "property_float_limited.h"
#include "property_float_limited_reference.h"
#include "property_float_enum_value.h"
#include "property_float_enum_value_reference.h"
#include "float_holder.h"

ref class property_converter_float_enum;
ref class property_converter_float;

using Flobbster::Windows::Forms::PropertySpec;
using System::String;


xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		xray::configs::lua_config_value const& config,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	xray::editor::float_holder^ f_holder = gcnew xray::editor::float_holder(identifier, config);
	return add_float		(
		identifier,
		category,
		description,
		default_value,
		gcnew float_getter_type(f_holder, &xray::editor::float_holder::get),
		gcnew float_setter_type(f_holder, &xray::editor::float_holder::set),
		read_only,
		notify_parent,
		password,
		refresh_grid);
}

xray::editor::property_value* property_holder::add_float				(
	System::String^ identifier,
	System::String^ category,
	System::String^ description,
	float const &default_value,
	xray::configs::lua_config_value const& config,
	float const &min_value,
	float const &max_value,
	readonly_enum const& read_only,
	notify_parent_on_change_enum const& notify_parent,
	password_char_enum const& password,
	refresh_grid_on_change_enum const& refresh_grid
	)
{
	xray::editor::float_holder^ f_holder = gcnew xray::editor::float_holder(identifier, config);
	return add_float		(
		identifier,
		category,
		description,
		default_value,
		gcnew float_getter_type(f_holder, &xray::editor::float_holder::get),
		gcnew float_setter_type(f_holder, &xray::editor::float_holder::set),
		min_value,
		max_value,
		read_only,
		notify_parent,
		password,
		refresh_grid);
}


xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		float_getter_type^ getter,
		float_setter_type^ setter,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			float::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float(
			getter,
			setter,
			.05f
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
/*
xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		float& value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			float::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float_reference(value, .05f),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
*/
xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		float_getter_type^ getter,
		float_setter_type^ setter,
		float const &min_value,
		float const &max_value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	PropertySpec^ spec = gcnew PropertySpec(
		identifier,
		float::typeid,
		category,
		description,
		default_value,
		(String^)nullptr,
		property_converter_float::typeid
	);

	array<System::Attribute^>^ attribute_array = gcnew array<System::Attribute^>(1);
	attribute_array[0] = gcnew xray::editor::wpf_controls::value_range_and_format_attribute(min_value, max_value, 0.1, 4);
	spec->Attributes = attribute_array;

	m_container->add_property	(
		spec,
		gcnew property_float_limited(
			getter,
			setter,
			(max_value - min_value)*.0025f,
			min_value,
			max_value
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}

xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		float_getter_type^ getter,
		float_setter_type^ setter,
		System::Func<System::Double>^ min_value_func,
		System::Func<System::Double>^ max_value_func,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	PropertySpec^ spec = gcnew PropertySpec(
		identifier,
		float::typeid,
		category,
		description,
		default_value,
		(String^)nullptr,
		property_converter_float::typeid
	);

	array<System::Attribute^>^ attribute_array = gcnew array<System::Attribute^>(1);
	attribute_array[0] = gcnew xray::editor::wpf_controls::value_range_and_format_attribute(min_value_func, max_value_func, 0.1, 4);
	spec->Attributes = attribute_array;

	m_container->add_property	(
		spec,
		gcnew property_float_limited(
			getter,
			setter,
			(float)(max_value_func() - min_value_func())*.0025f,
			(float)min_value_func(),
			(float)max_value_func()
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
/*
xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		float& value,
		float const &min_value,
		float const &max_value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			float::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_float::typeid
		),
		gcnew property_float_limited_reference(
			value,
			(max_value - min_value)*.0025f,
			min_value,
			max_value
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
*/
/*
xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		float_getter_type^ getter,
		float_setter_type^ setter,
		std::pair<float,LPCSTR> *values,
		u32 const &value_count,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			float::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_float_enum::typeid
		),
		gcnew property_float_enum_value(
			getter,
			setter,
			values,
			value_count
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
*/
/*
xray::editor::property_value* property_holder::add_float				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		float const &default_value,
		float& value,
		std::pair<float,LPCSTR> *values,
		u32 const &value_count,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			float::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_float_enum::typeid
		),
		gcnew property_float_enum_value_reference(
			value,
			values,
			value_count
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
*/