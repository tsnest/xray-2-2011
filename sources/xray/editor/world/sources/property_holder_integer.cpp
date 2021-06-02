////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container.h"
#include "property_integer.h"
#include "property_integer_reference.h"
#include "property_integer_limited.h"
#include "property_integer_limited_reference.h"
#include "property_integer_values_value.h"
#include "property_integer_values_value_getter.h"
#include "property_integer_values_value_reference.h"
#include "property_integer_values_value_reference_getter.h"
#include "property_integer_enum_value.h"
#include "property_integer_enum_value_reference.h"
#include "integer_holder.h"

ref class property_converter_integer_enum;
ref class property_converter_integer_values;
ref class property_integer_values_value;

using Flobbster::Windows::Forms::PropertySpec;
using System::String;

xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		xray::configs::lua_config_value const& config,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
		)
{
	xray::editor::integer_holder^ i_holder = gcnew xray::editor::integer_holder(identifier, config);
	return add_integer		(
		identifier,
		category,
		description,
		default_value,
		gcnew integer_getter_type(i_holder, &xray::editor::integer_holder::get),
		gcnew integer_setter_type(i_holder, &xray::editor::integer_holder::set),
		read_only,
		notify_parent,
		password,
		refresh_grid);
}


xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		integer_getter_type^ getter,
		integer_setter_type^ setter,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			int::typeid,
			category,
			description,
			default_value
		),
		gcnew property_integer(
			getter,
			setter
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
/*
xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		int& value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			int::typeid,
			category,
			description,
			default_value
		),
		gcnew property_integer_reference(value),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}*/

xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		integer_getter_type^ getter,
		integer_setter_type^ setter,
		int const &min_value,
		int const &max_value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	PropertySpec^ spec = gcnew PropertySpec(
		identifier,
		int::typeid,
		category,
		description,
		default_value
	);

	array<System::Attribute^>^ attribute_array = gcnew array<System::Attribute^>(1);
	attribute_array[0] = gcnew xray::editor::wpf_controls::value_range_and_format_attribute(min_value, max_value);
	spec->Attributes = attribute_array;

	m_container->add_property	(
		spec,
		gcnew property_integer_limited(
			getter,
			setter,
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

xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		integer_getter_type^ getter,
		integer_setter_type^ setter,
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
		int::typeid,
		category,
		description,
		default_value
	);

	array<System::Attribute^>^ attribute_array = gcnew array<System::Attribute^>(1);
	attribute_array[0] = gcnew xray::editor::wpf_controls::value_range_and_format_attribute(min_value_func, max_value_func, 1, 0);
	spec->Attributes = attribute_array;

	m_container->add_property	(
		spec,
		gcnew property_integer_limited(
			getter,
			setter,
			(int)min_value_func(),
			(int)max_value_func()
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
/*
xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		int& value,
		int const &min_value,
		int const &max_value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			int::typeid,
			category,
			description,
			default_value
		),
		gcnew property_integer_limited_reference(
			value,
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
xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		integer_getter_type const &getter,
		integer_setter_type const &setter,
		std::pair<int,LPCSTR> *values,
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
			int::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_integer_enum::typeid
		),
		gcnew property_integer_enum_value(
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
}*/
/*
xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		int& value,
		std::pair<int,LPCSTR> *values,
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
			int::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_integer_enum::typeid
		),
		gcnew property_integer_enum_value_reference(
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
}*/
/*
xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		integer_getter_type^ getter,
		integer_setter_type^ setter,
		LPCSTR const* values,
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
			int::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_integer_values::typeid
		),
		gcnew property_integer_values_value(
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
xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		int& value,
		LPCSTR const* values,
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
			int::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_integer_values::typeid
		),
		gcnew property_integer_values_value_reference(
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
}*/

xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		integer_getter_type^ getter,
		integer_setter_type^ setter,
		string_collection_getter_type^ values,
		string_collection_size_getter_type^ value_count,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			int::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_integer_values::typeid
		),
		gcnew property_integer_values_value_getter(
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
/*
xray::editor::property_value* property_holder::add_integer				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		int const &default_value,
		int& value,
		string_collection_getter_type const& values,
		string_collection_size_getter_type const& value_count,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			int::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_integer_values::typeid
		),
		gcnew property_integer_values_value_reference_getter(
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