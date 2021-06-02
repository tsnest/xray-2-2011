////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_BOOLEAN_HPP_INCLUDED
#define PROPERTY_BOOLEAN_HPP_INCLUDED

#include "property_holder_include.h"

public ref class property_boolean : public xray::editor::controls::property_value {

public:
							property_boolean	(
								boolean_getter_type^ getter,
								boolean_setter_type^ setter
							);
	virtual					~property_boolean	();
							!property_boolean	();
	virtual System::Object	^get_value			();
	virtual void			set_value			(System::Object ^object);

private:
	boolean_getter_type		^m_getter;
	boolean_setter_type		^m_setter;
}; // ref class property_boolean

#endif // ifndef PROPERTY_BOOLEAN_HPP_INCLUDED