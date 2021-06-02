////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "particle_actions.h"
#include "particle_emitter_instance.h"

namespace xray {
namespace particle {

template <class ConfigValueType>
void particle_action::load_impl( ConfigValueType const& config)
{
	m_visibility = read_config_value(config, "Enabled", true);
}

void particle_action::load	(configs::binary_config_value const& config)
{
	load_impl( config );
}

#ifndef	MASTER_GOLD
void particle_action::load	(configs::lua_config_value const& config)
{
	load_impl( config );
}
#endif // #ifndef	MASTER_GOLD

void particle_action::set_defaults( bool ) 
{
	m_next = 0;
	m_visibility = true;
}

static enum_particle_screen_alignment screen_alignment_name_to_type(fixed_string<128> const& name)
{
		 if (name=="Square")	return particle_screen_alignment_square;
	else if (name=="Rectangle")	return particle_screen_alignment_rectangle;
	else if (name=="ToPath")	return particle_screen_alignment_to_path;
	else if (name=="ToAxis")	return particle_screen_alignment_to_axis;
	
	return particle_screen_alignment_rectangle;
}
static enum_particle_locked_axis locked_axis_name_to_type(fixed_string<128> const& name)
{
		 if (name=="X")			return particle_locked_axis_x;
	else if (name=="Y")			return particle_locked_axis_y;
	else if (name=="Z")			return particle_locked_axis_z;
	else if (name=="-X")		return particle_locked_axis_negative_x;
	else if (name=="-Y")		return particle_locked_axis_negative_y;
	else if (name=="-Z")		return particle_locked_axis_negative_z;
	else if (name=="RotateX")	return particle_locked_axis_rotate_x;
	else if (name=="RotateY")	return particle_locked_axis_rotate_y;
	else if (name=="RotateZ")	return particle_locked_axis_rotate_z;
	
	return particle_locked_axis_none;
}
static enum_particle_subuv_method sub_uv_method_name_to_type(fixed_string<128> const& name)
{
	// smOth ? FIXIT
		 if (name=="Linear")		return particle_subuv_method_linear;
	else if (name=="LinearSmooth")	return particle_subuv_method_linear_smooth;
	else if (name=="Random")		return particle_subuv_method_random;
	else if (name=="RandomSmooth")	return particle_subuv_method_random_smooth;

	return particle_subuv_method_linear;
}

template <class ConfigValueType>
void particle_action_billboard::load_impl( ConfigValueType const& prop_config )
{
	m_use_sub_uv			= read_config_value(prop_config, "UseSubUV", m_use_sub_uv);
	m_sub_image_horizontal  = read_config_value(prop_config, "SubImgHorizontal", m_sub_image_horizontal);
	m_sub_image_vertical	= read_config_value(prop_config, "SubImgVertical", m_sub_image_vertical);
	
	m_use_movie				= read_config_value(prop_config, "UseMovie", m_use_movie);
	m_movie_frame_rate		= read_config_value(prop_config, "MovieFrameRate", m_movie_frame_rate);
	m_movie_start_frame		= read_config_value(prop_config, "MovieStartFrame", m_movie_start_frame);
	
	m_billboard_parameters.sub_image_changes	= read_config_value(prop_config, "SubImgChange", m_billboard_parameters.sub_image_changes);
	m_billboard_parameters.sub_image_horizontal = m_sub_image_horizontal;
	m_billboard_parameters.sub_image_vertical	= m_sub_image_vertical;
	
	
	if (prop_config.value_exists("ScreenAlignment"))
		m_billboard_parameters.screen_alignment		= screen_alignment_name_to_type(pcstr(prop_config["ScreenAlignment"]));
	
	if (prop_config.value_exists("LockAxisFlags"))
		m_billboard_parameters.locked_axis		= locked_axis_name_to_type(pcstr(prop_config["LockAxisFlags"]));
	
	if (prop_config.value_exists("Method"))
		m_billboard_parameters.subuv_method		= sub_uv_method_name_to_type(pcstr(prop_config["Method"]));
	
	
	m_subimage_index.load(prop_config["SubImgIndex"]);
}

u32 particle_action_billboard::save_binary( xray::mutable_buffer& buffer, bool calc_size)
{
	return m_subimage_index.save_binary(buffer, calc_size);
}

void particle_action_billboard::load_binary( xray::mutable_buffer& buffer)
{
	m_subimage_index.load_binary(buffer);
}

void particle_action_billboard::load( configs::binary_config_value const& prop_config )
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_billboard::load( configs::lua_config_value const& prop_config )
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}
#endif

void particle_action_billboard::set_defaults( bool mt_alloc )
{
	particle_action_data_type::set_defaults(mt_alloc);
	
	m_billboard_parameters.screen_alignment		= particle_screen_alignment_rectangle;
	m_billboard_parameters.locked_axis			= particle_locked_axis_x;
	m_billboard_parameters.subuv_method			= particle_subuv_method_linear;
	m_billboard_parameters.sub_image_horizontal = 0;
	m_billboard_parameters.sub_image_vertical	= 0;
	m_billboard_parameters.sub_image_changes	= 0;
	m_use_sub_uv			= false;
	
	m_sub_image_horizontal	= 0;
	m_sub_image_vertical	= 0;

	m_use_movie				= false;
	m_movie_frame_rate		= 30.0f;
	m_movie_start_frame		= 0.0f;

	m_subimage_index.set_defaults();
}
 
void particle_action_billboard::init(particle_emitter_instance* instance, base_particle* P, float time_delta)
{
	XRAY_UNREFERENCED_PARAMETERS(instance, time_delta);
	
	if (!m_use_sub_uv)
		return;
	
	float const num_images = float(m_billboard_parameters.sub_image_horizontal * m_billboard_parameters.sub_image_vertical);
	
	if (m_use_movie)
	{
		if (math::is_similar(m_movie_start_frame, 0.0f, math::epsilon_3))
			m_movie_start_frame = random_float(0.0f, num_images);
		else
		{
			m_movie_start_frame = m_movie_start_frame - 1.0f;
			math::clamp(m_movie_start_frame, 0.0f, num_images);
		}
		
		P->subimage_index = m_movie_start_frame;
		
		if (P->subimage_index > num_images)
			P->subimage_index = 0.0f;
		
		P->next_subimage_index = P->subimage_index + 1.0f;
	}
}

static float frac(float f)
{
	return f - (u32)f;
}

void particle_action_billboard::update(particle_emitter_instance* instance, base_particle* P, float time_delta)
{
	XRAY_UNREFERENCED_PARAMETERS(instance);
	
	if (!m_use_sub_uv)
		return;
	
	float const num_images = float(m_billboard_parameters.sub_image_horizontal * m_billboard_parameters.sub_image_vertical);
	
	if (m_use_movie)
	{
		P->subimage_index += time_delta * m_movie_frame_rate;
		
		if (P->subimage_index > num_images)
			P->subimage_index = 0.0f;
		
		P->next_subimage_index = P->subimage_index + 1.0f;
	}
	else
	{
		if (m_billboard_parameters.subuv_method==particle_subuv_method_random || 
			m_billboard_parameters.subuv_method==particle_subuv_method_random_smooth)
		{
			P->next_subimage_index = P->subimage_index;
			
			float const num_images_per_lifetime = float(m_billboard_parameters.sub_image_changes);
			float const particle_time = frac(P->get_lifetime());
			
			if (math::is_similar(particle_time, 0.0f, math::epsilon_3) &&
				math::is_similar(num_images_per_lifetime, 0.0f, math::epsilon_3))
			{
				P->subimage_index = random_float(0.0f, num_images);
				return;
			}
			
			float const f_position = particle_time * num_images_per_lifetime;
			float const	i_position = float(math::floor(particle_time * num_images_per_lifetime));
			
			if (i_position <= f_position && f_position < i_position + time_delta * num_images_per_lifetime)
			{
				P->subimage_index = random_float(0.0f, num_images);
			}
		}
		else
		{
			P->subimage_index = m_subimage_index.evaluate(frac(P->get_lifetime()), 0.0f, range_time_type, P->seed());
			P->next_subimage_index = P->subimage_index + 1.0f;
		}
	}
}









template <class ConfigValueType>
void particle_action_trail::load_impl(ConfigValueType const& prop_config)
{
	m_screen_alignment		= screen_alignment_name_to_type	(read_config_value<pcstr>(prop_config, "ScreenAlignment", ""));
	
	m_beamtrail_parameters.num_sheets			= read_config_value(prop_config, "SheetsCount", m_beamtrail_parameters.num_sheets);
	m_beamtrail_parameters.num_texture_tiles	= read_config_value(prop_config, "TextureTile", m_beamtrail_parameters.num_texture_tiles);
}

void particle_action_trail::load(configs::binary_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_trail::load(configs::lua_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}
#endif

void particle_action_trail::set_defaults(bool mt_alloc)
{
	particle_action_data_type::set_defaults(mt_alloc);
	
	m_screen_alignment = particle_screen_alignment_rectangle;
	m_beamtrail_parameters.num_sheets			= 0;
	m_beamtrail_parameters.num_texture_tiles	= 0;
}


void particle_action_trail::update(particle_emitter_instance* instance, base_particle* P, float time_delta)
{
	XRAY_UNREFERENCED_PARAMETERS(instance,P,time_delta);
}







// void particle_action_beam::tick(particle_emitter_instance* instance, float time_delta)
// {
// 	XRAY_UNREFERENCED_PARAMETERS(instance,time_delta);
// }

template <class ConfigValueType>
void particle_action_beam::load_impl(ConfigValueType const& prop_config)
{
	m_beamtrail_parameters.num_sheets			= read_config_value(prop_config, "SheetsCount", m_beamtrail_parameters.num_sheets);
	m_beamtrail_parameters.num_texture_tiles	= read_config_value(prop_config, "TextureTile", m_beamtrail_parameters.num_texture_tiles);
	m_beamtrail_parameters.num_beams			= read_config_value(prop_config, "BeamsCount", m_beamtrail_parameters.num_beams);
	
	m_speed										= read_config_value(prop_config, "Speed", m_speed);
	m_noise										= read_config_value(prop_config, "Noise", m_noise);
	m_frequency									= read_config_value(prop_config, "Frequency", m_frequency);
	
	math::clamp<u32>(m_beamtrail_parameters.num_sheets, 0, 1000);
	math::clamp<u32>(m_beamtrail_parameters.num_texture_tiles, 0, 1000);
	math::clamp<u32>(m_beamtrail_parameters.num_beams, 1, 1000);
	math::clamp(m_speed, 0.0f, 1000.0f);
	math::clamp(m_noise, 0.0f, 1000.0f);
	math::clamp(m_frequency, 0.0f, 1000.0f);
}

void particle_action_beam::load(configs::binary_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_beam::load(configs::lua_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}
#endif

void particle_action_beam::set_defaults(bool mt_alloc)
{
	particle_action_data_type::set_defaults(mt_alloc);
	
	m_beamtrail_parameters.num_sheets			= 1;
	m_beamtrail_parameters.num_texture_tiles	= 1;
	m_beamtrail_parameters.num_beams			= 1;
	m_speed										= 0;
	m_noise										= 1;
	m_frequency									= 0;
}





void particle_event::set_defaults(bool)
{
	m_child_ps_index		= -1;
	m_inherit_position		= false;
}

template <class ConfigValueType>
void particle_event::load_impl(ConfigValueType const& config)
{
	m_inherit_position = read_config_value<bool>(config, "InheritPosition", m_inherit_position);
}

void particle_event::load(configs::binary_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_event::load(configs::lua_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}
#endif

void particle_event_on_play::set_defaults(bool mt_alloc)
{
	particle_event::set_defaults(mt_alloc);
	// TODO: remove particle_event_on_play?
}

void particle_event_on_birth::set_defaults(bool mt_alloc)
{
	particle_event::set_defaults(mt_alloc);
	// TODO: remove particle_event_on_birth?
}

void particle_event_on_death::set_defaults(bool mt_alloc)
{
	particle_event::set_defaults(mt_alloc);
	// TODO: remove particle_event_on_death?
}

void particle_event_on_collide::set_defaults(bool mt_alloc)
{
	particle_event::set_defaults(mt_alloc);
	// TODO: remove particle_event_on_collide?
}

void particle_action_source::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_domain.set_defaults();
}

template <class ConfigValueType>
void particle_action_source::load_impl(ConfigValueType const& prop_config)
{
	m_domain.load(prop_config);
}

void particle_action_source::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_source::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

void particle_action_source::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(time, instance);
	
	float3 pos = m_domain.generate();
	
	P->spawn_position += pos;
	P->position += pos;
	P->old_position += pos;
}

xray::math::float4x4 
particle_action_source::get_transform() const
{
	return m_domain.get_transform();
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_initial_color::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	
	m_init_color.set_defaults();
}

void particle_action_initial_color::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(time);
	P->color = m_init_color.evaluate(instance->get_linear_emitter_time(), float4(0.0f,0.0f,0.0f,1.0f));
}


template <class ConfigValueType>
void particle_action_initial_color::load_impl(ConfigValueType const& prop_config)
{
	m_init_color.load(prop_config["InitColor"]);
}

void particle_action_initial_color::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_initial_color::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_initial_color::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_init_color.save_binary(buffer, calc_size);
}

