////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_light.h"

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

void object_light::fill_props( render::light_props& props )
{
	props.does_cast_shadows		= false;
	props.range					= m_range;
	props.color					= m_color.math_color().m_value;
	props.transform				= *m_transform;
	props.type					= m_type;
	props.attenuation_power		= m_attenuation_power;
	props.lighting_model		= m_lighting_model;
	props.intensity				= m_intensity;
	props.spot_umbra_angle		= m_spot_umbra_angle;
	props.spot_penumbra_angle	= m_spot_penumbra_angle;
	props.spot_falloff			= m_spot_falloff;
	props.diffuse_influence_factor	= m_diffuse_influence_factor;
	props.use_with_lpv			= m_use_with_lpv;
	
	props.specular_influence_factor	= m_specular_influence_factor;
	props.shadower				= m_is_shadower;
	props.sun_shadow_map_size	= m_sun_shadow_map_size;
	props.num_sun_cascades		= m_num_sun_cascades;
	props.does_cast_shadows		= m_is_cast_shadow;
	
	props.shadow_map_size_index = m_shadow_map_size_index;
	
	props.shadow_distribution_sides[0]	= m_is_cast_shadow_x;
	props.shadow_distribution_sides[1]	= m_is_cast_shadow_neg_x;
	props.shadow_distribution_sides[2]	= m_is_cast_shadow_y;
	props.shadow_distribution_sides[3]	= m_is_cast_shadow_neg_y;
	props.shadow_distribution_sides[4]	= m_is_cast_shadow_z;
	props.shadow_distribution_sides[5]	= m_is_cast_shadow_neg_z;
	
	props.local_light_z_bias			= m_z_bias;
	

	props.m_year = m_year;
	props.m_month = m_month;
	props.m_day = m_day;
	props.m_hours = m_hours;
	props.m_minutes = m_minutes;
	props.m_seconds = m_seconds;
	props.m_use_auto_lacation = m_use_auto_lacation;
	props.shadow_transparency			= m_shadow_transparency;
}

void object_light::sync_render( )
{
	if(get_visible())
	{
		xray::render::light_props props;
		fill_props( props );
		get_editor_renderer().scene().update_light( *m_scene, id(), props );
	}
}
/*
void object_light::time_of_day::set( float v )
{
	if (v < 0.0f)
		return;
	
	u32 const seconds	= u32(v * 36.0f * 24.0f);
	
	m_hours				= seconds / 3600;
	m_minutes			= (seconds - m_hours * 3600) / 60;
	m_seconds			= seconds - m_hours * 3600 - m_minutes * 60;
	
	m_time_of_day		= v;
	
	on_property_changed	( "time_of_day" );
	sync_render			( );
}

void object_light::use_auto_lacation::set( bool v )
{
	m_use_auto_lacation = v;
	on_property_changed	( "use_auto_lacation" );
	sync_render			( );

}

void object_light::year::set( u32 v )
{
	m_year = v;
	on_property_changed	( "year" );
	sync_render			( );
}

void object_light::month::set( u32 v )
{
	m_month = v;
	on_property_changed	( "month" );
	sync_render			( );
}

void object_light::day::set( u32 v )
{
	m_day = v;
	on_property_changed	( "day" );
	sync_render			( );
}

void object_light::hours::set( u32 v )
{
	m_hours = v;
	on_property_changed	( "hours" );
	sync_render			( );
}

void object_light::minutes::set( u32 v )
{
	m_minutes = v;
	on_property_changed	( "minutes" );
	sync_render			( );
}

void object_light::seconds::set( u32 v )
{
	m_seconds = v;
	on_property_changed	( "seconds" );
	sync_render			( );
}
*/
void object_light::attenuation::set( float value )
{
	m_attenuation_power	= value;
	on_property_changed	( "attenuation" );
	sync_render			( );
}

void object_light::lighting_model::set( int value )
{
	m_lighting_model	= value;
	on_property_changed	( "lighting_model" );
	sync_render			( );
}

void object_light::range::set( float v )
{
	m_range				= v;
	on_property_changed( "range" );
	sync_render			( );
}

void object_light::diffuse_influence_factor::set( float v )
{
	m_diffuse_influence_factor = v;
	on_property_changed	( "diffuse_influence_factor" );
	sync_render			( );
}

void object_light::use_with_lpv::set( bool v )
{
	m_use_with_lpv = v;
	on_property_changed	( "use_with_lpv" );
	sync_render			( );
}


