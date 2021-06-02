////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_CONTROLLER_INLINE_H_INCLUDED
#define ANIMATION_CONTROLLER_INLINE_H_INCLUDED

#include <xray/animation/world.h>
#include <xray/animation/i_animation_controller.h>

namespace xray {
namespace rtp {

template < class controller_type >
animation_controller< controller_type >::animation_controller( pcstr name, xray::animation::i_animation_controller_set *set ):
super( name, typename action_type::global_actions_params_type ( set ) ), 
	m_loaded( true ),
	m_animation_world( 0 ),
	//m_controller_position( set->position() ),
	m_debug_wolk_initialized( false ),
	m_animation_controller( 0 )
{
	R_ASSERT( set );
	R_ASSERT( set->loaded() );
	
}

template < class controller_type >
inline animation_controller< controller_type >::animation_controller( pcstr name, animation::world &w, pcstr animation_set_path   ):
super( name, typename action_type::global_actions_params_type ( 0 ) ), 
	m_loaded( false ),
	m_animation_world( &w ),
	//m_controller_position( float4x4().identity() ),
	m_debug_wolk_initialized( false ),
	m_animation_controller( 0 )
{
	
//	super::base().global_params().m_set = w.create_controller_set( animation_set_path );
	super::base().global_params().m_set->set_callback_on_loaded( 

			boost::bind( &animation_controller< controller_type >::on_load_set_controller_create, this ) 

		);
}

template < class controller_type >
inline animation_controller< controller_type >::animation_controller ( pcstr name, animation::world &w, pcstr animation_set_path, const xray::configs::binary_config_ptr &cfg ):
super( name, typename action_type::global_actions_params_type ( 0 ) ), 
	m_loaded( false ),
	m_animation_world( &w ),
	m_debug_wolk_initialized( false ),
	m_animation_controller( 0 )
{
//		super::base().global_params().m_set = w.create_controller_set( animation_set_path, cfg  );

		super::base().global_params().m_set->set_callback_on_loaded( 

			boost::bind( &animation_controller< controller_type >::on_load_set_controller_create, this ) 

		);
}

template < class controller_type >
inline animation_controller< controller_type >::animation_controller( pcstr name, animation::world &w,  const xray::configs::binary_config_ptr &config_ptr ):
super( name, typename action_type::global_actions_params_type ( 0 ) ), 
	m_loaded( false ),
	m_animation_world( &w ),
	m_debug_wolk_initialized( false ),
	m_config ( config_ptr ),
	m_animation_controller( 0 )
{
	const xray::configs::binary_config_value cfg = m_config->get_root( );

//	super::base().global_params().m_set = w.create_controller_set( cfg["controller"]["animation_set_path"] );
	super::base().global_params().m_set->set_callback_on_loaded( 

			boost::bind( &animation_controller< controller_type >::on_load_set_controller_load, this ) 

		);
}

template < class controller_type >
inline animation_controller< controller_type >::~animation_controller( )
{
	if( m_animation_world )
	{
//		m_animation_world->destroy_controller( m_animation_controller );
//		m_animation_world->destroy_controller_set( super::base().global_params().m_set );
	}

}

template < class controller_type >
inline		render::skeleton_model_ptr	animation_controller< controller_type >::model( )
{
	if( !m_animation_controller )
		return render::skeleton_model_ptr( 0 );

	return m_animation_controller->model();
}

template < class controller_type >
inline void	animation_controller< controller_type >::render_impl	(
		render::scene_ptr const& scene,
		render::scene_renderer& scene_renderer,
		render::debug::renderer& debug_renderer,
		render::skeleton_model_ptr const& model,
		bool render_debug_info
	) const
{
	if( !m_animation_controller )
		return;
	ASSERT( m_animation_controller );
	m_animation_controller->render( scene, scene_renderer, debug_renderer, model, super::action_time(), render_debug_info );
	debug_render( scene, scene_renderer, debug_renderer );
}

extern bool b_render_learn;

template < class controller_type >
inline void		animation_controller< controller_type >::debug_render(
		render::scene_ptr const& scene,
		render::scene_renderer& scene_renderer,
		render::debug::renderer& debug_renderer
	) const
{
	if( !m_loaded )
		return;
	
	if( b_render_learn )
		super::render_value( scene, debug_renderer );
	ASSERT( m_animation_controller );
	m_animation_controller->render( scene, scene_renderer, debug_renderer, m_animation_controller->model(), super::action_time(), true );

}

template < class controller_type >
inline void	animation_controller< controller_type >::cached_data_load( )
{
	ASSERT( super::base().global_params().m_set );

	u32 actions_count = super::base().size();

	for( u32 i = 0; i < actions_count; ++i )
		super::base().get( i )->cached_data_query( boost::bind( &animation_controller< controller_type >::on_action_cached_data_loaded, this ) );

}

template < class controller_type >
inline void	animation_controller< controller_type >::on_action_cached_data_loaded	( )
{
	
	ASSERT( super::base().global_params().m_set );

	u32 actions_count = super::base().size();
	
	for( u32 i = 0; i < actions_count; ++i )
		if ( !super::base().get( i )->motion_data_query_returned(  ) )
			return;
	m_loaded = true;
	
}

template < class controller_type >
void	animation_controller< controller_type >::actions_config_load( )
{
	ASSERT( super::base().global_params().m_set );
	u32 actions_count = super::base().size();
	
	for( u32 i = 0; i < actions_count; ++i )
		super::base().get( i )->load_config(  );
}

template < class controller_type >
inline void	animation_controller< controller_type >::on_load_set_controller_create(  )
{
	create_actions( );
	R_ASSERT( super::base().global_params().m_set );

//	m_animation_controller	= m_animation_world->create_controller( *super::base().global_params().m_set );
	
	m_controller_position	= action_type::world_space_param_type( *m_animation_controller );
	actions_config_load( );
	cached_data_load( );
	//m_loaded = true;
}

template < class controller_type >
inline void	animation_controller< controller_type >::on_load_set_controller_load(  )
{
	
	R_ASSERT( super::base().global_params().m_set );

//	m_animation_controller	= m_animation_world->create_controller( *super::base().global_params().m_set );
	m_controller_position = action_type::world_space_param_type( *m_animation_controller );
	load( m_config->get_root() );
	actions_config_load( );
	cached_data_load( );
	//m_loaded = true;
}

//template < class controller_type >
//void	loaded_callback_create_from_config( )
//{
//	
//	R_ASSERT( super::base().global_params().m_set );
//
//}

template < class controller_type >
inline void	animation_controller< controller_type >::create_actions( )
{
		
	ASSERT( super::base().global_params().m_set );

	const u32 num_actions = super::base().global_params().m_set->num_actions();
	for( u32 i = 0; i < num_actions; ++i )
	{
		action_type *a = NEW( action_type )( super::base() , super::base().global_params().m_set->action(i) );
		a->construct();
	}

}



template < class controller_type >
inline bool	animation_controller< controller_type >::dbg_update_walk( float dt )
{ 
	
	XRAY_UNREFERENCED_PARAMETERS( dt );
	
	if( super::value().empty() )
		return false;

	if( !m_debug_wolk_initialized  )
	{
		ASSERT(  super::base().global_params().m_set );

		walk_init( start_action_id, 0, m_controller_position );
		m_debug_wolk_initialized = true;

	}

	 ASSERT( m_animation_controller );
	 m_controller_position.controller_pos =   m_animation_controller->position();

	 return false;
}

template < class controller_type >
animation::i_animation_controller* animation_controller< controller_type >::get_controller( )
{
	if(m_animation_controller)
		return m_animation_controller;

	return NULL;
}


#ifndef MASTER_GOLD
template < class controller_type >
inline void		animation_controller< controller_type >::save					( xray::configs::lua_config_value cfg, bool training_trees )const
{
	super::save( cfg, training_trees );
	R_ASSERT( super::base().global_params().m_set );
	cfg["controller"][ "animation_set_path" ] = super::base().global_params().m_set->path();
}
#endif // MASTER_GOLD

template <class action>
inline void		animation_controller<action>::learn_init		( )
{
	if( super::learn_initialized() )
		return;

	u32 actions_count = super::base().size();

	for( u32 i = 0; i < actions_count; ++i )
		super::base().get( i )->cache_transforms( );

	super::learn_init( );
}

template <class action>
inline void		animation_controller<action>::load					( const xray::configs::binary_config_value& cfg )
{
	super::load( cfg );
	
}


template <class action>
float4x4 animation_controller<action>::get_object_matrix_for_camera	( ) const
{
	if( !m_animation_controller )
		return float4x4().identity();

	ASSERT( m_animation_controller );

	float4x4 result;
	m_animation_controller->get_bone_world_matrix( result, 0 );
	return result;
}


} // namespace rtp
} // namespace xray

#endif // #ifndef ANIMATION_CONTROLLER_INLINE_H_INCLUDED