void particle_action_initial_color::load_binary(xray::mutable_buffer& buffer)
{
	m_init_color.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_initial_rotation::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_init_rotation.set_defaults();
}

void particle_action_initial_rotation::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	float3 rotation = xray::math::pi_x2 * m_init_rotation.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed());
	
	P->rotation		= P->rotation  + rotation.x;
	P->rotationY	= P->rotationY + rotation.y;
	P->rotationZ	= P->rotationZ + rotation.z;
}

template <class ConfigValueType>
void particle_action_initial_rotation::load_impl(ConfigValueType const& prop_config)
{
	m_init_rotation.load(prop_config["InitRotation"]);
}

void particle_action_initial_rotation::load(configs::binary_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_initial_rotation::load(configs::lua_config_value const& prop_config)
{
	particle_action::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_initial_rotation::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_init_rotation.save_binary(buffer, calc_size);
}

void particle_action_initial_rotation::load_binary(xray::mutable_buffer& buffer)
{
	m_init_rotation.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_rotation_over_lifetime::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_rotation_over_life.set_defaults();
}

void particle_action_rotation_over_lifetime::update( particle_emitter_instance* instance, base_particle* P, float )
{
	XRAY_UNREFERENCED_PARAMETERS(instance);
	
	float3 rotation = m_rotation_over_life.evaluate(P->get_linear_lifetime(), float3(1.0f, 1.0f, 1.0f), range_time_type, P->seed());
	
	P->rotationX = P->rotationX * rotation.x;
	P->rotationY = P->rotationY * rotation.y;
	P->rotationZ = P->rotationZ * rotation.z;
}

template <class ConfigValueType>
void particle_action_rotation_over_lifetime::load_impl(ConfigValueType const& prop_config)
{
	m_rotation_over_life.load(prop_config["RotationOverLife"]);
}

void particle_action_rotation_over_lifetime::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_rotation_over_lifetime::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_rotation_over_lifetime::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_rotation_over_life.save_binary(buffer, calc_size);
}

