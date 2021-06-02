////////////////////////////////////////////////////////////////////////////
//	Created		: 17.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_LIGHT_H_INCLUDED
#define OBJECT_LIGHT_H_INCLUDED

#include "object_base.h"
#include <xray/render/world.h>
#include <xray/render/facade/light_props.h>

namespace xray {

namespace render{	struct light_props; }

namespace editor {

ref class tool_light;

ref class object_light : public object_base
{
	typedef object_base		super;
public:
							object_light			( tool_light^ t, render::scene_ptr const& scene );
	virtual					~object_light			( );
	virtual void			destroy_collision		( )									override;

	virtual void			load_props				( configs::lua_config_value const& t ) override;
	virtual void			save					( configs::lua_config_value t ) override;
	virtual void			render					( ) override;
	virtual void			load_defaults			( ) override;
	virtual void			set_visible				( bool bvisible ) override;
	virtual void			set_transform			( math::float4x4 const& transform ) override;
	virtual void			load_contents			( )	override;
	virtual void			unload_contents			( bool bdestroy ) override;
	virtual wpf_controls::property_container^	get_property_container	( )			override;
	virtual void			on_selected				( bool is_selected )	override;

protected:
			void				initialize_collision	( );
			void				fill_props				( render::light_props& props );
			void				sync_render				( );
			void				draw_octahedron			( );

			tool_light^			m_tool_light;
			render::scene_ptr*	m_scene;
			xrayColor			m_color;
			float				m_range;
			float				m_spot_umbra_angle;
			float				m_spot_penumbra_angle;
			float				m_spot_falloff;
			float				m_intensity;
			float				m_attenuation_power;
			int					m_lighting_model;
			float				m_diffuse_influence_factor;
			float				m_specular_influence_factor;

			bool				m_is_shadower;
			bool				m_use_with_lpv;
			u32					m_sun_shadow_map_size;
			int					m_shadow_map_size_index;
			
			u32					m_num_sun_cascades;
			bool				m_is_cast_shadow;
			
			bool				m_is_cast_shadow_x;
			bool				m_is_cast_shadow_neg_x;
			bool				m_is_cast_shadow_y;
			bool				m_is_cast_shadow_neg_y;
			bool				m_is_cast_shadow_z;
			bool				m_is_cast_shadow_neg_z;
			
			float				m_z_bias;
			
			float				m_shadow_transparency;
			
			float				m_time_of_day;
			u32					m_year;
			u32					m_month;
			u32					m_day;
			u32					m_hours;
			u32					m_minutes;
			u32					m_seconds;
			bool				m_use_auto_lacation;
			
			render::light_type	m_type;
public:
	typedef xray::editor::wpf_controls::property_editors::attributes::value_range_and_format_attribute	value_range_and_format_attribute;
	typedef xray::editor::wpf_controls::property_editors::attributes::vector3_attribute					vector3_attribute;
	typedef xray::editor::wpf_controls::property_grid::refresh_grid_on_change_attribute					refresh_grid_on_change_attribute;
	typedef System::Windows::Media::Media3D::Vector3D													Vector3D;
	/*
	[System::ComponentModel::DisplayNameAttribute("time of day"), DescriptionAttribute("time of day"), CategoryAttribute("location")]
	[value_range_and_format_attribute(0.0f, 100.0f, 0.01f)]
	property float			time_of_day{
		float				get		( )				{ return m_time_of_day; }
		void				set		( float v );
	}
	
	[System::ComponentModel::DisplayNameAttribute("year"), DescriptionAttribute("year"), CategoryAttribute("location")] 
	[value_range_and_format_attribute(0, 3000, 1) ]
	property u32			year{
		u32					get		( )				{ return m_year; }
		void				set		( u32 v );
	}
	
	[System::ComponentModel::DisplayNameAttribute("use auto lacation"), DescriptionAttribute("use_auto_lacation"), CategoryAttribute("location")] 
	property bool			use_auto_lacation {
		bool				get		( )				{ return m_use_auto_lacation; }
		void				set		( bool v );
	}
	
	[System::ComponentModel::DisplayNameAttribute("month"), DescriptionAttribute("month"), CategoryAttribute("location")] 
	[value_range_and_format_attribute(1, 12, 1) ]
	property u32			month{
		u32					get		( )				{ return m_month; }
		void				set		( u32 v );
	}

	[System::ComponentModel::DisplayNameAttribute("day"), DescriptionAttribute("day"), CategoryAttribute("location")] 
	[value_range_and_format_attribute(1, 31, 1) ]
	property u32			day{
		u32					get		( )				{ return m_day; }
		void				set		( u32 v );
	}

	[System::ComponentModel::DisplayNameAttribute("hours"), DescriptionAttribute("hours"), CategoryAttribute("location")] 
	[value_range_and_format_attribute(0, 24, 1) ]
	property u32			hours{
		u32					get		( )				{ return m_hours; }
		void				set		( u32 v );
	}

	[System::ComponentModel::DisplayNameAttribute("minutes"), DescriptionAttribute("minutes"), CategoryAttribute("location")] 
	[value_range_and_format_attribute(0, 59, 1) ]
	property u32			minutes{
		u32					get		( )				{ return m_minutes; }
		void				set		( u32 v );
	}

	[System::ComponentModel::DisplayNameAttribute("seconds"), DescriptionAttribute("seconds"), CategoryAttribute("location")] 
	[value_range_and_format_attribute(0, 59, 1) ]
	property u32			seconds{
		u32					get		( )				{ return m_seconds; }
		void				set		( u32 v );
	}
*/

	[DisplayNameAttribute("shadow in -Z"), DescriptionAttribute("shadow in negative Z side"), CategoryAttribute("shadow")] 
	property bool			is_cast_shadow_neg_z {
		bool				get		( )				{ return m_is_cast_shadow_neg_z; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("shadow in +Z"), DescriptionAttribute("shadow in positive Z side"), CategoryAttribute("shadow")] 
	property bool			is_cast_shadow_z {
		bool				get		( )				{ return m_is_cast_shadow_z; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("shadow in -Y"), DescriptionAttribute("shadow in negative Y side"), CategoryAttribute("shadow")] 
	property bool			is_cast_shadow_neg_y {
		bool				get		( )				{ return m_is_cast_shadow_neg_y; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("shadow in +Y"), DescriptionAttribute("shadow in positive Y side"), CategoryAttribute("shadow")] 
	property bool			is_cast_shadow_y {
		bool				get		( )				{ return m_is_cast_shadow_y; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("shadow in +X"), DescriptionAttribute("shadow in positive X side"), CategoryAttribute("shadow")] 
	property bool			is_cast_shadow_x {
		bool				get		( )				{ return m_is_cast_shadow_x; }
		void				set		( bool v );
	}
	
	[DisplayNameAttribute("shadow in -X"), DescriptionAttribute("shadow in negative X side"), CategoryAttribute("shadow")] 
	property bool			is_cast_shadow_neg_x {
		bool				get		( )				{ return m_is_cast_shadow_neg_x; }
		void				set		( bool v );
	}

	[DisplayNameAttribute("shadow map cascades count"), DescriptionAttribute("shadow map cascades count"), CategoryAttribute("shadow")] 
	[value_range_and_format_attribute(0, 4, 1) ]
	property u32			num_sun_cascades{
		u32					get		( )				{ return m_num_sun_cascades; }
		void				set		( u32 v );
	}

	[ DisplayNameAttribute("shadow map cascade resolution"), DescriptionAttribute("shadow map cascade resolution"), CategoryAttribute("shadow") ]
	[ refresh_grid_on_change_attribute ]
	property int			shadow_map_size{
		int					get		( )				{ return (int)m_shadow_map_size_index; }
		void				set		( int t );
	}
	
	[DisplayNameAttribute("shadow transparency"), DescriptionAttribute("shadow transparency"), CategoryAttribute("shadow")] 
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 1.0f, 0.0000001f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property float			shadow_transparency{
		float				get		( )				{ return m_shadow_transparency; }
		void				set		( float v );
	}
	
	[DisplayNameAttribute("shadow z bias"), DescriptionAttribute("shadow z bias"), CategoryAttribute("shadow")] 
	[ValueAttribute(ValueAttribute::value_type::e_def_val, 0.0f, 1.0f, 0.0000001f), ReadOnlyAttribute(false),ConnectionAttribute(enum_connection::inout)]
	property float			z_bias{
		float				get		( )				{ return m_z_bias; }
		void				set		( float v );
	}
	
	[DisplayNameAttribute("does cast shadow"), DescriptionAttribute("set true to make light cast shadows"), CategoryAttribute("shadow")] 
	property bool			is_cast_shadow {
		bool				get		( )				{ return m_is_cast_shadow; }
		void				set		( bool v );
	}

	[DisplayNameAttribute("spot falloff"), DescriptionAttribute("spot falloff"), CategoryAttribute("light")] 
	[value_range_and_format_attribute(0.001f, 1000.f, .01f) ]
	property float			spot_falloff{
		float				get		( )				{ return m_spot_falloff; }
		void				set		( float v );
	}

	[DisplayNameAttribute("umbra angle"), DescriptionAttribute("spot light umbra angle"), CategoryAttribute("light")]
	[value_range_and_format_attribute(0.f, 170.f, .01f) ]
	property float			spot_umbra_angle{
		float				get		( )				{ return math::rad2deg(m_spot_umbra_angle); }
		void				set		( float v );
	}

	[DisplayNameAttribute("penumbra angle"), DescriptionAttribute("spot light penumbra angle"), CategoryAttribute("light")] 
	[value_range_and_format_attribute(0.f, 170.f, .01f) ]
	property float			spot_penumbra_angle{
		float				get		( )				{ return math::rad2deg(m_spot_penumbra_angle); }
		void				set		( float v );
	}

	[DisplayNameAttribute("specular intensity factor"), DescriptionAttribute("specular intensity factor"), CategoryAttribute("light")] 
	[value_range_and_format_attribute(-0.0000001f, 1.f, .001f) ]
	property float specular_influence_factor {
		float				get		( )				{ return m_specular_influence_factor; }
		void				set		( float v );
	}

	[DisplayNameAttribute("diffuse intensity factor"), DescriptionAttribute("diffuse intensity factor"), CategoryAttribute("light")] 
	[value_range_and_format_attribute(-0.0000001f, 1.f, .001f) ]
	property float diffuse_influence_factor {
		float				get		( )				{ return m_diffuse_influence_factor; }
		void				set		( float v );
	}

	[DisplayNameAttribute("intensity"), DescriptionAttribute("light intensity"), CategoryAttribute("light")] 
	[value_range_and_format_attribute(0.001f, 1024.f, .01f) ]
	property float			intensity{
		float				get		( )				{ return m_intensity; }
		void				set		( float v );
	}

	[ DisplayNameAttribute("attenuation power") ]
	[ CategoryAttribute("light") ]
	property float attenuation {
		float				get		( )				{ return m_attenuation_power; }
		void				set		( float v );
	}

	[DisplayNameAttribute("range"), DescriptionAttribute("light range"), CategoryAttribute("light")] 
	[value_range_and_format_attribute(0.001f, 1024.f) ]
	property float			range{
		float				get		( )				{ return m_range; }
		void				set		( float v );
	}

	[DisplayNameAttribute("color"), DescriptionAttribute("light color"), CategoryAttribute("light")]
	property xrayColor	color{
		xrayColor			get		( )				{ return m_color; }
		void				set		( xrayColor c );
	}
	
	[DisplayNameAttribute("is shadower"), DescriptionAttribute("is shadower"), CategoryAttribute("light")] 
	property bool			is_shadower {
		bool				get		( )				{ return m_is_shadower; }
		void				set		( bool v );
	}

	[DisplayNameAttribute("indirect lighting"), DescriptionAttribute("indirect lighting use with light propagation volumes"), CategoryAttribute("light")] 
	property bool			use_with_lpv {
		bool				get		( )				{ return m_use_with_lpv; }
		void				set		( bool v );
	}

	
	[DisplayNameAttribute("lighting model"), DescriptionAttribute("lighting model specifies BRDF: Bidirectional Reflection Distribution Function"), CategoryAttribute("light")]
	property int		lighting_model{
		int					get		( )				{ return m_lighting_model; }
		void				set		( int lighting_model );
	}

	[ DisplayNameAttribute("type"), DescriptionAttribute("light type"), CategoryAttribute("light") ]
	[ refresh_grid_on_change_attribute ]
	property int			light_type{
		int					get		( )				{ return (int)m_type; }
		void				set		( int t );
	}
private:
	wpf_controls::property_container^ m_current_property_container;
}; // class object_light


} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_LIGHT_H_INCLUDED
