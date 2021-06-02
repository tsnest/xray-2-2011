////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_VEC3F_BASE_HPP_INCLUDED
#define PROPERTY_VEC3F_BASE_HPP_INCLUDED

#include "property_holder_include.h"
#include "property_container_holder.h"
#include "property_container.h"


ref class property_container_Vec3f;
ref class property_vec3f_base;

ref class property_container_Vec2f;
ref class property_vec2f_base;

ref class vec3f_components {
public:
									vec3f_components(property_vec3f_base^ holder);

			float					x_getter		();
			void					x_setter		(float);

			float					y_getter		();
			void					y_setter		(float);

			float					z_getter		();
			void					z_setter		(float);

private:
	property_vec3f_base^			m_holder;
}; // class vec3f_components

public value struct Vec3f {
public:
	inline					Vec3f			(float x_, float y_, float z_)
	{
		x			= x_;
		y			= y_;
		z			= z_;
	}

	property float	x;
	property float	y;
	property float	z;
}; // value struct  Vec3f

public ref class property_vec3f_base abstract :
	public xray::editor::controls::property_value,
	public property_container_holder
{
public:
	property_vec3f_base	(	float3 const% vec3f, 
							property_holder::readonly_enum read_only,
							property_holder::notify_parent_on_change_enum notify_parent,
							property_holder::password_char_enum password,
							property_holder::refresh_grid_on_change_enum refresh_grid );

	virtual					~property_vec3f_base();
							!property_vec3f_base();
			void			x					(float value);
			void			y					(float value);
			void			z					(float value);

public:
	virtual System::Object	^get_value			();
	virtual void			set_value			(System::Object ^object);

public:
	virtual float3			get_value_raw	() = 0;
	virtual void			set_value_raw		(float3 vec3f) = 0;

private:
	property_container_Vec3f^	m_container;
	vec3f_components^			m_components;
}; // ref class property_vec3f_base abstract

public ref class property_container_Vec3f : public property_container
{
public:
	property_container_Vec3f(property_holder* holder, property_container_holder^ container_holder):
								property_container	(holder, container_holder){}


	virtual bool			Equals			( Object^ other_object ) override
	{
		if( other_object->GetType() == Vec3f::typeid )
		{	

			Vec3f^ other = safe_cast<Vec3f^>(other_object);
			float3 own = safe_cast<property_vec3f_base%>(container_holder()).get_value_raw();
			return ( own.x == other->x && own.y == other->y && own.z == other->z);
		}
		return property_container::Equals( other_object );
	}

	virtual System::String^ ToString() override
	{
		float3 own = safe_cast<property_vec3f_base%>(container_holder()).get_value_raw();
		return  String::Format("x: {0:F3}; x: {1:F3}; x: {2:F3};", own.x, own.y, own.z);
	}
};


// vec2f
ref class vec2f_components {
public:
									vec2f_components(property_vec2f_base^ holder);

			float					x_getter		();
			void					x_setter		(float);

			float					y_getter		();
			void					y_setter		(float);
private:
	property_vec2f_base^			m_holder;
}; // class vec2f_components

public value struct Vec2f {
public:
	inline					Vec2f			(float x_, float y_)
	{
		x			= x_;
		y			= y_;
	}

	property float	x;
	property float	y;
}; // value struct  Vec2f

public ref class property_vec2f_base abstract :
	public xray::editor::controls::property_value,
	public property_container_holder
{
public:
	property_vec2f_base	(	float2 const% vec2f, 
							property_holder::readonly_enum read_only,
							property_holder::notify_parent_on_change_enum notify_parent,
							property_holder::password_char_enum password,
							property_holder::refresh_grid_on_change_enum refresh_grid );

	virtual					~property_vec2f_base();
							!property_vec2f_base();
			void			x					(float value);
			void			y					(float value);

public:
	virtual System::Object	^get_value			();
	virtual void			set_value			(System::Object ^object);

public:
	virtual float2			get_value_raw	() = 0;
	virtual void			set_value_raw		(float2 vec2f) = 0;

private:
	property_container_Vec2f^	m_container;
	vec2f_components^			m_components;
}; // ref class property_vec2f_base abstract

public ref class property_container_Vec2f : public property_container
{
public:
	property_container_Vec2f(property_holder* holder, property_container_holder^ container_holder):
								property_container	(holder, container_holder){}


	virtual bool			Equals			( Object^ other_object ) override
	{
		if( other_object->GetType() == Vec2f::typeid )
		{	

			Vec2f^ other = safe_cast<Vec2f^>(other_object);
			float2 own = safe_cast<property_vec2f_base%>(container_holder()).get_value_raw();
			return ( own.x == other->x && own.y == other->y);
		}
		return property_container::Equals( other_object );
	}
};

#endif // ifndef PROPERTY_VEC3F_BASE_HPP_INCLUDED