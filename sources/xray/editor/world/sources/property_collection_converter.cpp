////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_collection_converter.h"

using System::Object;

bool property_collection_converter::CanConvertTo(ITypeDescriptorContext^ context, Type^ type)
{
	XRAY_UNREFERENCED_PARAMETER		( context );
	return	(type == System::String::typeid);
}
	
Object^ property_collection_converter::ConvertTo(
		ITypeDescriptorContext^ context,
		CultureInfo^ culture,
		Object^ value,
		Type^ type
	)
{
	XRAY_UNREFERENCED_PARAMETERS	( context, culture, value, type );
	return	("< ... >");
}
