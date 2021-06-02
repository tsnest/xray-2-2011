////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container.h"
#include "property_collection.h"
#include "property_collection_getter.h"
#include "attribute.h"

using Flobbster::Windows::Forms::PropertySpec;
using xray::editor::property_holder_collection;

public ref class property_value_managed : public xray::editor::controls::property_value {
public:
				property_value_managed	(
			System::Object^						object,
			System::Reflection::PropertyInfo^	property_info
		) :
		m_object		( object ),
		m_property_info	( property_info )
	{
	}

private:
	delegate System::Object^ getter_type( );

	generic <typename ParameterType> 
	delegate void setter_type(ParameterType parameter);

public:
	virtual System::Object	^get_value	( )
	{
		return m_property_info->GetValue( m_object, nullptr );
	}
	
	virtual void			set_value	( System::Object ^value )
	{
		m_property_info->SetValue	( m_object, value, nullptr );
	}

private:
	System::Object^						m_object;
	System::Reflection::PropertyInfo^	m_property_info;
}; // ref class property_value_managed

public ref class property_object_managed : public xray::editor::controls::property_value 
{
public:
				property_object_managed	( System::Object^ property_object):m_object(property_object){}

private:
	//delegate System::Object^ getter_type( );

	//generic <typename ParameterType> 
	//delegate void setter_type(ParameterType parameter);

public:
	virtual System::Object	^get_value	( )
	{
		return m_object;
	}
	
	virtual void			set_value	( System::Object ^value )
	{
		m_object = value;
	}

private:
	System::Object^						m_object;
}; // ref class property_object_managed

xray::editor::property_value* property_holder::add_property( System::Object^ object, System::Reflection::PropertyInfo^ property_info)
{
	using System::ComponentModel::DisplayNameAttribute;
	using System::ComponentModel::DescriptionAttribute;
	using System::ComponentModel::CategoryAttribute;
	using System::ComponentModel::ReadOnlyAttribute;
	using System::ComponentModel::EditorAttribute;

	array<System::Object^>^ attribs			= property_info->GetCustomAttributes(DisplayNameAttribute::typeid, true);

	DisplayNameAttribute^ att_name			= safe_cast<DisplayNameAttribute^>(attribs[0]);
	System::String^ display_name			= att_name->DisplayName;
	
	attribs									= property_info->GetCustomAttributes(DescriptionAttribute::typeid, true);
	DescriptionAttribute^ att_desc			= safe_cast<DescriptionAttribute^>(attribs[0]);
	System::String^ description				= att_desc->Description;
	
	attribs									= property_info->GetCustomAttributes(CategoryAttribute::typeid, true);
	CategoryAttribute^ att_category			= safe_cast<CategoryAttribute^>(attribs[0]);
	System::String^ category				= att_category->Category;

	attribs									= property_info->GetCustomAttributes(ReadOnlyAttribute::typeid, true);
	
	ReadOnlyAttribute^ att_readonly			= safe_cast<ReadOnlyAttribute^>(attribs[0]);
	readonly_enum read_only					= att_readonly->IsReadOnly ? property_holder::property_read_only : property_holder::property_read_write;

	System::String^	editor					= nullptr;
	attribs									= property_info->GetCustomAttributes(EditorAttribute::typeid, true);
	if(attribs->Length)
		editor								= safe_cast<EditorAttribute^>(attribs[0])->EditorTypeName;
	else
	{
		attribs = property_info->PropertyType->GetCustomAttributes(EditorAttribute::typeid, true);
		if(attribs->Length)
			editor							= safe_cast<EditorAttribute^>(attribs[0])->EditorTypeName;
	}

	System::Object^ default_value			= nullptr;

	attribs									= property_info->GetCustomAttributes(ValueAttribute::typeid, true);
	for each (ValueAttribute^ att in attribs)
	{
		if(att->m_type==e_def_val)
			default_value = att->m_params[0];
	}

	System::String^	type_converter			= nullptr;

	m_container->add_property	(
		gcnew PropertySpec(
			display_name,
			property_info->PropertyType,
			category,
			description,
			default_value, 
			editor, 
			type_converter
		),
		gcnew property_value_managed( object, property_info ),
		read_only,
		property_holder::do_not_notify_parent_on_change,
		property_holder::no_password_char,
		property_holder::do_not_refresh_grid_on_change
	);

	return						(nullptr);
}

xray::editor::property_value* property_holder::add_collection				(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		property_holder_collection* collection,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			property_collection::typeid,
			category,
			description
		),
		gcnew property_collection(
			collection
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}

xray::editor::property_value* property_holder::add_collection		(
		System::String^ identifier,
		System::String^ category,
		System::String^ description,
		collection_getter_type const& collection_getter,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid
	)
{
	m_container->add_property	(
		gcnew PropertySpec(
			identifier,
			property_collection::typeid,
			category,
			description
		),
		gcnew property_collection_getter(
			collection_getter
		),
		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}

xray::editor::property_value* property_holder::add_property( 
		System::Object^ prop_object,
		System::String^ display_value,
		System::String^ category,
		System::String^ description,
		readonly_enum const& read_only,
		notify_parent_on_change_enum const& notify_parent,
		password_char_enum const& password,
		refresh_grid_on_change_enum const& refresh_grid)
{
	using System::ComponentModel::DisplayNameAttribute;
	using System::ComponentModel::DescriptionAttribute;
	using System::ComponentModel::CategoryAttribute;
	using System::ComponentModel::ReadOnlyAttribute;
	using System::ComponentModel::EditorAttribute;

	m_container->add_property	(
		gcnew PropertySpec(
			display_value,
			prop_object->GetType(),
			category,
			description,
			(System::Object^)nullptr, 
			(System::String^)nullptr, 
			(System::String^)nullptr
		),

		gcnew property_object_managed( prop_object ),

		read_only,
		notify_parent,
		password,
		refresh_grid
	);

	return						(nullptr);
}
