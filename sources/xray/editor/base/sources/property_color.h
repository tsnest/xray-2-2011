////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_COLOR_HPP_INCLUDED
#define PROPERTY_COLOR_HPP_INCLUDED

using namespace System;
using namespace System::Reflection;

public ref class property_color : public xray::editor::wpf_controls::i_property_value
{

public:
							property_color	(
								color_getter_type^ getter,
								color_setter_type^ setter
								);
							property_color	(
								Object^ obj,
								PropertyInfo^ prop_info
								);
	
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Windows::Media::Color::typeid;}
	}

	virtual System::Object	^get_value			();
	virtual void			set_value			(System::Object ^object);

private:
	color_getter_type		^m_getter;
	color_setter_type		^m_setter;
}; // ref class property_color

#endif // ifndef PROPERTY_COLOR_HPP_INCLUDED