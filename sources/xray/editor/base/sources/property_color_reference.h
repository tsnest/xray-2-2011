////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_COLOR_REFERENCE_HPP_INCLUDED
#define PROPERTY_COLOR_REFERENCE_HPP_INCLUDED

#include "property_value_include.h"

using namespace System;

public ref class property_color_reference : public  xray::editor::wpf_controls::i_property_value
{
public:
					property_color_reference	(
						color& value
					);
	virtual			~property_color_reference	();
					!property_color_reference	();
	virtual Object^	get_value					();
	virtual void	set_value					(Object^ value);
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Windows::Media::Color::typeid;}
	}
private:
	value_holder<color>*	m_value;
}; // ref class property_color_reference

#endif // ifndef PROPERTY_COLOR_REFERENCE_HPP_INCLUDED