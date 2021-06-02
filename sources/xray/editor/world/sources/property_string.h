////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_STRING_HPP_INCLUDED
#define PROPERTY_STRING_HPP_INCLUDED

#include "property_holder_include.h"

public ref class property_string : public xray::editor::controls::property_value {

public:
							property_string	(
								string_getter_type^ getter,
								string_setter_type^ setter
							);
	virtual					~property_string();
							!property_string();
	virtual System::Object	^get_value		();
	virtual void			set_value		(System::Object ^object);

private:
	string_getter_type^		m_getter;
	string_setter_type^		m_setter;
}; // ref class property_string

#endif // ifndef PROPERTY_STRING_HPP_INCLUDED