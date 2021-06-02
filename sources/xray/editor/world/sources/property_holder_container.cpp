////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container.h"
#include "property_property_container.h"

using Flobbster::Windows::Forms::PropertySpec;
using xray::editor::wpf_controls::special_attribute;

xray::editor::property_value* property_holder::add_property_holder				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		xray::editor::property_holder_ptr value,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	::property_holder*			real_value = dynamic_cast<::property_holder*>(&*value);
	ASSERT						(real_value);
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			property_container::typeid,
			category,
			description,
			real_value->container()
		),
		gcnew property_property_container(
			value
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}

xray::editor::property_value* property_holder::add_special_property_holder				(
	System::String^ identifier,
	System::String^ category,
	System::String^ description,
	xray::editor::property_holder_ptr value,
	System::String^	holder_type,
	readonly_enum const& read_only,
	notify_parent_on_change_enum const& notify_parent,
	password_char_enum const& password,
	refresh_grid_on_change_enum const& refresh_grid
	)
{
	::property_holder*			real_value = dynamic_cast<::property_holder*>(&*value);
	ASSERT						(real_value);
	PropertySpec^ spec = gcnew PropertySpec(
		identifier,
		property_container::typeid,
		category,
		description,
		real_value->container()
	);

	array<System::Attribute^>^ attribute_array = gcnew array<System::Attribute^>(1);
	attribute_array[0] = gcnew xray::editor::wpf_controls::special_attribute(holder_type);
	spec->Attributes = attribute_array;

	
	m_container->add_property	(
		spec,
		gcnew property_property_container(
		value
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
		);

	return						(nullptr);
}