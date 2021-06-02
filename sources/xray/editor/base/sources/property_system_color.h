////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_SYSTEM_COLOR_H_INCLUDED
#define PROPERTY_SYSTEM_COLOR_H_INCLUDED

#include "property_value_include.h"

public ref class property_system_color : public xray::editor::wpf_controls::i_property_value  {
public:
	property_system_color	(
		color_getter_type^ getter,
		color_setter_type^ setter
		);
	~property_system_color	();
	!property_system_color	();
	virtual System::Object	^get_value		();
	virtual void			set_value		(System::Object ^object);
	
	virtual property System::Type^			value_type
	{
		System::Type^ get() {return System::Windows::Media::Color::typeid;}
	}
private:
	color_getter_type		^m_getter;
	color_setter_type		^m_setter;
}; // ref class property_system_color
#endif // #ifndef PROPERTY_SYSTEM_COLOR_H_INCLUDED