void particle_action_rotation_over_lifetime::load_binary(xray::mutable_buffer& buffer)
{
	m_rotation_over_life.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_rotation_over_velocity::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_rotation_over_velocity.set_defaults();
	m_affect_x = false;
	m_affect_y = false;
	m_affect_z = false;
}

void particle_action_rotation_over_velocity::update( particle_emitter_instance* instance, base_particle* P, float )
{
	XRAY_UNREFERENCED_PARAMETERS(instance);
	
	float3 rot_ang = P->velocity.length() * m_rotation_over_velocity.evaluate(P->get_linear_lifetime(), float3(0.0f,0.0f,0.0f), range_time_type, P->seed());
	
	// flip z/x?
	if (m_affect_x) P->rotation  *= rot_ang.x;
	if (m_affect_y) P->rotationY *= rot_ang.y;
	if (m_affect_z) P->rotationZ *= rot_ang.z;
}

template <class ConfigValueType>
void particle_action_rotation_over_velocity::load_impl(ConfigValueType const& prop_config)
{
	m_affect_x = read_config_value(prop_config, "AffectX", m_affect_x);
	m_affect_y = read_config_value(prop_config, "AffectY", m_affect_y);
	m_affect_z = read_config_value(prop_config, "AffectZ", m_affect_z);
	
	m_rotation_over_velocity.load(prop_config["RotationOverVelocity"]);
}

