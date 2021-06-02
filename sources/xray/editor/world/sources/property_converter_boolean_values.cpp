////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_converter_boolean_values.h"
#include "property_boolean_values_value.h"
#include "property_container.h"

using System::ComponentModel::ITypeDescriptorContext;
using System::ComponentModel::TypeConverter;
typedef TypeConverter::StandardValuesCollection	StandardValuesCollection;
typedef Flobbster::Windows::Forms::PropertyBag::PropertySpecDescriptor PropertySpecDescriptor;
using System::Object;

bool property_converter_boolean_values::GetStandardValuesSupported				(ITypeDescriptorContext^ context)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return							(true);
}

bool property_converter_boolean_values::GetStandardValuesExclusive				(ITypeDescriptorContext^ context)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return							(true);
}

extern property_container^		property_container_from_context		( ITypeDescriptorContext^ context );
extern PropertySpecDescriptor^	property_descriptor_from_context	( ITypeDescriptorContext^ context );

StandardValuesCollection ^property_converter_boolean_values::GetStandardValues	(ITypeDescriptorContext^ context)
{
	property_container^				container			= property_container_from_context(context);
	PropertySpecDescriptor^			descriptor			= property_descriptor_from_context(context);
	xray::editor::controls::property_value^	raw_value	= container->value(descriptor->item);
	property_boolean_values_value^	value				= safe_cast<property_boolean_values_value^>(raw_value);
	return												(gcnew StandardValuesCollection(value->m_collection));
}

Object^	property_converter_boolean_values::ConvertTo							(
		ITypeDescriptorContext^ context,
		CultureInfo^ culture,
		Object^ value,
		Type^ destination_type
	)
{
	if (!context)
		return						(inherited::ConvertTo(context, culture, value, destination_type));

	if (!context->Instance)
		return						(inherited::ConvertTo(context, culture, value, destination_type));

	if (!context->PropertyDescriptor)
		return						(inherited::ConvertTo(context, culture, value, destination_type));

	if (destination_type != System::String::typeid)
		return						(inherited::ConvertTo(context, culture, value, destination_type));

	if (dynamic_cast<System::String^>(value))
		return						(value);

	property_container^				container = safe_cast<property_container^>(context->Instance);
	PropertySpecDescriptor^			descriptor = safe_cast<PropertySpecDescriptor^>(context->PropertyDescriptor);
	xray::editor::controls::property_value^	raw_value = container->value(descriptor->item);
	property_boolean_values_value^	real_value = safe_cast<property_boolean_values_value^>(raw_value);
	bool							bool_value = safe_cast<bool>(value);
	return							(real_value->m_collection[bool_value ? 1 : 0]);
}
