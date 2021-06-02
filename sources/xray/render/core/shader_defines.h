////////////////////////////////////////////////////////////////////////////
//	Created		: 20.05.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SHADER_DEFINES_H_INCLUDED
#define SHADER_DEFINES_H_INCLUDED

namespace xray {
namespace render {

enum	enum_shader_type
{
	enum_shader_type_vertex,
	enum_shader_type_pixel,
	enum_shader_type_geometry,
	enum_shader_types_count		// This is for determining supported shaders actual count
};

template < enum_shader_type shader_type >	 struct shader_type_traits
{ 
	enum { value = false }; 
};

struct vs_data;
template < >	 struct shader_type_traits < enum_shader_type_vertex >
{
	enum						{ value = true }; 

	typedef ID3DVertexShader	shader_hw_interface;
	typedef	vs_data				shader_data;

	static char const * name()				{ return "vertex shader"; }
	static char const * short_name	()		{ return "vs"; }
};

struct gs_data;
template < >	 struct shader_type_traits < enum_shader_type_geometry >
{
	enum						{ value = true }; 

	typedef	gs_data				shader_data;
	typedef ID3DGeometryShader	shader_hw_interface;

	static char const * name()				{ return "geometry shader"; }
	static char const * short_name()		{ return "gs"; }
};

struct ps_data;
template < >	 struct shader_type_traits < enum_shader_type_pixel >
{
	enum						{ value = true }; 

	typedef	ps_data				shader_data;
	typedef ID3DPixelShader		shader_hw_interface;

	static char const * name()				{ return "pixel shader"; }
	static char const * short_name()		{ return "ps"; }
};



} // namespace render
} // namespace xray

#endif // #ifndef SHADER_DEFINES_H_INCLUDED