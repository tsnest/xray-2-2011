////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container.h"
#include "property_string.h"
#include "property_string_shared_str.h"
#include "property_string_values_value.h"
#include "property_string_values_value_getter.h"
#include "property_string_values_value_shared_str.h"
#include "property_string_values_value_shared_str_getter.h"
#include "property_file_name_value.h"
#include "property_file_name_value_shared_str.h"
#include "string_holder.h"

ref class property_converter_tree_values;
ref class property_converter_string_values;
ref class property_converter_string_values_no_enter;
ref class property_editor_file_name;
ref class property_editor_tree_values;

using Flobbster::Windows::Forms::PropertySpec;
using xray::editor::wpf_controls::property_grid_editors::string_select_file_editor_attribute;
using xray::editor::wpf_controls::property_grid_editors::external_editor_attribute;
using xray::editor::wpf_controls::property_grid_editors::external_editor_event_handler;
using System::String;

xray::editor::property_value* property_holder::add_string				(
	System::String^ identifier,
	System::String^ category,
	System::String^ description,
	System::String^ default_value,
	xray::configs::lua_config_value const& config,
	readonly_enum const& read_only,
	notify_parent_on_change_enum const& notify_parent,
	password_char_enum const& password,
	refresh_grid_on_change_enum const& refresh_grid
	)


{
	xray::editor::string_holder^ str_holder = gcnew xray::editor::string_holder(identifier, config);
	return add_string(
				identifier,
			   category,
			   description,
			   default_value,
			   gcnew string_getter_type(str_holder, &xray::editor::string_holder::get),
				gcnew string_setter_type(str_holder, &xray::editor::string_holder::set),
				read_only,
				notify_parent,
				password,
				refresh_grid);
}


xray::editor::property_value* property_holder::add_string				(
	System::String^ identifier,
	System::String^ category,
	System::String^ description,
	System::String^ default_value,
	xray::configs::lua_config_value const& config,
	System::Collections::ArrayList^ values,
	readonly_enum const& read_only,
	notify_parent_on_change_enum const& notify_parent,
	password_char_enum const& password,
	refresh_grid_on_change_enum const& refresh_grid
	)


{
	xray::editor::string_holder^ str_holder = gcnew xray::editor::string_holder(identifier, config);
	return add_string(
		identifier,
		category,
		description,
		default_value,
		gcnew string_getter_type(str_holder, &xray::editor::string_holder::get),
		gcnew string_setter_type(str_holder, &xray::editor::string_holder::set),
		values,
		value_editor_combo_box,
		cannot_enter_text,
		read_only,
		notify_parent,
		password,
		refresh_grid);
}



