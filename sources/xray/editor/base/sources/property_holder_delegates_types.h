////////////////////////////////////////////////////////////////////////////
//	Created		: 12.08.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PROPERTY_HOLDER_DELEGATES_TYPES
#define PROPERTY_HOLDER_DELEGATES_TYPES

namespace xray {
	namespace editor_base {
		class property_holder;
		class editor_intrusive_base;
		class property_holder_collection;
		class property_holder_holder;
	}
}

public value struct Float3
{
	inline Float3			(float3 const& other)
							{
								x = other.x;
								y = other.y;
								z = other.z;
							}

	inline operator	float3	()	{return float3(x,y,z);}

	float x;
	float y;
	float z;
};


public value struct Float2
{
	inline Float2			(float2 const& other)
							{
								x = other.x;
								y = other.y;
							}

	inline operator	float2	()	{return float2(x,y);}

	property float x;
	property float y;
};

public value struct xrayColor
{
	inline xrayColor		( xray::math::color const& other)				{ r=other.get_R()/255.0f; g=other.get_Gf(); b=other.get_Bf(); a=other.get_Af();}
	inline operator	xray::math::color	( )									{ return xray::math::color(r,g,b,a); }
	inline xray::math::color math_color	( )									{ return xray::math::color(r,g,b,a); }
	inline void set_rgba	( float cr, float cg, float cb, float ca )		{ r = cr; g = cg; b = cb; a = ca; }
	property float r;
	property float g;
	property float b;
	property float a;
};

public delegate bool			boolean_getter_type					();
public delegate void			boolean_setter_type					(bool);

public delegate int				integer_getter_type					();
public delegate void			integer_setter_type					(int);

public delegate float			float_getter_type					();
public delegate void			float_setter_type					(float);

public delegate System::String^ string_getter_type					();
public delegate void			string_setter_type					(System::String^);

public delegate System::String^	string_collection_getter_type		(u32 idx);
public delegate u32				string_collection_size_getter_type	();

public delegate Float2			float2_getter_type					();
public delegate void			float2_setter_type					(Float2);

public delegate Float3			float3_getter_type					();
public delegate void			float3_setter_type					(Float3);

public delegate xrayColor		color_getter_type					();
public delegate void			color_setter_type					(xrayColor);

#endif // #ifndef PROPERTY_HOLDER_DELEGATES_TYPES
