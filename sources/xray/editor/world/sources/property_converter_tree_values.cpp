////////////////////////////////////////////////////////////////////////////
//	Created 	: 21.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_converter_tree_values.h"
#include "property_color.h"

using System::Object;
using System::String;
using System::ComponentModel::PropertyDescriptorCollection;
using System::ComponentModel::PropertyDescriptor;
using System::ComponentModel::TypeDescriptor;
using System::ArgumentException;
using Flobbster::Windows::Forms::PropertyBag;
using System::Attribute;

typedef PropertyBag::PropertySpecDescriptor	PropertySpecDescriptor;

bool property_converter_tree_values::CanConvertFrom	(
		ITypeDescriptorContext^ context,
		Type^ source_type
	)
{
	XRAY_UNREFERENCED_PARAMETERS	( context, source_type );
	return		(false);
}
