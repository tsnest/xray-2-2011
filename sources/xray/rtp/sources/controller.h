////////////////////////////////////////////////////////////////////////////
//	Created		: 03.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED

#include "action.h"
#include "training_value_function.h"
#include "value_function.h"
#include "action_base.h"
#include "space_params.h"
#include <xray/rtp/base_controller.h>

#include <xray/threading_extensions.h>

namespace xray {

namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace configs {
	class lua_config_value;
} // namespace configs

namespace rtp {


template < class action, class value_function_type >
class controller: public base_controller {

public:
	typedef			 action								action_type;
	typedef	typename action::space_param_type			space_param_type;
	typedef	typename action::world_space_param_type		world_space_param_type;
	typedef	typename action::global_actions_params_type global_actions_params_type;

private:	
	typedef	controller<action, value_function_type>				self_type;

public:
						controller			( pcstr name, const global_actions_params< action > &par );
	virtual				~controller			( );
	bool				update				( world_space_param_type &out_w, float	td );

private:
	void				next_action			( const world_space_param_type& w, space_param_type	const &params );

public:
action_base<action>						&base	( ){ return m_action_base; }
action_base<action>				const	&base	( )const{ return m_action_base; }
value_function_type				const	&value	( )const{ return m_value_function; }

public:
	virtual	void						learn						( ){};
	virtual	float						learn_step					( ){ return 0 ; }
	virtual bool						learn_step					(  float, u32  ){ return false; }
	virtual	void						step_logic					(  float ){};
	virtual	void						learn_init					( ){};
	virtual	bool						learn_initialized			( ) const { return false; }

	virtual	void						learn_stop					( ){};

#ifndef MASTER_GOLD
	virtual	void						save_training_sets			( xray::configs::lua_config_value  )const{};
#endif // MASTER_GOLD

	virtual	void						load_training_sets			( const xray::configs::binary_config_value & ){};

	void								walk_init					( u32 action_id, u32 blend_state, const world_space_param_type& start_position );
	
	const action						*current_action				( )const	{return m_action; }
	virtual	pcstr						name						( )const	{ return m_name.c_str(); }

#ifndef MASTER_GOLD
	virtual	void						save						( xray::configs::lua_config_value cfg, bool training_trees )const;
#endif // MASTER_GOLD

	virtual	void						load						( const xray::configs::binary_config_value &cfg );


	virtual bool						is_loaded					( ){ return true; }			

	virtual	bool						compare_value				( const base_controller& ctrl ) const;

public:
	virtual	void						render_value				( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const;


protected:
inline	float							action_time					( )const { return m_action_time; }
inline	u32								step						( )const { return m_step; }
inline 	const world_space_param_type	&action_start_world_pos		( )const { return m_action_start_world_pos; }
inline	void							set_action_start_world_pos	( const world_space_param_type& pos ) { m_action_start_world_pos = pos; }

private:
	inline	void						space_params_fromworld	( const world_space_param_type& w, space_param_type &p )const;
	virtual	void						from_world				( const world_space_param_type& w, space_param_type &p )const		= 0;
	virtual	void						on_new_walk_action		( world_space_param_type& w, space_param_type	&params );

private:
	world_space_param_type				m_action_start_world_pos;

	space_param_type					m_action_start_params;

	const action						*m_action;
	u32									m_blend_state;

	const action						*m_previous_action;
	u32									m_previous_blend_state;

	float								m_action_time;
	float								m_global_time;

protected:
	//training_value_function<action>		m_value_function;
	
	value_function_type					m_value_function;

private:
	action_base<action>					m_action_base;

	u32									m_step;
	fixed_string<64>					m_name;

}; // class controller


template < class action >
class learning_controller: public controller< action, training_value_function< action > > {

	typedef	 controller< action, training_value_function< action > >			super;
	typedef	 learning_controller< action >										self_type;
	using	 super::is_loaded;
	using	 super::m_value_function;
public:
						learning_controller			( pcstr name, const global_actions_params< action > &par ): 
						super( name, par ),
						m_b_learn_initialized( 0 ),
						m_b_learn_stoped( 0 ),
						m_last_residual( math::float_max )
						{};


	virtual	void						learn						( );
	virtual	float						learn_step					( );
	virtual bool						learn_step					( float residual, u32 stop_iterations );
	virtual	void						step_logic					( float stop_residual, u32 stop_iterations );
	virtual	void						learn_init					( );
	virtual	bool						learn_initialized			( ) const { return !!m_b_learn_initialized; }
	virtual	bool						learn_stoped				( ) const { return !!m_b_learn_stoped; }

	virtual	void						learn_stop					( );
	virtual	void						learn_start					( );
#ifndef MASTER_GOLD
	virtual	void						save_training_sets			( xray::configs::lua_config_value )const;
#endif // MASTER_GOLD

	virtual	void						load_training_sets			( const xray::configs::binary_config_value & );

private:
	threading::atomic32_type			m_b_learn_initialized;
	threading::atomic32_type			m_b_learn_stoped;
	threading::mutex					m_learn_exit_lock;

private:
	typedef typename threading::atomic_type_helper< float, boost::is_volatile<threading::atomic32_type>::value >::result residual_type;
	residual_type						m_last_residual;

}; // class learning_controller

template < class action >
class walking_controller: public controller< action, walking_value_function< action > > {

	typedef	 controller< action, walking_value_function< action > >			super;
	
public:
							walking_controller			( pcstr name, const global_actions_params< action > &par ): 
							super( name, par )
							{};

}; // class learning_controller


base_controller*	create_controller( pcstr name,  const xray::configs::binary_config_ptr &cfg , animation::world &w );
base_controller*	create_controller( pcstr name, pcstr type, animation::world &w );

base_controller*	create_test_grasping_controller(  );



base_controller*	create_animation_grasping_controller( pcstr name, animation::world &w, pcstr animation_set_path  );
base_controller*	create_navigation_controller( pcstr name, animation::world &w, pcstr animation_set_path );
base_controller*	create_navigation_controller( pcstr name, animation::world &w, pcstr animation_set_path, const xray::configs::binary_config_ptr &cfg );
base_controller*	create_navigation_controller( pcstr name, animation::world &w,const xray::configs::binary_config_ptr &cfg );




} // namespace rtp
} // namespace xray


#include "controller_inline.h"



#endif // #ifndef CONTROLLER_H_INCLUDED