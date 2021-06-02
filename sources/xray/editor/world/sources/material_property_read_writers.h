////////////////////////////////////////////////////////////////////////////
//	Created		: 25.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_PROPERTY_READ_WRITERS_H_INCLUDED
#define MATERIAL_PROPERTY_READ_WRITERS_H_INCLUDED

typedef System::Windows::Media::Color MColor; 
using System::Windows::Media::Media3D::Vector3D;
using xray::editor::wpf_controls::color_rgb;

namespace xray 
{
	namespace editor 
	{

		template<typename TType>
		ref class material_property_read_writer abstract
		{
			public:
				virtual		TType		read	( configs::lua_config_value config )				= 0;
				virtual		void		write	( configs::lua_config_value config, TType value )	= 0;
		};// ref class material_property_read_writer

		template<typename TType>
		ref class material_property_value_read_writer: material_property_read_writer<TType>
		{
			public:
				virtual		TType		read	( configs::lua_config_value config)override					{ return (TType)config; }
				virtual		void		write	( configs::lua_config_value config, TType value )override	{  config = (TType)value; }
		};// ref class material_property_value_read_writer

		ref class material_property_string_read_writer: material_property_read_writer<System::String^>
		{
			public:
				virtual		System::String^		read	( configs::lua_config_value config)override	{ return gcnew System::String( (pcstr)config ); }
				virtual		void				write	( configs::lua_config_value config, System::String^ value )override	{  config = unmanaged_string( value ).c_str(); }
		};// ref class material_property_string_read_writer

		ref class material_property_color_read_writer: material_property_read_writer<MColor>
		{
			public:
				virtual		MColor		read	( configs::lua_config_value config)override { float4 c = (float4)config; return MColor::FromScRgb(c.w, c.x, c.y, c.z ); }
				virtual		void		write	( configs::lua_config_value config, MColor value )override	{  config = float4( value.ScR, value.ScG, value.ScB, value.ScA ); }
		};// ref class material_property_color_read_writer

		ref class material_property_color_rgb_read_writer: material_property_read_writer<color_rgb>
		{
		public:
			virtual		color_rgb	read	( configs::lua_config_value config)override { float4 c = (float4)config; return color_rgb( c.x, c.y, c.z, c.w ); }
			virtual		void		write	( configs::lua_config_value config, color_rgb value )override	{  config = float4( (float)value.r, (float)value.g, (float)value.b, (float)value.a ); }
		};// ref class material_property_color_read_writer

		ref class material_property_vector3_read_writer: material_property_read_writer<Vector3D>
		{
		public:
			virtual		Vector3D	read	( configs::lua_config_value config)override { float3 c = (float3)config; return Vector3D( c.x, c.y, c.z ); }
			virtual		void		write	( configs::lua_config_value config, Vector3D value )override	{  config = float3( (float)value.X, (float)value.Y, (float)value.Z ); }
		};// ref class material_property_vector3_read_writer

	} // namespace editor
} // namespace xray

#endif // #ifndef MATERIAL_PROPERTY_READ_WRITERS_H_INCLUDED