////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_FLOAT_HPP_INCLUDED
#define PROPERTY_FLOAT_HPP_INCLUDED

#include "property_holder_include.h"

public ref class property_float :
	public xray::editor::controls::property_value,
	public xray::editor::controls::property_incrementable
{
public:
							property_float	(
								float_getter_type^ getter,
								float_setter_type^ setter,
								float const% increment_factor
							);
	virtual					~property_float	();
							!property_float	();
	virtual System::Object	^get_value		();
	virtual void			set_value		(System::Object ^object);
	virtual void			increment		(float const% increment);

private:
	float_getter_type^		m_getter;
	float_setter_type^		m_setter;
	float					m_increment_factor;
}; // ref class property_float

#endif // ifndef PROPERTY_FLOAT_HPP_INCLUDED