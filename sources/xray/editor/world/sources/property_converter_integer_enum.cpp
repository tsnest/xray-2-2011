////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_converter_integer_enum.h"
#include "property_integer_enum_value.h"
#include "property_container.h"

using System::ComponentModel::ITypeDescriptorContext;
using System::ComponentModel::TypeConverter;
typedef TypeConverter::StandardValuesCollection	StandardValuesCollection;
typedef Flobbster::Windows::Forms::PropertyBag::PropertySpecDescriptor PropertySpecDescriptor;
using System::Object;

bool property_converter_integer_enum::GetStandardValuesSupported				(ITypeDescriptorContext^ context)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return							(true);
}

bool property_converter_integer_enum::GetStandardValuesExclusive				(ITypeDescriptorContext^ context)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return							(true);
}

extern property_container^		property_container_from_context		( ITypeDescriptorContext^ context );
extern PropertySpecDescriptor^	property_descriptor_from_context	( ITypeDescriptorContext^ context );

StandardValuesCollection ^property_converter_integer_enum::GetStandardValues	(ITypeDescriptorContext^ context)
{
	property_container^				container			= property_container_from_context(context);
	PropertySpecDescriptor^			descriptor			= property_descriptor_from_context(context);
	xray::editor::controls::property_value^	raw_value	= container->value(descriptor->item);
	property_integer_enum_value^	value				= safe_cast<property_integer_enum_value^>(raw_value);
	return												(gcnew StandardValuesCollection(value->m_collection));
}

Object^	property_converter_integer_enum::ConvertTo								(
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

	typedef property_integer_enum_value::ValuePair	ValuePair;
	ValuePair^						pair_value = dynamic_cast<ValuePair^>(value);
	if (pair_value)
		return						(pair_value->second);

	property_container^				container = safe_cast<property_container^>(context->Instance);
	PropertySpecDescriptor^			descriptor = safe_cast<PropertySpecDescriptor^>(context->PropertyDescriptor);
	xray::editor::controls::property_value^	raw_value = container->value(descriptor->item);
	property_integer_enum_value^	real_value = safe_cast<property_integer_enum_value^>(raw_value);
	int								int_value = safe_cast<int>(value);

	for each (ValuePair^ i in real_value->m_collection) {
		if (i->first != int_value)
			continue;

		return						(i->second);
	}

	return							(safe_cast<ValuePair^>(real_value->m_collection[0])->second);
}