void particle_action_rotation_over_velocity::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_rotation_over_velocity::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_rotation_over_velocity::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_rotation_over_velocity.save_binary(buffer, calc_size);
}

void particle_action_rotation_over_velocity::load_binary(xray::mutable_buffer& buffer)
{
	m_rotation_over_velocity.load_binary(buffer);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_initial_velocity::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_init_velocity.set_defaults();
}

void particle_action_initial_velocity::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	float3 vel			= m_init_velocity.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed());
	P->start_velocity	= instance->m_transform.transform_direction( vel );
	P->velocity			+= P->start_velocity;
}

template <class ConfigValueType>
void particle_action_initial_velocity::load_impl(ConfigValueType const& prop_config)
{
	m_init_velocity.load(prop_config["InitVelocity"]);
}

void particle_action_initial_velocity::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_initial_velocity::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_initial_velocity::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_init_velocity.save_binary(buffer, calc_size);
}

void particle_action_initial_velocity::load_binary(xray::mutable_buffer& buffer)
{
	m_init_velocity.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_initial_rotation_rate::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_init_rate.set_defaults();
}

void particle_action_initial_rotation_rate::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	P->start_rotation_rate	= m_init_rate.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed());
	P->rotation_rate		= P->start_rotation_rate;
}

template <class ConfigValueType>
void particle_action_initial_rotation_rate::load_impl(ConfigValueType const& prop_config)
{
	m_init_rate.load(prop_config["InitRotRate"]);
}

