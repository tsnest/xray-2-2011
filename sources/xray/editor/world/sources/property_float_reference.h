////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_FLOAT_REFERENCE_HPP_INCLUDED
#define PROPERTY_FLOAT_REFERENCE_HPP_INCLUDED

#include "property_holder_include.h"

public ref class property_float_reference :
	public xray::editor::controls::property_value,
	public xray::editor::controls::property_incrementable
{
public:
							property_float_reference	(float& value, float const% increment_factor);
	virtual					~property_float_reference	();
							!property_float_reference	();
	virtual System::Object	^get_value					();
	virtual void			set_value					(System::Object ^object);
	virtual void			increment					(float const% increment);

private:
	value_holder<float>*	m_value;
	float					m_increment_factor;
}; // ref class property_float_reference

#endif // ifndef PROPERTY_FLOAT_REFERENCE_HPP_INCLUDED