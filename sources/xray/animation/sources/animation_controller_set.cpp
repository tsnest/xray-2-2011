////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_controller_set.h"
#include "animation_group_action.h"
#include "animation_mix.h"
#include "animation_group.h"

#include <xray/render/base/debug_renderer.h>

//#define FOOT_SHIFT_DEBUG

namespace xray {
namespace animation {

static const u32 max_group_mem_size = animation_controller_set::max_group_size *  sizeof( mixing_animation_type );

animation_controller_set::animation_controller_set( ):
m_mix( 0 ),
m_id_current( u32(-1) ),
m_id_step( u32(-1) ),
m_action_start_time( 0 ),
m_action_start_local_time( 0 ),
m_update_time( 0 ),
m_animation_in( MALLOC( max_group_mem_size, "animation_controller_set::m_animation_in " ), max_group_size ),
m_animation_out( MALLOC( max_group_mem_size, "animation_controller_set::m_animation_out " ), max_group_size ),
m_fixed_bones( 0 ),
m_fixed_bone( u32(-1)  ),
m_fixed_vetex_global( float3( 0, 0, 0 ) ),
m_fixed_global_shift( float3( 0, 0, 0 ) ),
m_dbg_shift_fixed_action( float3( 0, 0, 0 ) ),
m_moving_root_bone( 0 ),
m_sum_displacement( float4x4().identity() ),
b_loaded( false )
{
	
	m_sum_displacement.c.xyz().y += 0.1f;
	
	resources::request	resources[]	= {
		{ "resources/animations/controller_set.lua",	xray::resources::config_lua_class },
		{ "resources/animations/skeletons/human",		xray::resources::skeleton_class },
	};

		xray::resources::query_resources(
		resources,
		array_size(resources),
		boost::bind( &animation_controller_set::on_config_loaded, this, _1 ),
		g_allocator
	);
	
	
}


animation_controller_set::~animation_controller_set()
{
	m_actions.clear();
	clear_groups();
	DELETE( m_mix );
	DELETE( m_fixed_bones );
	m_animation_out.clear();
	m_animation_in.clear();
	mixing_animation_type	*buffer = m_animation_in.begin();
	FREE( buffer );
	buffer = m_animation_out.begin();
	FREE( buffer );
}

i_animation_action const *animation_controller_set::taget_action( )const
{
	return &m_actions[0];
}

void	animation_controller_set:: get_taget_transform ( float4x4 &m )const
{
	m.identity();
	m.c.xyz().set( 0, 0, 0.5f );
}

u32	animation_controller_set::num_actions( )const
{
	return m_actions.size();
}

i_animation_action const *animation_controller_set::action( u32 id )const
{
	return &m_actions[ id ];
}

void animation_controller_set::clear_groups( )
{
	vector<animation_group*>::iterator	i = m_groups.begin(), e = m_groups.end();
	for( ; i!=e; ++i )
		DELETE(*i);
}


void	animation_controller_set::add_group( animation_group *group )
{
	ASSERT( group );
	group->set_callback_on_loaded( boost::bind( &animation_controller_set::callback_group_loaded, this ) );
	m_groups.push_back( group );
	m_actions.push_back( animation_group_action( group, this ) );
}

inline animation_group_action const& animation_controller_set::current( )const
{
	ASSERT(m_id_current != u32(-1));
	ASSERT( m_id_current < m_actions.size() );
	return m_actions[ m_id_current ];
}

static void apply_factor( animation_vector_type &anims, float factor )
{
	const u32 count = anims.size();

	ASSERT( factor <= 1.f && factor >= 0.f  );



	ASSERT( sum_weights( anims ) <= 1.f );
	ASSERT( sum_weights( anims ) >= 0.f );


	for( u32 i =0; i < count; ++i )
	{
		anims[i].set_factor(  factor * anims[i].mix_factor( ) );
	}

	ASSERT( sum_weights( anims ) <= 1.f );
}

void	set_time( animation_vector_type	& anims, float time  )
{
	const u32 count = anims.size();

	for( u32 i =0; i < count; ++i )
	{
		anims[i].set_start_time( time );
	}
}



void animation_controller_set::apply_transition( float time_global  )
{

	if( m_animation_out.size() == 0 )
		return;

	ASSERT( m_animation_in.size()  != 0 );
	ASSERT( m_animation_out.size() != 0 );
	
	const float time_action = action_time( time_global );
	
	
	float in_factor		= m_animation_in[0].transition_factor( time_action );
	float out_factor	= 1.f - in_factor;

	ASSERT( in_factor >=  0.f );
	ASSERT( in_factor <=  1.f );

	ASSERT( out_factor >= 0.f );
	ASSERT( out_factor <= 1.f );

	ASSERT(  sum_weights( m_animation_in ) <= 1.f   );
	ASSERT(  sum_weights( m_animation_out ) <= 1.f   );
	
	ASSERT(  sum_weights( m_animation_in ) >= 0.f   );
	ASSERT(  sum_weights( m_animation_out ) >= 0.f   );
	

	apply_factor( m_animation_in, in_factor );
	
	if( out_factor < math::epsilon_5 )
	{
		m_animation_out.clear( );
		return;
	} 


	apply_factor( m_animation_out, out_factor );

	ASSERT( math::similar( sum_weights( m_animation_in ) + sum_weights( m_animation_out ), 1.f )  );

}

void animation_controller_set::set_mix( )
{
	
	ASSERT( m_mix );

	m_mix->clear();
	ASSERT( math::similar( sum_weights( m_animation_in ) + sum_weights( m_animation_out ), 1.f )  );
	ASSERT( ( sum_weights( m_animation_in ) + sum_weights( m_animation_out ) <= 1.f + math::epsilon_6 )  );
	m_mix->insert( m_animation_in  );
	m_mix->insert( m_animation_out );
	
}


inline	float	animation_controller_set::action_time		( float time_global )const
{
	
	ASSERT( time_global >= m_action_start_time );
	float ret_action_time = time_global - m_action_start_time;

	return ret_action_time;

}

void animation_controller_set::start_action(  const animation_group_action& next_action, float time_global, float time_action, const buffer_vector< float > &weights  )
{

	m_action_start_time = time_global;
	m_action_start_local_time = time_action;
	m_id_current = next_action.id();
	m_actions[ next_action.id() ].animation().play( m_animation_in, weights, time_action );

}

void	g_calculate_anim_fixed_vertex( float3 &pos, const fixed_bone &fb, const animation_mix& mix, float time_anim )
{
	u32 anim_fixed_bone = fb.m_anim_skeleton_idx;

	float4x4 bone;
	mix.bone_matrix( anim_fixed_bone, time_anim, bone );

	pos = fb.m_fixed_vertex *  bone;
}

void g_calculate_anim_fixed_vertex( float3 &pos, const fixed_bone &fb, const animation_mix& mix, float time_anim, 
								   float4x4 const &sum_displacement, float3 const& fixed_global_shift )
{

	animation::g_calculate_anim_fixed_vertex( pos, fb, mix, time_anim );
	
	float4x4 world_anim = sum_displacement;
	world_anim.c.xyz() +=  fixed_global_shift;


	pos = pos *  world_anim;

}

float3 g_fixed_bone_shift(  const fixed_bone &fb, const animation_mix& mix, float time_anim, 
								   float4x4 const &sum_displacement, 
								   float3 const& fixed_global_shift,
								   float3 const& fixed_vetex_global
								   )
{
	float3 current_anim_fixed_vetex_global;
	g_calculate_anim_fixed_vertex( 
		current_anim_fixed_vetex_global, fb,
		mix, time_anim, 
		sum_displacement, fixed_global_shift );

	float3 shift = fixed_vetex_global - current_anim_fixed_vetex_global;
	shift.y = 0;
	
	return  shift;
}



void animation_controller_set::start_action_transition(  const animation_group_action& next_action, float time_global, const buffer_vector< float > &weights, const buffer_vector< float > &new_weights  )
{


	ASSERT ( m_animation_out.empty() );

	const float current_action_time = action_time( time_global );
	const float current_anim_time = current().animation().time(); 
	const float current_anim_time_left = current_anim_time - m_action_start_local_time - current_action_time;
	
	if( current_anim_time_left >= 0 )
	{
		m_animation_out = m_animation_in ;
		set_time( m_animation_out, current_action_time  );
	} else 
		LOG_DEBUG( "transition without blend" );

///////////////////////////////////////////////////////////////////////
	float4x4 local_displacement = float4x4().identity();
	frame displacement_frame = zero;

	if( is_active() )
	{


		float3 foot_displacement;
		current().get_displacement( displacement_frame, foot_displacement, weights );
		frame_matrix( displacement_frame, local_displacement );
	}
////////////////////////////////////////////////////////////////////////

	//float next_anim_action_time = next_action.animation().overlapp_time() - current_anim_time_left;
	float next_anim_action_time = current().animation().tail_time() - current_anim_time_left;
	const float next_anim_time =  next_action.animation().time();
	while( next_anim_action_time >= next_anim_time )
		next_anim_action_time -= next_anim_time;
	

	ASSERT( next_anim_action_time >= 0 );
	start_action( next_action, time_global, next_anim_action_time, new_weights ); //time_global 
	
	set_root_start( displacement_frame, m_animation_out );
	m_sum_displacement = local_displacement * m_sum_displacement; 

}

float	animation_controller_set::time_left( const animation_group_action& next_action, float time_global )
{
	XRAY_UNREFERENCED_PARAMETER( next_action );
	

	float plaing_time = current().animation().play_time(); /// current().animation().time()- next_action.animation().overlapp_time();


	//ASSERT_CMP_U( current().animation().tail_time(), >=,  next_action.animation().overlapp_time() );

	float	 passed_time =  m_action_start_local_time + action_time(time_global) ;
	
	return   plaing_time - passed_time;
}


void	animation_controller_set::foot_steps_update( float time_global, const buffer_vector< float > &weights  )
{
	
	
	u32 fixed_bone_id =  current().animation().fixed_bone( action_time( time_global ), weights );

	fixed_bone_start( fixed_bone_id, time_global, weights  );

}




void	animation_controller_set::update(  float time_global, const buffer_vector< float > &weights  )
{
	

	apply_transition( time_global );
	set_mix( );

	foot_steps_update( time_global, weights );

	m_update_time = time_global;

}

void	animation_controller_set::init( u32 id, const buffer_vector< float > &weights, float time_global )
{
		m_animation_out.clear();
		start_action( m_actions[ id ], time_global, 0, weights );
}

void	animation_controller_set::start_transition( const animation_group_action& next_action, const buffer_vector< float > &weights, const buffer_vector< float > &new_weights, float time_global )
{


	//apply_transition( time_global );
	update(  time_global, weights );

	start_action_transition( next_action, time_global, weights, new_weights );
	update(  time_global, new_weights );


#ifdef FOOT_SHIFT_DEBUG
	m_dbg_shift_fixed_action = m_fixed_global_shift;
	fixed_bone_start( u32(-1), time_global, new_weights  );
	m_dbg_shift_fixed_action = m_fixed_global_shift;
	fixed_bone_start( m_fixed_bone, time_global, new_weights  );

	m_dbg_shift_fixed_action = m_fixed_global_shift;

#endif // FOOT_SHIFT_DEBUG

}

void	animation_controller_set::dbg_test_run( const animation_group_action& next_action, const buffer_vector< float > &weights, float time_global )
{
	
	ASSERT( &m_actions[ next_action.id() ] == &next_action );
	
	if(  m_id_current == u32(-1) )
	{
		init( next_action.id(), weights, time_global );
		update( time_global, weights );
		return ;
	}

	if( time_left( next_action, time_global) > math::epsilon_7 )
	{
		update( time_global, weights );
		return ;
	}

	start_transition( next_action, weights, weights, time_global );

}



void	animation_controller_set::test_update( float time_global )
{

	
	if( !b_loaded )
	{
		check_loaded();
		return;
	}

	ASSERT( m_mix );

	if( count() == 0 )
		return;
	
	u32 id_next = 0;

	if( is_active() )
		id_next =  ( current().id() + 1 ) % count( );

	const animation_group_action &next = action( id_next ) ;

	float4x4 disp;

	const u32 animations_count = next.animation().animation_count();
	buffer_vector<float>	weights( ALLOCA( sizeof( float ) * animations_count ), animations_count );
	weights.resize( animations_count );

	const float factor = 1.f/animations_count;
	for( u32 i = 0; i < animations_count;  ++i )
		weights[i] = factor;


	dbg_test_run( next, weights, time_global );
///////////////
	if( animations_count == 0 || m_mix->animations_count( ) == 0 )
		return;
	calculate_bones	( );
/////////////////
}


void	animation_controller_set::calculate_anim_fixed_vertex( float3 &pos, float time_global ) const
{
	ASSERT( m_mix );

	R_ASSERT( m_fixed_bones );

	const fixed_bone &fb = m_fixed_bones->bone( m_fixed_bone );
	const float time = action_time( time_global ) ;

	
	g_calculate_anim_fixed_vertex( pos, fb, *m_mix, time, m_sum_displacement, m_fixed_global_shift );

}

float3	animation_controller_set::fixed_bone_shift( float time_global )const
{
	if( m_fixed_bone == u32(-1) )
		return float3( 0, 0, 0 );



	const float3 shift   = g_fixed_bone_shift(  m_fixed_bones->bone( m_fixed_bone ),
							*m_mix,  action_time( time_global ), 
							m_sum_displacement, m_fixed_global_shift, m_fixed_vetex_global );

	return  shift;

}

float3 fixed_foots_displacement( const fixed_bones &f_bones, const foot_steps &f_steps, const buffer_vector< float > &weights, const animation_mix &m, float time );

void	animation_controller_set::fixed_bone_start( u32 bone, float time_global, const buffer_vector< float > &weights )
{
	XRAY_UNREFERENCED_PARAMETER( weights );
	
	if( bone == m_fixed_bone  )
		return;
	
	float3 bone_shift =  fixed_bone_shift( time_global );


#ifdef FOOT_SHIFT_DEBUG
	float time_int_start;
	//bone =
	current().animation().foots().bone( action_time( time_global ), weights, time_int_start );

	 if( m_fixed_bone != u32(-1)  )
		 bone_shift = g_fixed_bone_shift(  m_fixed_bones->bone( m_fixed_bone ),
											*m_mix, time_int_start, 
											m_sum_displacement, m_fixed_global_shift, m_fixed_vetex_global 
											);

	float3 dbg_shift_calculated = fixed_foots_displacement( *m_fixed_bones, current().animation().foots(),weights, *m_mix, time_int_start );
	float4x4 mat = m_sum_displacement; 
	mat.c.xyz() = float3(0,0,0);
	
	
	dbg_shift_calculated = dbg_shift_calculated * mat;

	float3 dbg_shift_test = ( m_fixed_global_shift + bone_shift ) - m_dbg_shift_fixed_action;
	if( bone != u32(-1) )
	{
		LOG_DEBUG( "dbg_shift_test       %f, %f, mag %f", dbg_shift_test.x, dbg_shift_test.z, dbg_shift_test.magnitude()  );
		LOG_DEBUG( "dbg_shift_calculated %f, %f, mag %f", dbg_shift_calculated.x, dbg_shift_calculated.z, dbg_shift_calculated.magnitude() );
	}
#endif //FOOT_SHIFT_DEBUG

	
	m_fixed_global_shift += bone_shift;
	m_fixed_bone = bone;
	if( m_fixed_bone == u32(-1) )
		return;
	calculate_anim_fixed_vertex( m_fixed_vetex_global, time_global );

#ifdef FOOT_SHIFT_DEBUG
	const fixed_bone &fb = m_fixed_bones->bone( m_fixed_bone );
	g_calculate_anim_fixed_vertex( m_fixed_vetex_global, fb, *m_mix, time_int_start, m_sum_displacement, m_fixed_global_shift );
#endif //FOOT_SHIFT_DEBUG

}

void	animation_controller_set::calculate_bones()const
{
	ASSERT( m_mix );
	const u32 count = m_mix->bone_count();
	buffer_vector<float4x4>	matrices ( ALLOCA( count * sizeof(float4x4) ), count );
	float l_action_time = action_time( m_update_time );
	m_mix->calculate_bones( l_action_time, matrices );

}

void dbg_draw_matrix( render::debug::renderer& renderer, const float4x4 &m )
{
	const float scale = 1.f;
	const float3 start = m.c.xyz();

	float3 end =start + m.i.xyz()*scale;
	renderer.draw_line( start,end, math::color_xrgb(255,0,0) );
	
	end =start + m.j.xyz()*scale;
	renderer.draw_line( start,end, math::color_xrgb(0,255,0) );

	end =start + m.k.xyz()*scale;
	renderer.draw_line( start,end, math::color_xrgb(0,0,255) );
}

bool animation_controller_set::ready()const
{
	if( !b_loaded )
		return false;
	
	ASSERT( m_mix );
	return  is_active() && ( m_mix->animations_count() != 0 );

}

void	animation_controller_set::get_bone_world_matrix( float4x4 &m, u32 bone_id ) const
{
	m = float4x4().identity();

	if( !ready() )
		return;

	float l_action_time = action_time( m_update_time );
	math::clamp( l_action_time, 0.f, current().animation().play_time() );//?time

	float4x4 bone_matrix;
	m_mix->bone_matrix( bone_id, l_action_time,bone_matrix );
	m = bone_matrix * object_position();

}


float4x4 animation_controller_set::object_position() const
{
	float4x4 m = m_sum_displacement;
	
	float3 bone_shift =   fixed_bone_shift( m_update_time );



	m.c.xyz() += m_fixed_global_shift + bone_shift;

	return m;
}

void	animation_controller_set::render( render::debug::renderer& renderer, float /*time_global*/ ) const
{

	if( !ready() )
		return;

	float l_action_time = action_time( m_update_time );

	math::clamp( l_action_time, 0.f, current().animation().play_time() );//time

	ASSERT(l_action_time>=0.f);
	

	float4x4 m = object_position();

	m_mix->render( renderer, m, l_action_time );
	
//////
	const float3 start = m.c.xyz();
	float3 from = start + float3(0,2,0);


	const float scale = 0.1f;

	float3 to0 = from  + float3( 0, + current().animation().play_time()  * scale, 0 ); 
	renderer.draw_line( from,to0, math::color_xrgb(0,255,0) );

	float3 to = from  + float3( 0, + l_action_time * scale, 0 ); 
	renderer.draw_line( from,to, math::color_xrgb(255,0,0) );



//////
	
	if( m_fixed_bone != (u32(-1)) )
		renderer.draw_sphere( m_fixed_vetex_global, 0.01f, math::color_xrgb(255,0,0) );


	
	dbg_draw_matrix( renderer, m );
}


void	animation_controller_set::query_resources		( configs::lua_config_value const& config )
{

	const u32 sz = config.size();

	resources::request	*resources = (resources::request*) ALLOCA( sizeof( resources::request ) * sz );
	
	for( u32 i = 0; i < sz; ++i )
	{
		resources[i].path	= config[i];
		resources[i].id		= xray::resources::config_lua_class;
	}
	xray::resources::query_resources(
			resources,
			sz,
			boost::bind( &animation_controller_set::on_resources_loaded, this, _1 ),
			xray::animation::g_allocator
		);

}

void	animation_controller_set::on_config_loaded( xray::resources::queries_result& resource )
{
	
	R_ASSERT( !resource.is_failed() );

	ASSERT( resource.size() == 2 );
	
	m_skeleton = resource[1].get_unmanaged_resource();
	xray::configs::lua_config_ptr config	= static_cast_resource_ptr<xray::configs::lua_config_ptr>(resource[0].get_unmanaged_resource());
	xray::configs::lua_config_value const cfg= *config;

	const skeleton * pskeleton		= static_cast_checked<const skeleton*>( m_skeleton.c_ptr() );

	R_ASSERT( !m_fixed_bones );
	R_ASSERT( pskeleton );


	m_fixed_bones		= NEW (fixed_bones)	(  *pskeleton );
	
	m_fixed_bones->read( cfg["animation_controller_set"][ "fixed_bones" ] );

	query_resources( cfg["animation_controller_set"][ "animation_groups" ]  );

	
}

void animation_controller_set::on_resources_loaded( xray::resources::queries_result& resource )
{

	R_ASSERT( !resource.is_failed() );

	u32 count = resource.size();

	

	const skeleton * pskeleton		= static_cast_checked<const skeleton*>( m_skeleton.c_ptr() );
	

	for(u32 i = 0; i < count; ++i  )
	{
		xray::configs::lua_config_ptr config	= static_cast_resource_ptr<xray::configs::lua_config_ptr>(resource[i].get_unmanaged_resource());

		animation_group *group =  NEW(animation_group)( *config, pskeleton, *m_fixed_bones ) ;
		group->set_name( resource[i].get_requested_path() );
		add_group( group );

	}

	m_mix				= NEW( animation_mix )( *pskeleton );

}

void	animation_controller_set::callback_group_loaded	( )
{
	if( check_loaded() && m_callback_on_loaded )
		m_callback_on_loaded();
		
}

bool	animation_controller_set::check_loaded()
{
	
	if( !m_mix )
		return false;

	vector<animation_group*>::const_iterator i = m_groups.begin(), e = m_groups.end();
	for( ; i != e ; ++i )
		if( !( *i )->loaded() )
			return false ;
	b_loaded = true;
	return true;
}




} // namespace animation
} // namespace xray