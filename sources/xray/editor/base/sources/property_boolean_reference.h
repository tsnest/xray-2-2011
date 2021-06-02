////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_BOOLEAN_REFERENCE_HPP_INCLUDED
#define PROPERTY_BOOLEAN_REFERENCE_HPP_INCLUDED

#include "property_value_include.h"

public ref class property_boolean_reference : public xray::editor::wpf_controls::i_property_value {
public:
							property_boolean_reference	(bool& value);
	virtual					~property_boolean_reference	();
							!property_boolean_reference	();
	virtual System::Object	^get_value					();
	virtual void			set_value					(System::Object ^object);
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Boolean::typeid;}
	}
private:
	value_holder<bool>*		m_value;
}; // ref class property_boolean

#endif // ifndef PROPERTY_BOOLEAN_REFERENCE_HPP_INCLUDED