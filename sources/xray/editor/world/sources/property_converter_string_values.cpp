////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_converter_string_values.h"
#include "property_string_values_value_base.h"
#include "property_container.h"

using System::ComponentModel::ITypeDescriptorContext;
using System::ComponentModel::TypeConverter;
using namespace System;
using namespace System::Reflection;
using namespace System::ComponentModel;
typedef TypeConverter::StandardValuesCollection	StandardValuesCollection;
typedef Flobbster::Windows::Forms::PropertyBag::PropertySpecDescriptor PropertySpecDescriptor;

bool property_converter_string_values::GetStandardValuesSupported				(ITypeDescriptorContext^ context)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return							(true);
}

bool property_converter_string_values::GetStandardValuesExclusive				(ITypeDescriptorContext^ context)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return							(true);
}

property_container^ property_container_from_context	( ITypeDescriptorContext^ context )
{
	property_container^	result		= dynamic_cast<property_container^>(context->Instance);
	if ( result )
		return						result;

	System::Array^ containers		= safe_cast<System::Array^>(context->Instance);
	ASSERT							( containers->Length );
	return							safe_cast<property_container^>( containers->GetValue(0) );
}

PropertySpecDescriptor^ property_descriptor_from_context	( ITypeDescriptorContext^ context )
{
	PropertySpecDescriptor^	result	= dynamic_cast<PropertySpecDescriptor^>(context->PropertyDescriptor);
	if ( result )
		return						result;

	PropertyDescriptor^ descriptor = context->PropertyDescriptor;
	Object^ tmp_obj = descriptor->GetType()->GetField("descriptors", BindingFlags::Instance|BindingFlags::NonPublic)->GetValue(descriptor);
	return safe_cast<PropertySpecDescriptor^>(safe_cast<array<PropertyDescriptor^>^>(tmp_obj)[0]);
}

StandardValuesCollection ^property_converter_string_values::GetStandardValues	(ITypeDescriptorContext^ context)
{
	property_container^	container	= property_container_from_context(context);
	PropertySpecDescriptor^	descriptor	= property_descriptor_from_context(context);
	xray::editor::controls::property_value^		raw_value = container->value(descriptor->item);
	property_string_values_value_base^	value = safe_cast<property_string_values_value_base^>(raw_value);
	return							(gcnew StandardValuesCollection(value->values()));
}

bool property_converter_string_values::CanConvertFrom							(ITypeDescriptorContext^ context, Type^ source_type)
{
	XRAY_UNREFERENCED_PARAMETERS	( context, source_type );
	return							(false);
}
