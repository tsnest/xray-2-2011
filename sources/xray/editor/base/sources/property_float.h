////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_FLOAT_HPP_INCLUDED
#define PROPERTY_FLOAT_HPP_INCLUDED

#include "property_value_include.h"

using namespace System;
using namespace System::Reflection;

public ref class property_float :
	public xray::editor::wpf_controls::i_property_value
{
public:
							property_float	(
								float_getter_type^ getter,
								float_setter_type^ setter
							);
							property_float	(
								Object^ obj,
								PropertyInfo^ prop_info
							);
	
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Single::typeid;}
	}

	virtual System::Object	^get_value		();
	virtual void			set_value		(System::Object ^object);
	
private:
	float_getter_type^		m_getter;
	float_setter_type^		m_setter;
}; // ref class property_float

#endif // ifndef PROPERTY_FLOAT_HPP_INCLUDED