xray::editor::property_value* property_holder::add_string				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		System::String^ default_value,
		string_getter_type^ getter,
		string_setter_type^ setter,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			String::typeid,
			category,
			description,
			default_value
		),
		gcnew property_string(
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

xray::editor::property_value* property_holder::add_string				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		System::String^ default_value,
		string_getter_type^ getter,
		string_setter_type^ setter,
		external_editor_event_handler ^	type_of_external_editor,
		System::Object^ filter,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	PropertySpec	^spec = gcnew PropertySpec(
		identifier,
		String::typeid,
		category,
		description,
		default_value,
		filter
	);
	spec->Attributes = gcnew cli::array<Attribute^>(1);
	spec->Attributes[0] = gcnew xray::editor::wpf_controls::property_grid_editors::external_editor_attribute(type_of_external_editor);

	m_container->add_property	(
		spec,
		gcnew property_string(
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

xray::editor::property_value* property_holder::add_string				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		System::String^ default_value,
		string_getter_type^ getter,
		string_setter_type^ setter,
		System::String^ default_extension,
		System::String^ file_mask,
		System::String^ default_folder,
		System::String^ caption,
		enter_text_enum const& can_enter_text,
		extension_action_enum const& extension_action,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	PropertySpec^ spec = (can_enter_text == property_holder::can_enter_text)? 
		gcnew PropertySpec(
			identifier,
			String::typeid,
			category,
			description,
			default_value,
			property_editor_file_name::typeid,
			(String^)nullptr
		) :
		gcnew PropertySpec(
			identifier,
			String::typeid,
			category,
			description,
			default_value,
			property_editor_file_name::typeid,
			property_converter_tree_values::typeid
		);

	spec->Attributes = gcnew cli::array<Attribute^>(1);
	spec->Attributes[0] = gcnew string_select_file_editor_attribute
		(
			default_extension,
			file_mask,
			default_folder,
			caption
		);

	m_container->add_property	(
		spec,
		gcnew property_file_name_value(
			getter,
			setter,
			default_extension,
			file_mask,
			default_folder,
			caption,
			extension_action == remove_extension
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}

xray::editor::property_value* property_holder::add_string				(
	System::String^ identifier,
	System::String^ category,
	System::String^ description,
	System::String^ default_value,
	xray::configs::lua_config_value const& config,
	System::String^ default_extension,
	System::String^ file_mask,
	System::String^ default_folder,
	System::String^ caption,
	enter_text_enum const& can_enter_text,
	extension_action_enum const& extension_action,
	readonly_enum const& read_only,
	notify_parent_on_change_enum const& notify_parent,
	password_char_enum const& password,
	refresh_grid_on_change_enum const& refresh_grid
	)

{
	xray::editor::string_holder^ str_holder = gcnew xray::editor::string_holder(identifier, config);
	

	return add_string				(
		identifier,
		category,
		description,
		default_value,
		gcnew string_getter_type(str_holder, &xray::editor::string_holder::get),
		gcnew string_setter_type(str_holder, &xray::editor::string_holder::set),
		default_extension,
		file_mask,
		default_folder,
		caption,
		can_enter_text,
		extension_action,
		read_only,
		notify_parent,
		password,
		refresh_grid
		);
}

xray::editor::property_value* property_holder::add_string				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		System::String^ default_value,
		string_getter_type^ getter,
		string_setter_type^ setter,
		System::Collections::ArrayList^ values,
		value_editor_enum const& value_editor,
		enter_text_enum const& can_enter_text,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	PropertySpec	^spec = nullptr;

	if (value_editor == value_editor_tree_view) {
		if (can_enter_text == property_holder::can_enter_text)
			spec	= 
				gcnew PropertySpec(
					identifier,
					String::typeid,
					category,
					description,
					default_value,
					property_editor_tree_values::typeid,
					(String^)nullptr
				);
		else
			spec	= 
				gcnew PropertySpec(
					identifier,
					String::typeid,
					category,
					description,
					default_value,
					property_editor_tree_values::typeid,
					property_converter_tree_values::typeid
				);
	}
	else {
		ASSERT		(value_editor == value_editor_combo_box);
		spec	= 
			gcnew PropertySpec(
				identifier,
				System::String::typeid,
				category,
				description,
				default_value
			);
	}

	array<System::Attribute^>^ attribute_array = gcnew array<System::Attribute^>(1);
	attribute_array[0] = gcnew xray::editor::wpf_controls::property_grid_editors::combo_box_items_attribute(values);
	spec->Attributes = attribute_array;

	m_container->add_property	(
		spec,
		gcnew property_string(
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

xray::editor::property_value* property_holder::add_string				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		System::String^ default_value,
		string_getter_type^ getter,
		string_setter_type^ setter,
		System::Func<System::Int32, System::String^>^ values,
		System::Func<System::Int32>^ value_count,
		value_editor_enum const& value_editor,
		enter_text_enum const& can_enter_text,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	PropertySpec	^spec = nullptr;

	if (value_editor == value_editor_tree_view) {
		if (can_enter_text == property_holder::can_enter_text)
			spec	= 
				gcnew PropertySpec(
					identifier,
					String::typeid,
					category,
					description,
					default_value,
					property_editor_tree_values::typeid,
					(String^)nullptr
				);
		else
			spec	= 
				gcnew PropertySpec(
					identifier,
					String::typeid,
					category,
					description,
					default_value,
					property_editor_tree_values::typeid,
					property_converter_tree_values::typeid
				);
	}
	else {
		ASSERT		(value_editor == value_editor_combo_box);
		spec	= gcnew PropertySpec(
			identifier,
			String::typeid,
			category,
			description,
			default_value
		);

		array<System::Attribute^>^ attribute_array = gcnew array<System::Attribute^>(1);
		attribute_array[0] = gcnew xray::editor::wpf_controls::property_grid_editors::combo_box_items_attribute(value_count, values);
		spec->Attributes = attribute_array;
	}
	m_container->add_property	(
		spec,
		gcnew property_string(
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