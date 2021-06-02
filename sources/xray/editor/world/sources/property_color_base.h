////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_COLOR_BASE_HPP_INCLUDED
#define PROPERTY_COLOR_BASE_HPP_INCLUDED

#include "property_holder_include.h"
#include "property_container_holder.h"
#include "property_container.h"

ref class property_container_Color;
ref class property_color_base;

ref class color_components {
public:
									color_components(property_color_base^ holder);

			float		red_getter		();
			void		red_setter		(float);

			float		green_getter	();
			void		green_setter	(float);

			float		blue_getter		();
			void		blue_setter		(float);

private:
	property_color_base^	m_holder;
}; // class color_components

public value struct Color {
public:
	inline					Color					(float red, float green, float blue)
	{
		r			= red;
		g			= green;
		b			= blue;
	}

	property float	r;
	property float	g;
	property float	b;
}; // value struct Color

public ref class property_color_base abstract :
	public xray::editor::controls::property_value,
	public property_container_holder,
	public xray::editor::controls::property_mouse_events,
	public xray::editor::controls::property_incrementable
{
public:
	typedef System::Attribute					Attribute;

public:
							property_color_base	(color const% color, 
								array<System::Attribute^>^ attributes,
								property_holder::readonly_enum read_only,
								property_holder::notify_parent_on_change_enum notify_parent,
								property_holder::password_char_enum password,
								property_holder::refresh_grid_on_change_enum refresh_grid );

	virtual					~property_color_base();
							!property_color_base();
			void			red					(float value);
			void			green				(float value);
			void			blue				(float value);

public:
	virtual System::Object	^get_value			();
	virtual void			set_value			(System::Object ^object);
	virtual	void			on_double_click		(xray::editor::controls::property_grid^ property_grid);
	virtual void			increment			(float const% increment);

public:
	virtual color			get_value_raw		() = 0;
	virtual void			set_value_raw		(color color) = 0;

private:
	property_container_Color^						m_container;
	color_components^							m_components;
	array<System::Attribute^>^					m_attributes;
}; // ref class property_color_base abstract


public ref class property_container_Color : public property_container
{
public:
	property_container_Color(property_holder* holder, property_container_holder^ container_holder):
								property_container	(holder, container_holder){}


	  virtual bool			Equals			( Object^ other_object ) override
	  {
		  if( other_object->GetType() == Color::typeid )
		  {	

			  Color^ other = safe_cast<Color^>(other_object);
			  color own = safe_cast<property_color_base%>(container_holder()).get_value_raw();
			  return ( own.r == other->r && own.g == other->g && own.b == other->b);
		  }
		  return property_container::Equals( other_object );
	  }
};


#endif // ifndef PROPERTY_COLOR_BASE_HPP_INCLUDED