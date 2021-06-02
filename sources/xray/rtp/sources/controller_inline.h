////////////////////////////////////////////////////////////////////////////
//	Created		: 13.11.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "action.h"
#include "value_function.h"
#include "action_base.h"
#include "space_params.h"

namespace xray {
namespace rtp {

//extern command_line::key	rtp_learn_singlethreaded;
static  bool rtp_learn_singlethreaded = true;

template < class action, class value_function_type >
void	controller< action, value_function_type >::next_action( const world_space_param_type& w, space_param_type	const &params )
{
			

			world_space_param_type	world_pos		=	w;
			space_param_type		local_params	=	params;
			
			on_new_walk_action			( world_pos, local_params  );
			
			m_action_start_world_pos	=	world_pos;
			m_action_start_params		=	local_params;

			u32 new_blend_state			=	m_blend_state;

			m_previous_action			=	m_action;

			m_action					=	m_value_function.pi( *m_action, m_blend_state, m_action_start_params, new_blend_state );

			LOG_DEBUG( "next action: %s, id: %d, blend_id: %d",  m_action->name(), m_action->id(), new_blend_state  );

			m_previous_blend_state		=	m_blend_state;
			m_blend_state				=	new_blend_state;
			
			++m_step;
			
			ASSERT( m_action );
}

template < class action, class value_function_type >
inline void	controller< action, value_function_type >::on_new_walk_action	( world_space_param_type& w, space_param_type	&params )
{
	XRAY_UNREFERENCED_PARAMETERS( &w, &params );
}

template < class action, class value_function_type >
bool	controller< action, value_function_type >::update( world_space_param_type &out_w, float td )
{
	m_action_time+=td;
	m_global_time+=td;
	float time = m_global_time;

	out_w = m_action_start_world_pos;

	do
	{
		float time_left = m_action->run( out_w, m_action_start_params, *m_previous_action, m_previous_blend_state, m_blend_state, m_step, time ) ;

		if( time_left <= 0.f )
		{

			m_action_time			=	-time_left;

			if( m_action->target( m_action_start_params ) )
				return true;
			
			if( m_action->is_target_action() )
			{
				LOG_DEBUG("target not reached");
				return true;
			}


			space_param_type			params = m_action_start_params;
#if 0
			
			space_params_fromworld		( out_w, params );

			space_param_type			start_params;								//debug
			space_params_fromworld		( m_action_start_world_pos, start_params );	//debug

#ifdef RTP_ANIMATION_SIMULATE_MOVEMENT
			ASSERT( m_action_start_params.similar		( start_params )  );	
#endif
			m_action->run				(  start_params, *m_previous_action, m_previous_blend_state,  m_blend_state );//debug

			//ASSERT( params.similar		( start_params )  );
			//debug
#endif
			
			next_action( out_w, params );

			continue;
		}
		
	} while( xray::identity(false) );

	return false;
}

template < class action, class value_function_type >
controller< action, value_function_type >::controller( pcstr name, const global_actions_params< action > &par ):
m_name( name ),
m_action( 0 ),
m_blend_state( 0 ),
m_previous_action( 0 ),
m_previous_blend_state( 0 ),
m_action_time( 0 ),
m_global_time( 0 ),
m_action_base( par ),
m_value_function( m_action_base ),
m_step( 0 )

{
	;
};

template < class action, class value_function_type >
controller< action, value_function_type >::~controller			( )
{
	learn_stop();
}

template < class action >
inline void learning_controller< action >::learn(  )
{

	//m_learn_exit_lock.lock();

	//long running = false;

	ASSERT( m_b_learn_initialized );
	

	do{

	learn_step(); 

	//running = m_b_learn_stoped;//m_b_learn_initialized;


	}while ( !m_b_learn_stoped );
	
	//m_learn_exit_lock.unlock();
	//m_value_function.learn();
}

template < class action >
inline void	learning_controller< action >::learn_init		( )
{

	if( m_b_learn_initialized )
		return;

	ASSERT( !m_b_learn_initialized );
	

	m_value_function.learn_init();
	

	if( rtp_learn_singlethreaded )
	{
		m_b_learn_initialized =is_loaded(); 
		return;
	}

	threading::thread_function_type const  fun = boost::bind(  &self_type::learn, this );
	m_b_learn_initialized =is_loaded(); 
	
	threading::spawn( fun , "rtp_learn", "rtp_learn", 0, 0 );

}

template < class action >
inline	void	learning_controller< action >::step_logic(  float stop_residual, u32 stop_iterations  )
{
	if( !m_b_learn_initialized || m_b_learn_stoped )
		return;

	//if( m_value_function.samples_step() )
	//	LOG_INFO("rtp residual: %f ", m_last_residual );
	m_value_function.samples_step();
	LOG_INFO("rtp residual: %f ", m_last_residual );

	if( stop_residual > m_last_residual || stop_iterations < m_value_function.steps_samples_count() )
	{
		learn_stop( );	
		return;
	}

	if( !rtp_learn_singlethreaded )
		return;

	learn_step(); 
	
}

template < class action >
inline bool	learning_controller< action >::learn_step( float residual, u32 stop_iterations )
{
		learn_init( );
		learn_start( );
		step_logic( residual, stop_iterations );
		return learn_initialized( ) && !learn_stoped( );
}

template< class action >
inline float	learning_controller< action >::learn_step		( )
{

	float ret = m_value_function.learn_step();
	ASSERT(sizeof(long)==sizeof(float));
	
	threading::interlocked_exchange( (threading::atomic32_type&) m_last_residual, horrible_cast<float, long>( ret ).second  );
	return ret;

}

template < class action >
inline void learning_controller< action >::learn_stop( )
{
	//threading::interlocked_exchange			( m_b_learn_initialized, false );
	threading::interlocked_exchange			( m_b_learn_stoped, 1 );
	//m_learn_exit_lock.lock();
	//m_learn_exit_lock.unlock();
}

template < class action >
inline void learning_controller< action >::learn_start( )
{
	threading::interlocked_exchange			( m_b_learn_stoped,  0  );
}

template < class action, class value_function_type >
inline void		controller< action, value_function_type >::render_value			( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const
{
	 m_value_function.render( scene, renderer );
}

template < class action, class value_function_type >
inline void	controller< action, value_function_type >::space_params_fromworld	( const world_space_param_type& w, space_param_type &p )const
{
	from_world( w, p );
	if ( !in_range( p ) )
	{
		clamp_to_range( p );
	}
}

template < class action, class value_function_type >
bool	controller< action, value_function_type >::compare_value( const base_controller& ctrl ) const
{
	R_ASSERT( strings::compare( ctrl.type(), type() ) == 0 );
	const controller< action, value_function_type >& c = static_cast_checked< const controller< action, value_function_type >&  >( ctrl );

	return value().compare( c.value() );
}

template < class action, class value_function_type >
void		controller< action, value_function_type >::walk_init		(  u32 action_id, u32 blend_state, const world_space_param_type& start_position   )
{
	ASSERT( action_id < m_action_base.size() );
	m_action = m_action_base.get( action_id );
	m_previous_action = m_action;
	m_action_start_world_pos = start_position;
	space_params_fromworld( m_action_start_world_pos, m_action_start_params );

	m_action_time = 0;
	m_global_time = 0;
	m_previous_blend_state = blend_state;
	m_blend_state = blend_state;
	m_step = 0;
}

#ifndef MASTER_GOLD
template < class action, class value_function_type >
inline void	controller< action, value_function_type >::save( xray::configs::lua_config_value config, bool training_trees )const
{


		xray::configs::lua_config_value cfg = config["controller"];
		cfg["type"]					= type ();
		m_action_start_world_pos	.save(cfg["world_pos"]);
		cfg["action_time"]			= m_action_time;
		cfg["blend_state"]			= m_blend_state;
		m_action_base				.save(cfg["action_base"]);
		cfg["action_id"]			= !m_action ? u32(-1) : m_action->id();
		m_value_function			.save(cfg["value_function"],training_trees);


}
#endif //MASTER_GOLD

template < class action, class value_function_type >
inline void	controller< action, value_function_type >::load( const xray::configs::binary_config_value &config )
{
		const xray::configs::binary_config_value cfg = config["controller"];
		m_action_start_world_pos	.load(cfg["world_pos"]);
		m_action_time				=cfg["action_time"];
		m_global_time				= 0;
		m_blend_state				=cfg["blend_state"];
		m_action_base				.load(cfg["action_base"]);
		u32 id						=cfg["action_id"];
		m_action					= ( id == u32(-1) ) ? 0 : m_action_base.get( id );
		m_value_function			.load(cfg["value_function"]);
}

#ifndef MASTER_GOLD
template < class action >
inline void	learning_controller< action >::save_training_sets	( xray::configs::lua_config_value cfg )const
{
	m_value_function.save_training_sets( cfg );
}
#endif // MASTER_GOLD

template < class action >
inline void	learning_controller< action >::load_training_sets	( const xray::configs::binary_config_value &cfg )
{
	m_value_function.load_training_sets( cfg["controller"]["value_function"] );
}

} // namespace rtp
} // namespace xray
