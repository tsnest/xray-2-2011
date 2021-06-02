////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container.h"
#include "property_boolean.h"
#include "property_boolean_reference.h"
#include "property_boolean_values_value.h"
#include "property_boolean_values_value_reference.h"
#include "bool_holder.h"

ref class property_converter_boolean_values;

using Flobbster::Windows::Forms::PropertySpec;
using System::String;

xray::editor::property_value* property_holder::add_bool		(
	System::String^ identifier,
	System::String^ category,
	System::String^ description,
	bool const &default_value,
	xray::configs::lua_config_value const& config,
	readonly_enum const& read_only,
	notify_parent_on_change_enum const& notify_parent,
	password_char_enum const& password,
	refresh_grid_on_change_enum const& refresh_grid
	)
{
	xray::editor::bool_holder^ b_holder = gcnew xray::editor::bool_holder(identifier, config);
	return add_bool		(
						identifier,
						category,
						description,
						default_value,
						gcnew boolean_getter_type(b_holder, &xray::editor::bool_holder::get),
						gcnew boolean_setter_type(b_holder, &xray::editor::bool_holder::set),
						read_only,
						notify_parent,
						password,
						refresh_grid);
}




xray::editor::property_value* property_holder::add_bool		(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		bool const &default_value,
		boolean_getter_type^ getter,
		boolean_setter_type^ setter,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			bool::typeid,
			category,
			description,
			default_value
		),
		gcnew property_boolean(
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
xray::editor::property_value* property_holder::add_bool				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		bool const& default_value,
		bool& value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			bool::typeid,
			category,
			description,
			default_value
		),
		gcnew property_boolean_reference(value),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
*/
xray::editor::property_value* property_holder::add_bool				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		bool const &default_value,
		boolean_getter_type^ getter,
		boolean_setter_type^ setter,
		LPCSTR values[2],
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			bool::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_boolean_values::typeid
		),
		gcnew property_boolean_values_value(
			getter,
			setter,
			values
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
/*
xray::editor::property_value* property_holder::add_bool				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		bool const &default_value,
		bool& value,
		LPCSTR values[2],
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			bool::typeid,
			category,
			description,
			default_value,
			(String^)nullptr,
			property_converter_boolean_values::typeid
		),
		gcnew property_boolean_values_value_reference(
			value,
			values
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
*/