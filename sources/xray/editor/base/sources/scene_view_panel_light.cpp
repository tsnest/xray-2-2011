////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_view_panel.h"

#pragma managed( push, off )
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray{
namespace editor_base{

void fill_camera_light_props( render::light_props& props, math::float4x4 const& m )
{
	props.does_cast_shadows			= false;
	props.range						= 10.0f;
	props.color						= math::color(255,255,255,255).m_value;// white
	props.transform					= m;
	props.type						= xray::render::light_type_point;
	props.attenuation_power			= 2.f;
	props.intensity					= 5;
	props.spot_umbra_angle			= 0;
	props.spot_penumbra_angle		= 0;
	props.spot_falloff				= 0;
	props.diffuse_influence_factor	= 1;
	props.specular_influence_factor	= 1;
	props.lighting_model			= 1;
	props.shadower					= false;
	props.use_with_lpv				= false;
}

void fill_sun_light_props( render::light_props& props, math::float4x4 const& m )
{
	props.does_cast_shadows		= true;
	props.range					= 300.0f;
	props.color					= math::color( 255, 255, 255, 255 ).m_value;
	props.transform				= m;
	props.type					= render::light_type_parallel;
	props.attenuation_power		= 2;
	props.lighting_model		= 3;
	props.intensity				= 2.8f;
	props.spot_umbra_angle		= 0.39f;
	props.spot_penumbra_angle	= 0.78f;
	props.spot_falloff			= 2;
	props.diffuse_influence_factor	= 1;
	props.specular_influence_factor	= 1;
	
	props.sun_shadow_map_size	= 512;
	props.num_sun_cascades		= 4;
	
	props.shadow_map_size_index = 0;
	
	props.shadow_distribution_sides[0]	= true;
	props.shadow_distribution_sides[1]	= true;
	props.shadow_distribution_sides[2]	= true;
	props.shadow_distribution_sides[3]	= true;
	props.shadow_distribution_sides[4]	= true;
	props.shadow_distribution_sides[5]	= true;
	props.local_light_z_bias			= 0.000001f;
	props.shadow_transparency			= 0.1f;

	props.shadower						= false;
	props.use_with_lpv					= false;
}

u32 g_light_id = 20000;
void scene_view_panel::switch_camera_light( )
{
	if(!get_camera_light())
	{
		m_camera_light_id				= ++g_light_id;
		render::light_props				props;
		fill_camera_light_props			( props, create_translation( m_inverted_view->c.xyz()) );
		editor_renderer()->scene().add_light	( *m_scene, m_camera_light_id, props );
	}else
	{
		editor_renderer()->scene().remove_light	( *m_scene, m_camera_light_id );
		m_camera_light_id				= 0;
	}
}

void scene_view_panel::update_camera_ligtht( )
{
	ASSERT( get_camera_light() );

	render::light_props				props;
	fill_camera_light_props			( props, create_translation( m_inverted_view->c.xyz()) );
	editor_renderer()->scene().update_light	( *m_scene, m_camera_light_id, props );
}

void scene_view_panel::switch_sun( )
{
	if(m_sun_light_id==0)
		return;

	if(!get_sun_active())
	{
		m_sun_light_id					= ++g_light_id;
		render::light_props				props;
		fill_sun_light_props			( props, create_rotation( float3(-math::pi_d2, 0, 0) ));
		editor_renderer()->scene().add_light	( *m_scene, m_sun_light_id, props );
	}else
	{
		editor_renderer()->scene().remove_light	( *m_scene, m_sun_light_id );
		m_sun_light_id					= 1;
	}
}

} // namespace editor_base
} // namespace xray