void object_light::specular_influence_factor::set( float v )
{
	m_specular_influence_factor = v;
	on_property_changed	( "specular_influence_factor" );
	sync_render			( );
}

void object_light::is_shadower::set( bool v )
{
	m_is_shadower = v;
	on_property_changed	( "is_shadower" );
	sync_render			( );
}

void object_light::is_cast_shadow::set( bool v )
{
	m_is_cast_shadow = v;
	on_property_changed	( "is_cast_shadow" );
	sync_render		 ( );
}

void object_light::is_cast_shadow_x::set( bool v )
{
	m_is_cast_shadow_x = v;
	on_property_changed	( "is_cast_shadow_x" );
	sync_render		 ( );
}

void object_light::is_cast_shadow_neg_x::set( bool v )
{
	m_is_cast_shadow_neg_x = v;
	on_property_changed	( "is_cast_shadow_neg_x" );
	sync_render		 ( );
}

void object_light::is_cast_shadow_y::set( bool v )
{
	m_is_cast_shadow_y = v;
	on_property_changed	( "is_cast_shadow_y" );
	sync_render		 ( );
}

void object_light::is_cast_shadow_neg_y::set( bool v )
{
	m_is_cast_shadow_neg_y = v;
	on_property_changed	( "is_cast_shadow_neg_y" );
	sync_render		 ( );
}

void object_light::is_cast_shadow_z::set( bool v )
{
	m_is_cast_shadow_z = v;
	on_property_changed	( "is_cast_shadow_z" );
	sync_render		 ( );
}

void object_light::is_cast_shadow_neg_z::set( bool v )
{
	m_is_cast_shadow_neg_z = v;
	on_property_changed	( "is_cast_shadow_neg_z" );
	sync_render		 ( );
}

void object_light::color::set( xrayColor c )			
{
	m_color				= c;
	on_property_changed	( "color" );
	sync_render			( );
}

void object_light::light_type::set( int t )
{
	m_type				= (render::light_type)t;
	on_property_changed	( "type" );
	sync_render			( );
}

void object_light::spot_umbra_angle::set( float v )
{
	m_spot_umbra_angle	= math::deg2rad(v);
	on_property_changed	( "spot_umbra_angle" );

	if ( m_spot_umbra_angle > m_spot_penumbra_angle )
		spot_penumbra_angle	= v;

	sync_render			( );
}

void object_light::spot_penumbra_angle::set( float v )
{
	m_spot_penumbra_angle	= math::deg2rad(v);
	on_property_changed	( "spot_penumbra_angle" );

	if ( m_spot_penumbra_angle < m_spot_umbra_angle )
		spot_umbra_angle	= v;

	sync_render			( );
}

void object_light::spot_falloff::set( float v )
{
	m_spot_falloff		= v;
	on_property_changed	( "spot_falloff" );
	sync_render			( );
}

void object_light::z_bias::set( float v )
{
	m_z_bias		= v;
	on_property_changed	( "z_bias" );
	sync_render			( );
}

void object_light::shadow_transparency::set( float v )
{
	m_shadow_transparency	= v;
	on_property_changed		( "shadow_transparency" );
	sync_render				( );
}

void object_light::intensity::set( float v )
{
	m_intensity			= v;
	on_property_changed	( "intensity" );
	sync_render			( );
}

//void object_light::sun_shadow_map_size::set( float v )
//{
//	m_sun_shadow_map_size	= v;
//	on_property_changed	( "sun_shadow_map_size" );
//	sync_render			( );
//}

void object_light::shadow_map_size::set( int t )
{
	m_shadow_map_size_index	= t;
	on_property_changed	( "shadow_map_size_index" );
	sync_render			( );
}


void object_light::num_sun_cascades::set( u32 v )
{
	m_num_sun_cascades	= v;
	on_property_changed	( "num_sun_cascades" );
	sync_render			( );
}

void object_light::load_contents( )
{
	if(!m_collision->initialized())
		initialize_collision();
}