void particle_action_initial_rotation_rate::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_initial_rotation_rate::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_initial_rotation_rate::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_init_rate.save_binary(buffer, calc_size);
}

void particle_action_initial_rotation_rate::load_binary(xray::mutable_buffer& buffer) 
{
	m_init_rate.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_random_velocity::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);

	m_domain.set_defaults();
	m_domain.m_translate  = float3(0,10,0);
	m_domain.m_scale	  = float3(1,1,1);
	m_domain.m_rotate	  = float3(0,0,0);
	m_domain.m_inner_radius = 0.5f;
	m_domain.m_outer_radius = 0.5f;
	m_domain.m_domain_type = particle_domain_complex::sphere_domain;
	m_velocity_multiplier  = 1.0f;
}

void particle_action_random_velocity::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);

	P->start_velocity = instance->m_transform.transform_direction( m_domain.generate() );
	
	float3 target_location = m_domain.generate();
	float3 start_location  = P->position;

	P->start_velocity = m_velocity_multiplier * instance->m_transform.transform_direction( (target_location - start_location) );
	P->velocity		 += P->start_velocity;
}

template <class ConfigValueType>
void particle_action_random_velocity::load_impl(ConfigValueType const& prop_config)
{
	m_domain.load(prop_config);
	m_velocity_multiplier = read_config_value(prop_config, "VelocityMultiplier", m_velocity_multiplier);
}

