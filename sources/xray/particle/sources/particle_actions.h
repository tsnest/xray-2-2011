////////////////////////////////////////////////////////////////////////////
//	Created		: 26.07.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_ACTIONS_H_INCLUDED
#define PARTICLE_ACTIONS_H_INCLUDED

#include "curve_line.h"
#include "color_matrix.h"
#include "particle_domains.h"
#include <xray/particle/particle_data_type.h> // for enum_particle_data_type

namespace xray {
namespace particle {

class particle_emitter_instance;
class particle_domain;
class particle_system;

struct base_particle;
struct particle_system_lod;

// For binary saving/loading.
// TODO: remove it!
#define DECLARE_ACTION(type)\
	public:\
	virtual pcstr get_class_name	() const {return #type; }\
	virtual u32	  get_class_size	() const {return sizeof(type); }

class particle_action_platform : public boost::noncopyable {
#if !XRAY_PLATFORM_WINDOWS
	u32 dummy;
#endif // #if !XRAY_PLATFORM_WINDOWS
};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action : public particle_action_platform {
	DECLARE_ACTION(particle_action);
	typedef xray::platform_pointer<particle_action, xray::platform_pointer_default>::type particle_action_ptr_type;
	
public:
								particle_action		(): m_next(0) {}
	virtual						~particle_action	() {}
	virtual void				init				(particle_emitter_instance* , base_particle*,  float) {};
	virtual void				update				(particle_emitter_instance* , base_particle* , float) {};
	virtual void				load				(configs::binary_config_value const& config);
	virtual void				set_defaults		(bool mt_alloc = false);
	virtual bool				is_update_modifier	() const {return false;};
	virtual bool				is_init_modifier	() const {return false;};
	virtual bool				is_event			() const {return false;};
	virtual bool				is_datatype			() const {return false;};
	virtual u32					save_binary			(xray::mutable_buffer&, bool) {return 0;}
	virtual void				load_binary			(xray::mutable_buffer&) {}
	bool						get_visibility		() const { return m_visibility; }
	
#ifndef	MASTER_GOLD
	virtual void				load				(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
	particle_action_ptr_type	m_next;
	
private:
	template <class ConfigValueType>
	void						load_impl			(ConfigValueType const& config);
	
protected:
	bool						m_visibility;
}; // class particle_action

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class particle_modifier : public particle_action {
	DECLARE_ACTION(particle_modifier);
	typedef xray::platform_pointer<particle_modifier, xray::platform_pointer_default>::type particle_modifier_ptr_type;
public:
	virtual			~particle_modifier				( ) { }
	virtual u32		save_binary						(xray::mutable_buffer&, bool) {return 0;}
	virtual void	load_binary						(xray::mutable_buffer&) {}
	
	//particle_modifier_ptr_type					next_init_modifier;
	//particle_modifier_ptr_type					next_update_modifier;
}; // class particle_modifier

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class particle_action_data_type : public particle_action {
	DECLARE_ACTION(particle_action_data_type);
	typedef xray::platform_pointer<particle_action_data_type, xray::platform_pointer_default>::type particle_action_data_type_ptr_type;
public:
	virtual						~particle_action_data_type	( ) { }
	virtual bool					is_datatype		() const {return true;};
	
	// In editor used.
	virtual enum_particle_data_type get_data_type	() const { return particle_data_type_billboard; };
	
	//particle_action_data_type_ptr_type			next_datatype;
}; // class particle_action_data_type


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_billboard : public particle_action_data_type {
	DECLARE_ACTION(particle_action_billboard);
public:
	virtual void					load			(configs::binary_config_value const& config);
#ifndef	MASTER_GOLD
	virtual void					load			(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	virtual void					set_defaults	(bool mt_alloc = false);
	virtual void					init			(particle_emitter_instance* , base_particle*,  float);
	virtual void					update			(particle_emitter_instance* , base_particle* , float);	
	virtual bool					is_update_modifier	() const {return true;};
	virtual bool					is_init_modifier	() const {return true;};	
	// In editor used.
	virtual enum_particle_data_type get_data_type	() const { return particle_data_type_billboard; };
	
	virtual u32						save_binary		(xray::mutable_buffer&, bool);
	virtual void					load_binary		(xray::mutable_buffer&);
	
private:
	template <class ConfigValueType>
	void							load_impl		(ConfigValueType const& config);
	
public:
	curve_line_ranged_float			m_subimage_index;
	billboard_parameters			m_billboard_parameters;
	u32								m_sub_image_horizontal;
	u32								m_sub_image_vertical;
	float							m_movie_frame_rate;
	float							m_movie_start_frame;
	bool							m_use_sub_uv;
	bool							m_use_movie;
}; // class particle_action_billboard

class particle_action_mesh : public particle_action_data_type {
	DECLARE_ACTION(particle_action_mesh);
public:
	virtual enum_particle_data_type get_data_type	() const { return particle_data_type_mesh; };
}; // class particle_action_mesh


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_trail : public particle_action_data_type {
	DECLARE_ACTION(particle_action_trail);
public:
	virtual void					load			(configs::binary_config_value const& config);
	virtual void					set_defaults	(bool mt_alloc = false);
	virtual void					update			(particle_emitter_instance* , base_particle* , float);
	virtual enum_particle_data_type get_data_type	() const { return particle_data_type_trail; };
	
#ifndef	MASTER_GOLD
	virtual void					load			(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void							load_impl		(ConfigValueType const& config);
	
public:
	beamtrail_parameters			m_beamtrail_parameters;
	enum_particle_screen_alignment	m_screen_alignment;
}; // class particle_action_trail


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_beam : public particle_action_data_type {
	DECLARE_ACTION(particle_action_beam);
public:
	virtual void					load			(configs::binary_config_value const& config);
	virtual void					set_defaults	(bool mt_alloc = false);
	virtual enum_particle_data_type get_data_type	() const { return particle_data_type_beam; };
	
#ifndef	MASTER_GOLD
	virtual void					load			(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void							load_impl		(ConfigValueType const& config);
	
public:
	beamtrail_parameters			m_beamtrail_parameters;
	float							m_speed;
	float							m_noise;
	float							m_frequency;
}; // class particle_action_beam


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_decal : public particle_action_data_type {
	DECLARE_ACTION(particle_action_decal);
	virtual enum_particle_data_type get_data_type() const { return particle_data_type_decal; };
}; // class particle_action_decal


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

enum enum_particle_event {
	event_on_play,
	event_on_death,
	event_on_collide,
	event_on_birth,
	event_no_event,
}; // enum enum_particle_event


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_event : public particle_action {
	DECLARE_ACTION(particle_event);
	typedef xray::platform_pointer<particle_event, xray::platform_pointer_default>::type particle_event_ptr_type;
public:
	virtual bool				is_event							() const {return true;};
	// In editor used.
	virtual enum_particle_event get_event_type						() const { return event_no_event; }
	virtual void				set_defaults						(bool mt_alloc = false);
	virtual void				load								(configs::binary_config_value const& config);
			s32					get_child_particle_system_index		() const  { return m_child_ps_index; }
			void				set_child_particle_system_index		(s32 index) { m_child_ps_index = index; }
			bool				get_inherit_position				() const { return m_inherit_position; }
#ifndef	MASTER_GOLD
	virtual void				load								(configs::lua_config_value const& config);
#endif // MASTER_GOLD	
	//particle_event_ptr_type	next_event;
private:
	template <class ConfigValueType>
			void				load_impl							(ConfigValueType const& config);
protected:
			s32					m_child_ps_index;
			bool				m_inherit_position;
}; // class particle_event


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_event_on_play : public particle_event {
	DECLARE_ACTION(particle_event_on_play);
public:
	virtual enum_particle_event		get_event_type	() const { return event_on_play; }
	virtual void					set_defaults	(bool mt_alloc = false);
}; // class particle_event_on_play


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_event_on_birth : public particle_event {
	DECLARE_ACTION(particle_event_on_birth);
public:
	virtual enum_particle_event		get_event_type	() const { return event_on_birth; }
	virtual void					set_defaults	(bool mt_alloc = false);
}; // class particle_event_on_birth


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_event_on_death : public particle_event {
	DECLARE_ACTION(particle_event_on_death);
public:
	virtual enum_particle_event		get_event_type	() const { return event_on_death; }
	virtual void					set_defaults	(bool mt_alloc = false);
}; // class particle_event_on_death


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_event_on_collide : public particle_event {
	DECLARE_ACTION(particle_event_on_collide);
public:
	virtual enum_particle_event		get_event_type	() const { return event_on_collide; }
	virtual void					set_defaults	(bool mt_alloc = false);
}; // class particle_event_on_collide


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_source : public particle_modifier {
	DECLARE_ACTION(particle_action_source);
public:
	virtual void				load				(configs::binary_config_value const& config);
	virtual void				init				(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void				set_defaults		(bool mt_alloc = false);
	virtual bool				get_need_init		() const {return true;}
	virtual bool				is_init_modifier	() const {return true;};
	xray::math::float4x4		get_transform		() const;
	
#ifndef	MASTER_GOLD
	virtual void				load				(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void						load_impl			(ConfigValueType const& config);
	
private:
	particle_domain_complex		m_domain;
}; // class particle_action_source


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_initial_velocity : public particle_modifier {
	DECLARE_ACTION(particle_action_initial_velocity);
public:
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_init_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_init_velocity;
}; // class particle_action_initial_velocity


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_initial_rotation_rate : public particle_modifier {
	DECLARE_ACTION(particle_action_initial_rotation_rate);
public:
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_init_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool );
	virtual void		load_binary				(xray::mutable_buffer& );
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_init_rate;
}; // class particle_action_initial_rotation_rate

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_random_velocity : public particle_modifier {
	DECLARE_ACTION(particle_action_random_velocity);
public:
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_init_modifier		() const {return true;};
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	particle_domain_complex						m_domain;
	float										m_velocity_multiplier;
}; // class particle_action_random_velocity



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_initial_color : public particle_modifier {
	DECLARE_ACTION(particle_action_initial_color);
public:
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		() const{return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool );
	virtual void		load_binary				(xray::mutable_buffer& );
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_color	m_init_color;
}; // class particle_action_initial_color



////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_initial_rotation : public particle_modifier {
	DECLARE_ACTION(particle_action_initial_rotation);
public:
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_init_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool );
	virtual void		load_binary				(xray::mutable_buffer& );
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_init_rotation;
}; // class particle_action_initial_rotation


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_initial_size : public particle_modifier {
	DECLARE_ACTION(particle_action_initial_size);
public:
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_init_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool );
	virtual void		load_binary				(xray::mutable_buffer& );
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);

private:
	curve_line_ranged_xyz_float					m_init_size;
}; // class particle_action_initial_size


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_velocity_over_lifetime : public particle_modifier {
	DECLARE_ACTION(particle_action_velocity_over_lifetime);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		()const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		 load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	inline	void		load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_velocity_over_life;
}; // class particle_action_velocity_over_lifetime


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_acceleration : public particle_modifier {
	DECLARE_ACTION(particle_action_acceleration);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		()const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		 load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	inline	void		load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_acceleration;
}; // class particle_action_acceleration


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_kill_volume : public particle_modifier {
	DECLARE_ACTION(particle_action_kill_volume);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		()const {return true;};
	
#ifndef	MASTER_GOLD
	virtual void		 load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	inline	void		load_impl				(ConfigValueType const& config);
	
private:
	particle_domain_complex						m_domain;
	bool										m_kill_inside;
}; // class particle_action_kill_volume





////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_color_over_lifetime : public particle_modifier {
	DECLARE_ACTION(particle_action_color_over_lifetime);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		() const {return true;};
	virtual bool		is_init_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool );
	virtual void		load_binary				(xray::mutable_buffer& );
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	color_matrix		m_color_over_life;
}; // class particle_action_color_over_lifetime


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_rotation_over_lifetime : public particle_modifier {
	DECLARE_ACTION(particle_action_rotation_over_lifetime);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_rotation_over_life;
}; // class particle_action_rotation_over_lifetime


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_rotation_over_velocity : public particle_modifier {
	DECLARE_ACTION(particle_action_rotation_over_velocity);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_rotation_over_velocity;
	bool										m_affect_x;
	bool										m_affect_y;
	bool										m_affect_z;
}; // class particle_action_rotation_over_velocity


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_size_over_lifetime : public particle_modifier {
	DECLARE_ACTION(particle_action_size_over_lifetime);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false );
	virtual bool		is_update_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_size_over_life;
	bool										m_multiply_x;
	bool										m_multiply_y;
	bool										m_multiply_z;
}; // class particle_action_size_over_lifetime


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_size_over_velocity : public particle_modifier {
	DECLARE_ACTION(particle_action_size_over_velocity);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_update_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_size_over_velocity;
}; // class particle_action_size_over_velocity


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


class particle_action_orbit : public particle_modifier {
	DECLARE_ACTION(particle_action_orbit);
public:
	virtual void		update					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		init					(particle_emitter_instance* instance, base_particle* P, float time);
	virtual void		load					(configs::binary_config_value const& config);
	virtual void		set_defaults			(bool mt_alloc = false);
	virtual bool		is_init_modifier		() const {return true;};
	virtual bool		is_update_modifier		() const {return true;};
	virtual u32			save_binary				(xray::mutable_buffer&, bool);
	virtual void		load_binary				(xray::mutable_buffer&);
	
#ifndef	MASTER_GOLD
	virtual void		load					(configs::lua_config_value const& config);
#endif // MASTER_GOLD
	
private:
	template <class ConfigValueType>
	void				load_impl				(ConfigValueType const& config);
	
private:
	curve_line_ranged_xyz_float					m_offset_amount;
	curve_line_ranged_xyz_float					m_rotation_amount;
	curve_line_ranged_xyz_float					m_rotation_rate_amount;
}; // class particle_action_orbit

#undef DECLARE_ACTION

} // namespace particle
} // namespace xray

#endif // #ifndef PARTICLE_ACTIONS_H_INCLUDED