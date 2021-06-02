////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_COLOR_HPP_INCLUDED
#define PROPERTY_COLOR_HPP_INCLUDED

#include "property_color_base.h"

public ref class property_color : public property_color_base {
public:
	typedef property_color_base							inherited;

public:
							property_color	(
								color_getter_type^ getter,
								color_setter_type^ setter,
								array<System::Attribute^>^ attributes,
								property_holder::readonly_enum read_only,
								property_holder::notify_parent_on_change_enum notify_parent,
								property_holder::password_char_enum password,
								property_holder::refresh_grid_on_change_enum refresh_grid
								);
	virtual					~property_color	();
							!property_color	();
	virtual color			get_value_raw	() override;
	virtual void			set_value_raw	(color value) override;

private:
	color_getter_type		^m_getter;
	color_setter_type		^m_setter;
}; // ref class property_color

#endif // ifndef PROPERTY_COLOR_HPP_INCLUDED