void particle_action_random_velocity::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_random_velocity::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


/*
particle_action_velocity_over_lifetime::particle_action_velocity_over_lifetime()
{
	
	m_domain    = 0;

	m_domain = UNMANAGED_NEW(m_domain,particle_domain_sphere);

	m_domain->m_translate = float3(0,6,0);
	m_domain->m_scale	  = float3(1,1,1);
	m_domain->m_rotate	  = float3(0,0,0);
}

particle_action_velocity_over_lifetime::~particle_action_velocity_over_lifetime()
{
	UNMANAGED_DELETE(m_domain);
}

void particle_action_velocity_over_lifetime::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	ASSERT(m_domain);

	(void)&instance;
	(void)&time;

	float3 target_location = m_domain->generate();
	float3 start_location  = P->position;

	P->start_velocity = (target_location - start_location).normalize() * 50.0f;
	P->velocity		  += P->start_velocity;
}

void particle_action_velocity_over_lifetime::load( configs::binary_config_value const& prop_config )
{
	particle_modifier::load(prop_config);
	load_domain(prop_config);
}
*/


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_initial_size::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_init_size.set_defaults();
}

void particle_action_initial_size::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	P->start_size = m_init_size.evaluate(instance->get_linear_emitter_time(), float3(1.0f, 1.0f, 1.0f), range_time_type, P->seed());
	P->size		  += P->start_size;
}

template <class ConfigValueType>
void particle_action_initial_size::load_impl(ConfigValueType const& prop_config)
{
	m_init_size.load(prop_config["InitSize"]);
}

void particle_action_initial_size::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_initial_size::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_initial_size::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_init_size.save_binary(buffer, calc_size);
}

void particle_action_initial_size::load_binary(xray::mutable_buffer& buffer) 
{
	m_init_size.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_velocity_over_lifetime::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_velocity_over_life.set_defaults();
}

void particle_action_velocity_over_lifetime::update( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	float3 rel_velocity = m_velocity_over_life.evaluate(P->get_linear_lifetime(), float3(1.0f, 1.0f, 1.0f), range_time_type, P->seed());
	
	P->velocity			*= rel_velocity;
}

template <class ConfigValueType>
void particle_action_velocity_over_lifetime::load_impl(ConfigValueType const& prop_config)
{
	m_velocity_over_life.load(prop_config["VelocityLife"]);
}

void particle_action_velocity_over_lifetime::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_velocity_over_lifetime::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_velocity_over_lifetime::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_velocity_over_life.save_binary(buffer, calc_size);
}

void particle_action_velocity_over_lifetime::load_binary(xray::mutable_buffer& buffer)
{
	m_velocity_over_life.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_acceleration::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_acceleration.set_defaults();
}

void particle_action_acceleration::update( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	float3 rel_accel	= m_acceleration.evaluate(P->get_linear_lifetime(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed());
	P->velocity			+= rel_accel;
}

template <class ConfigValueType>
void particle_action_acceleration::load_impl(ConfigValueType const& prop_config)
{
	m_acceleration.load(prop_config["Acceleration"]);
}

void particle_action_acceleration::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_acceleration::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_acceleration::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_acceleration.save_binary(buffer, calc_size);
}

void particle_action_acceleration::load_binary(xray::mutable_buffer& buffer)
{
	m_acceleration.load_binary(buffer);
}



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_kill_volume::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_domain.set_defaults();
	m_kill_inside = true;
}

void particle_action_kill_volume::update( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	if (m_kill_inside)
	{
		if (m_domain.inside(P->position))
		{
			P->kill();
		}
	}
	else
	{
		if (!m_domain.inside(P->position))
		{
			P->kill();
		}
	}
}

