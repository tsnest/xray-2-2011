////////////////////////////////////////////////////////////////////////////
//	Created		: 15.10.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

#include <xray/render/facade/model.h>
#include <xray/animation/animation_player.h>
#include <xray/animation/mixing_base_lexeme.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>

namespace stalker2{

class weapon;
class game_world;
class weapon_part_visual;

class weapon_part
{
public:
						weapon_part	( );
	virtual void		load		( configs::binary_config_value const& config );
	bool				m_visible;
	weapon*				m_parent;
}; // class weapon_part

struct visual_attach_desc
{
								visual_attach_desc( ):m_item(NULL){}

	weapon_part_visual*			m_item;
	render::model_locator_item	m_attach_point;
};

class weapon_part_visual : public weapon_part
{
	typedef weapon_part super;

public:
						weapon_part_visual	( );
			void		load				( configs::binary_config_value const& config );

	virtual void		set_transform		( float4x4 const& transform );
	virtual void		show				( float4x4 const& initial_transform );
	virtual void		hide				( );
	virtual bool		calculate_locator	( render::model_locator_item const& locator, float4x4& result );
	virtual bool		get_locator			( pcstr locator_name, render::model_locator_item& locator ) =0;
			
protected:
			void		update_childs_transform( );
	float4x4					m_transform;

	typedef vector<visual_attach_desc> childs;
	childs						m_childs;
};

class weapon_part_solid_visual : public weapon_part_visual
{
	typedef weapon_part_visual super;
public:
						weapon_part_solid_visual( );
			void		load					( configs::binary_config_value const& config, render::static_model_ptr );
	virtual void		set_transform			( float4x4 const& transform );
	virtual void		show					( float4x4 const& initial_transform );
	virtual void		hide					( );
	virtual bool		get_locator				( pcstr locator_name, render::model_locator_item& locator );

protected:
	void				on_resources_loaded		( resources::queries_result& data );
	render::static_model_ptr		m_model;
};

class weapon_part_skinned_visual : public weapon_part_visual
{
	typedef weapon_part_visual super;
public:

						weapon_part_skinned_visual( );
	virtual				~weapon_part_skinned_visual( );
	void				load					( configs::binary_config_value const& config,
													render::skeleton_model_ptr model,
													animation::skeleton_animation_ptr idle_anim,
													animation::skeleton_animation_ptr reload_anim,
													animation::skeleton_animation_ptr shoot_anim );
	virtual void		set_transform			( float4x4 const& transform );
	virtual void		show					( float4x4 const& initial_transform );
	virtual void		hide					( );
	virtual bool		calculate_locator		( render::model_locator_item const& locator, float4x4& result );
	virtual bool		get_locator				( pcstr locator_name, render::model_locator_item& locator );
			void		tick					( animation::animation_player*& animation_player );

	xray::animation::mixing::animation_lexeme		select_animation		( mutable_buffer& buffer );

protected:

	render::skeleton_model_ptr			m_model;
	float4x4*							m_matrices;

	animation::skeleton_animation_ptr	m_idle;
	animation::skeleton_animation_ptr	m_reload;
	animation::skeleton_animation_ptr	m_shoot;
};

class weapon : public resources::unmanaged_resource,
				private boost::noncopyable
{
public:
				weapon				( );
	void		action				( int id );

	game_world&	get_game_world		( ) {return *m_game_world;}
	void		set_transform		( float4x4 const& transform );
	void		tick				( animation::animation_player*& animation_player );
	void		show				( float4x4 const& initial_transform );
	void		hide				( );
	weapon_part_visual*		get_part( pcstr name );
	xray::animation::mixing::animation_lexeme		select_animation		(mutable_buffer& buffer );

protected:
	void		load				( configs::binary_config_value const& config );

public: // tmp
	game_world*					m_game_world;
	weapon_part_skinned_visual*	m_base;
	
	weapon_part_solid_visual*	m_cover;
	weapon_part_solid_visual*	m_stock;
	weapon_part_solid_visual*	m_handle;
	weapon_part_solid_visual*	m_ammo;
	weapon_part_solid_visual*	m_bolt;
	weapon_part_solid_visual*	m_fore_end;
	weapon_part_solid_visual*	m_barrel;
	weapon_part_solid_visual*	m_barrel_end;

	float4x4					m_transform;

public:
	int							m_current_state;
	u32							m_current_state_start_time;
};

typedef	resources::resource_ptr<
	weapon,
	resources::unmanaged_intrusive_base >
weapon_ptr;

} // namespace stalker2

#endif // #ifndef WEAPON_H_INCLUDED