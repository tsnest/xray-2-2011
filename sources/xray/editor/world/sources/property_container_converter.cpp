////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_container_converter.h"
#include "property_container.h"

using System::ComponentModel::PropertyDescriptorCollection;
using System::ComponentModel::TypeDescriptor;
using System::String;
using System::Object;
using System::Array;
using System::Collections::ArrayList;
using Flobbster::Windows::Forms::PropertySpec;

PropertyDescriptorCollection^ property_container_converter::GetProperties	(
		ITypeDescriptorContext^ context,
		Object^ value,
		array<Attribute^>^ attributes
	)
{
	PropertyDescriptorCollection^	current = TypeDescriptor::GetProperties(value, attributes);
	ASSERT							(current);
	property_container^				container = dynamic_cast<property_container^>(context->Instance);
	if (container) {
		ArrayList%					properties = container->ordered_properties();
		ArrayList^					names = gcnew ArrayList();
		for each (PropertySpec^ i in properties)
			names->Add				(i->Name);

		return						(current->Sort(reinterpret_cast<array<String^>^>(names->ToArray(String::typeid))));
	}

	// here we should construct an intersection of properties sets
	// instead of just getting the properties from the first object
	Array^							objects = safe_cast<Array^>(context->Instance);
	ASSERT							(objects->Length);
	container						= safe_cast<property_container^>(objects->GetValue(0));
	ArrayList%						properties = container->ordered_properties();
	ArrayList^						names = gcnew ArrayList();
	for each (PropertySpec^ i in properties)
		names->Add					(i->Name);

	return							(current->Sort(reinterpret_cast<array<String^>^>(names->ToArray(String::typeid))));
}

bool property_container_converter::GetPropertiesSupported					(
		ITypeDescriptorContext^ context
	)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return		(true);
}

bool property_container_converter::CanConvertTo								(
		ITypeDescriptorContext^ context,
		Type^ destination_type
	)
{
	if (destination_type == property_container::typeid)
		return	(true);

	return		(inherited::CanConvertTo(context, destination_type));
}

Object^ property_container_converter::ConvertTo								(
		ITypeDescriptorContext^ context,
		CultureInfo^ culture,
		Object^ value,
		Type^ destination_type
	)
{
	if (destination_type != String::typeid)
		return	(inherited::ConvertTo(context, culture, value, destination_type));

	return		("...");
}