template <class ConfigValueType>
void particle_action_kill_volume::load_impl(ConfigValueType const& prop_config)
{
	m_domain.load(prop_config);
	m_kill_inside = read_config_value(prop_config, "KillInside", m_kill_inside);
}

void particle_action_kill_volume::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_kill_volume::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_size_over_lifetime::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_size_over_life.set_defaults();
	m_multiply_x = true;
	m_multiply_y = true;
	m_multiply_z = true;
}

void particle_action_size_over_lifetime::update( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance,time);
	
	float3 size_scale = m_size_over_life.evaluate(P->get_linear_lifetime(), float3(1.0f, 1.0f, 1.0f), range_time_type, P->seed());
	
	if (m_multiply_x) P->size.x *= size_scale.x;
	if (m_multiply_y) P->size.y *= size_scale.y;
	if (m_multiply_z) P->size.z *= size_scale.z;
}

template <class ConfigValueType>
void particle_action_size_over_lifetime::load_impl(ConfigValueType const& prop_config)
{
	m_multiply_x = read_config_value(prop_config, "MultiplyX", m_multiply_x);
	m_multiply_y = read_config_value(prop_config, "MultiplyY", m_multiply_y);
	m_multiply_z = read_config_value(prop_config, "MultiplyZ", m_multiply_z);
	
	m_size_over_life.load(prop_config["SizeLife"]);
}

void particle_action_size_over_lifetime::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_size_over_lifetime::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_size_over_lifetime::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_size_over_life.save_binary(buffer, calc_size);
}

void particle_action_size_over_lifetime::load_binary(xray::mutable_buffer& buffer) 
{
	m_size_over_life.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_size_over_velocity::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	m_size_over_velocity.set_defaults();
}

void particle_action_size_over_velocity::update( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance, time);
	
	float3 size_scale = P->velocity.length() * m_size_over_velocity.evaluate(P->get_linear_lifetime(), float3(1.0f, 1.0f, 1.0f), range_time_type, P->seed());
	
	P->size.x *= size_scale.x;
	P->size.y *= size_scale.y;
	P->size.z *= size_scale.z;
}

template <class ConfigValueType>
void particle_action_size_over_velocity::load_impl(ConfigValueType const& prop_config)
{
	m_size_over_velocity.load(prop_config["SizeVelocity"]);
}

void particle_action_size_over_velocity::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_size_over_velocity::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_size_over_velocity::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_size_over_velocity.save_binary(buffer, calc_size);
}

void particle_action_size_over_velocity::load_binary(xray::mutable_buffer& buffer)
{
	m_size_over_velocity.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_color_over_lifetime::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	
	m_color_over_life.set_defaults();
}

void particle_action_color_over_lifetime::init( particle_emitter_instance* instance, base_particle* P, float time_delta )
{
	XRAY_UNREFERENCED_PARAMETERS(instance, time_delta);
	P->target_color_y_position = random_float(0.0f, 1.0f);
}

void particle_action_color_over_lifetime::update( particle_emitter_instance* instance, base_particle* P, float time_delta )
{
	XRAY_UNREFERENCED_PARAMETERS(instance, time_delta);
	
#if 1
	P->color						= m_color_over_life.evaluate(P->get_linear_lifetime(), P->target_color_y_position, float4(0.0f,0.0f,0.0f,1.0f));
#else
	float const linear_time			= P->get_linear_lifetime();
	s32			column_index		= -1;
	bool		is_last				= false;
	
	bool		reset_local_time	= false;
	if (m_color_over_life.is_on_column(linear_time, time_delta / P->get_duration(), column_index, is_last))
	{
		if (!is_last)
		{
			float const x_time		= m_color_over_life.get_time(column_index + 1);
			P->target_color_position= float2(x_time, random_float(0.0f, 1.0f));
			reset_local_time		= true;
		}
	}
	float4 target_color				= m_color_over_life.evaluate(P->target_color_position.x, P->target_color_position.y, float4(0.0f,0.0f,0.0f,1.0f));
	
	float local_time				= m_color_over_life.get_local_time(linear_time);
	
	if (reset_local_time)
		local_time					= 0.0f;
	
	P->color						= linear_interpolation(
										m_color_over_life.evaluate(linear_time, 0.5f, float4(0.0f,0.0f,0.0f,1.0f)),
										target_color, 
										local_time
									  );
#endif
}