wpf_controls::property_container^ object_light::get_property_container( )
{
	wpf_controls::property_container^ container = super::get_property_container();
	m_current_property_container	= container;

	container->properties->remove( "general/scale" );
	
	if ( (m_type != render::light_type_spot) && (m_type != render::light_type_plane_spot) ) {
		container->properties->remove( "light/umbra angle" );
		container->properties->remove( "light/penumbra angle" );
		container->properties->remove( "light/spot falloff" );
	}
	
	if ( m_type != render::light_type_parallel ) {
		container->properties->remove( "shadow/shadow map cascades count" );
	}
	
	if ( m_type != render::light_type_point ) {
		container->properties->remove( "light/is shadower" );
	}
	
	if ( m_type != render::light_type_point		&& 
		 m_type != render::light_type_sphere	&& 
		 m_type != render::light_type_obb) {
		container->properties->remove( "shadow/shadow in +X" );
		container->properties->remove( "shadow/shadow in -X" );
		container->properties->remove( "shadow/shadow in +Y" );
		container->properties->remove( "shadow/shadow in -Y" );
		container->properties->remove( "shadow/shadow in +Z" );
		container->properties->remove( "shadow/shadow in -Z" );
	}
	
	if ( m_type != render::light_type_parallel ) {
		container->properties["shadow/shadow map cascade resolution"]->dynamic_attributes->add( 
			gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(3){ "1024", "512", "256" } ) 
		);
	}
	else {
		container->properties["shadow/shadow map cascade resolution"]->dynamic_attributes->add( 
			gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(5){ "2048", "1024", "512", "256", "128" } ) 
		);
	}

	if ( m_type == render::light_type_parallel ) {
		container->properties->remove( "light/attenuation power" );
		container->properties->remove( "light/range" );
	}
	
	container->properties["light/type"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(7){ "point", "spot", "box", "capsule", "parallel", "sphere", "plane_spot" } ) 
	);

	container->properties["light/lighting model"]->dynamic_attributes->add( 
		gcnew wpf_controls::property_editors::attributes::combo_box_items_attribute( gcnew cli::array<System::String^, 1>(4){ "Phong", "Blinn-Phong", "Hoffman", "Szirmay-Kalos" } ) 
	);

	return				container;
}

void object_light::on_selected	( bool is_selected )
{
	if ( !is_selected )
		m_current_property_container	= nullptr;
}

void object_light::set_visible( bool bvisible )
{
	ASSERT(bvisible!=get_visible());

	if(bvisible)
	{
		xray::render::light_props props;
		fill_props( props );
		get_editor_renderer().scene().add_light		(*m_scene, id(), props);
	}
	else
		get_editor_renderer().scene().remove_light	(*m_scene, id());

	super::set_visible	( bvisible );
}

void object_light::set_transform( float4x4 const& transform )
{
	float4x4 new_transform			= transform;
	switch ( m_type ) {
		case render::light_type_point : {
			break;
		}
		case render::light_type_spot : {
			break;
		}
		case render::light_type_obb : {
			break;
		}
		case render::light_type_parallel : {
			break;
		}
		case render::light_type_capsule : {
			float3 const& new_scale = new_transform.get_scale( );
			float3 old_scale		= m_transform->get_scale();
			if ( !math::is_similar(old_scale.x, old_scale.z, math::epsilon_5) )
				old_scale.x	= old_scale.z	= math::min(old_scale.x, old_scale.z);

			float new_sphere_radius	= -1.f;
			if ( new_scale.x != old_scale.x )
				new_sphere_radius	= new_scale.x;
			
			if ( new_scale.z != old_scale.x )
				new_sphere_radius	= math::max( new_sphere_radius, new_scale.z );

			if ( new_sphere_radius == -1.f )
				new_sphere_radius	= new_scale.x;

			new_transform.set_scale	( float3(new_sphere_radius, new_scale.y, new_sphere_radius) );

			break;
		}
		case render::light_type_sphere : {
			float3 const& new_scale = new_transform.get_scale( );

			float3 const& old_scale = m_transform->get_scale();
			R_ASSERT				( math::is_similar(old_scale.x, old_scale.y, math::epsilon_5) );
			R_ASSERT				( math::is_similar(old_scale.x, old_scale.z, math::epsilon_5) );

			float new_sphere_radius	= -1.f;
			if ( new_scale.x != old_scale.x )
				new_sphere_radius	= new_scale.x;
			
			if ( new_scale.y != old_scale.x )
				new_sphere_radius	= math::max( new_sphere_radius, new_scale.y );
			
			if ( new_scale.z != old_scale.x )
				new_sphere_radius	= math::max( new_sphere_radius, new_scale.z );

			if ( new_sphere_radius == -1.f )
				new_sphere_radius	= new_scale.x;

			new_transform.set_scale	( float3(new_sphere_radius, new_sphere_radius, new_sphere_radius) );
			break;
		}
		case render::light_type_plane_spot : {
			break;
		}
		default : NODEFAULT( );
	}

	super::set_transform( new_transform );
	
	sync_render			( );
}

} // namespace editor
} // namespace xray