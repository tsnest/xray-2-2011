////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TRAJECTORY_INLINE_H_INCLUDED
#define TRAJECTORY_INLINE_H_INCLUDED

#include <xray/render/world.h>
#include <xray/render/facade/debug_renderer.h>

namespace xray {
namespace rtp {

	template< class action >
	trajectory<action>::trajectory	( u32 self_id ): m_id( self_id ), m_count( 0 )
	{
		
	}
	
	template< class action >
	void	trajectory<action>::clear( )
	{ 
		
		m_count = 0;
		u32 max_trajectory_length = g_max_trajectory_length;
		m_samples.resize( max_trajectory_length );  

	} // m_samples.clear();

	//template< class action >
	//void trajectory<action>::add( const training_sample_type& sample, const action* a )
	//{
	//	ASSERT( m_count < max_trajectory_length );
	//	ASSERT( m_count < m_samples.size() );

	//	m_samples[ m_count ] = item_type( sample, a );
	//	
	//	++m_count;
	//	//m_samples.push_back( item_type( sample, a ) );
	//}

	template< class action >
	void	trajectory<action>::add( const space_param_type& sample,separate_reward_type const& rew, const action* a )
	{
		ASSERT( m_count < g_max_trajectory_length );
		ASSERT( m_count < m_samples.size() );

		m_samples[ m_count ] = item_type( a, sample, rew );
		
		++m_count;
	}

	template< class action >
	void	trajectory<action>::add_to_sets	( vector< training_set< space_param_type, training_sample_type > > &sets )
	{
		typename vector<item_type>::iterator i = m_samples.begin(), e = m_samples.begin() + m_count;//m_samples.end();
		
		for( ; i!=e; ++i )
		{
			item_type& it = *i;
			sets[it.const_action->id()].add( it.param, it.reward );
		}
		//m_samples.clear();
	}

	extern u32 g_dbg_trajectory_render_diapason_size;
	extern u32 g_dbg_trajectory_render_diapason_id;
		

	template< class action >	
	void	trajectory<action>::render		( xray::render::scene_ptr const& scene, xray::render::debug::renderer& r, u32 trajectories_count ) const
	{
		
		u32 const	diapasons_count				= trajectories_count/g_dbg_trajectory_render_diapason_size;
		
		u32 const	diapason_start				= ( g_dbg_trajectory_render_diapason_id % diapasons_count ) * g_dbg_trajectory_render_diapason_size;
		u32 const	diapason_end				= diapason_start + g_dbg_trajectory_render_diapason_size;
		if( m_id < diapason_start || m_id >= diapason_end )
			return;


		float3 const disp = float3( 0,1,0 )* float( m_id )* 0.01f ;// * 0.001f * float( m_id );
		//current_world.dbg_add_pose_displacement( disp );
		
		float3 start = float3(0,0,0);
		//current_world.dbg_get_pose( start );
		r.draw_sphere( scene, start + disp, 0.1f, math::color(255,0, u32( float( m_id ) * ( 255.f/trajectories_count ) ) ) );
		
		if( size() < 2 )
			return;

		typename action::world_space_param_type current_world;

		typename vector<item_type>::const_iterator b = m_samples.begin(), e = m_samples.begin() + m_count;
		typename vector<item_type>::const_iterator i = b; 
		for( ; i!=e-1; ++i )
		{
			const item_type& it = *i;
			const item_type& next_it = *(i+1);

			if( m_in_target && i == (e - 2) )
				it.param.render( next_it.param, scene, r, math::color( 255, 0, 0 ) );
			else
				it.param.render( next_it.param, scene, r, math::color( u8(m_in_target) * 255, ( m_id + 100 ) % 256,  ( m_id + 200 ) % 256 ) );

			//it.const_action->render( i, it.param, to, r  );

			it.const_action->debug_render_trajectory	(		math::color( u8(m_in_target) * 255, ( m_id + 100 ) % 256,  ( m_id + 200 ) % 256 ), (i - b),
																current_world,
																it.param,
																it.reward,
																next_it.param,	
																*next_it.const_action,  
																next_it.reward,
																scene,
																r
																);


		}
	

	}

template< class action >	
inline trajectory<action>::item_type::item_type( const action_type* a, space_param_type const& par, separate_reward_type const& rew ):
	const_action( a ),
	param		( par ),
	reward		( rew )
{

}

template< class action >	
inline trajectory<action>::item_type::item_type( item_type const &other ):
	const_action( other.const_action ),
	param		( other.param ),
	reward		( other.reward )
{

}

template< class action >	
inline trajectory<action>::item_type::item_type( ):
	const_action	( 0 )
{

}

//template< class action >	
//inline trajectory<action>::item_type&	trajectory<action>::item_type::operator = ( trajectory<action>::item_type const &other )
//{
//	(*this ) = other;
//}

} // namespace rtp
} // namespace xray

#endif // #ifndef TRAJECTORY_INLINE_H_INCLUDED