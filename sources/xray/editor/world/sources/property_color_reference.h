////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_COLOR_REFERENCE_HPP_INCLUDED
#define PROPERTY_COLOR_REFERENCE_HPP_INCLUDED

#include "property_color_base.h"

public ref class property_color_reference : public property_color_base {
public:
	typedef property_color_base							inherited;
public:
					property_color_reference	(
						color& value,
						array<System::Attribute^>^ attributes,
						property_holder::readonly_enum read_only,
						property_holder::notify_parent_on_change_enum notify_parent,
						property_holder::password_char_enum password,
						property_holder::refresh_grid_on_change_enum refresh_grid 
					);
	virtual			~property_color_reference	();
					!property_color_reference	();
	virtual color	get_value_raw				() override;
	virtual void	set_value_raw				(color value) override;

private:
	value_holder<color>*	m_value;
}; // ref class property_color_reference

#endif // ifndef PROPERTY_COLOR_REFERENCE_HPP_INCLUDED