template <class ConfigValueType>
void particle_action_color_over_lifetime::load_impl(ConfigValueType const& prop_config)
{
	m_color_over_life.load(prop_config["ColorLifeMatrix"]);
}

void particle_action_color_over_lifetime::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_color_over_lifetime::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_color_over_lifetime::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_color_over_life.save_binary(buffer, calc_size);
}

void particle_action_color_over_lifetime::load_binary(xray::mutable_buffer& buffer)
{
 	m_color_over_life.load_binary(buffer);
}


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


void particle_action_orbit::set_defaults( bool mt_alloc )
{
	particle_modifier::set_defaults(mt_alloc);
	
	m_offset_amount.set_defaults();
	m_rotation_amount.set_defaults();
	m_rotation_rate_amount.set_defaults();
}

void particle_action_orbit::init( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance, time);
	
	float3 offset			= m_offset_amount.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed());
	float3 rotation			= m_rotation_amount.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed()) * math::pi_x2;
	//float3 rotation_rate	= m_rotation_rate_amount.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed()) * math::pi_x2;
	
	offset					= math::create_rotation(rotation).transform_position(offset);
	P->position				+=offset;
	P->old_position			+=offset;
	
	// TODO: do not store this values in base_particle!
	//P->offset_amount		= offset;
	P->prev_offset_position	= P->position;
	//P->rotation_amount		= rotation;
	//P->rotation_rate_amount = rotation_rate;
}

void particle_action_orbit::update( particle_emitter_instance* instance, base_particle* P, float time )
{
	XRAY_UNREFERENCED_PARAMETERS(instance, time);
	
	float3 offset			= m_offset_amount.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed());
	float3 rotation			= m_rotation_amount.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed()) * math::pi_x2;
	float3 rotation_rate	= m_rotation_rate_amount.evaluate(instance->get_linear_emitter_time(), float3(0.0f, 0.0f, 0.0f), range_time_type, P->seed()) * math::pi_x2;
	
	offset					= math::create_rotation(rotation).transform_position(offset);
	
	float curr_time			= P->get_lifetime();
	//float prev_emitter_time	= instance->get_emitter_time() - 0.1f;
	
	float3 next_position	= P->spawn_position + math::create_rotation(rotation_rate * curr_time).transform_position(offset);
	
	P->position				+= next_position - P->prev_offset_position;
	P->prev_offset_position	= next_position;
}

template <class ConfigValueType>
void particle_action_orbit::load_impl(ConfigValueType const& prop_config)
{
	m_offset_amount.load(prop_config["OffsetAmount"]);
	m_rotation_amount.load(prop_config["RotationAmount"]);
	m_rotation_rate_amount.load(prop_config["RotationRateAmount"]);
}

void particle_action_orbit::load(configs::binary_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}

#ifndef	MASTER_GOLD
void particle_action_orbit::load(configs::lua_config_value const& prop_config)
{
	particle_modifier::load(prop_config);
	load_impl(prop_config);
}
#endif

u32 particle_action_orbit::save_binary(xray::mutable_buffer& buffer, bool calc_size)
{
	return m_offset_amount.save_binary(buffer, calc_size) + 
		   m_rotation_amount.save_binary(buffer, calc_size) + 
		   m_rotation_rate_amount.save_binary(buffer, calc_size);
}

void particle_action_orbit::load_binary(xray::mutable_buffer& buffer)
{
 	m_offset_amount.load_binary(buffer);
	m_rotation_amount.load_binary(buffer);
	m_rotation_rate_amount.load_binary(buffer);
}
} // namespace particle
} // namespace xray
