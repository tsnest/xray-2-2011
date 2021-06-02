////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////


#include "skeleton_animation.h"
#include "quaternion_mix.h"
#include "mixing_animation_clip.h"
#include "mixing_animation_state.h"
#include "base_interpolator.h"
#include "quaternion_mix.h"
#include "bone_animation_mixing.h"

namespace xray {
namespace animation {

	inline	animation_layer::animation_layer( const skeleton_animation* b, float factor, float start_time, float blending_time )
		:m_blend( b ), 
		 m_factor( factor ), 
		 m_mix_factor( factor ), 
		 m_time_scale( 1.f ),
		 m_start_time( start_time ),
		 m_blending_time( blending_time ),
		 m_moving_root( u32(-1) ),
	//	 m_cycles( 0 ),
		 m_root_start( zero )
	{
		set_moving_root( 0 );
	}


	inline	animation_layer::animation_layer( const mixing::animation_state & data )
		:m_blend( &data.animation->animation() ), 
		 m_factor( data.weight ), 
		 m_mix_factor( data.weight ), 
		 m_time_scale( 1.f ),
		 m_start_time( data.time * default_fps ),
		 m_blending_time( data.animation->interpolator().transition_time( ) ),
		 m_moving_root( u32(-1) ),
	//	 m_cycles( 0 ),
		 m_root_start( zero )

	{
		set_moving_root( 0 );
		//set_time_scale			( default_fps );
	}

	inline	void	animation_layer::set_factor( float factor )
	{
		ASSERT( factor >= 0.f );
		ASSERT( factor <= 1.f );

		m_factor = factor;// m_mix_factor * factor;
	}

	inline	void	animation_layer::set_time_scale( float scale )
	{
		m_time_scale = scale;
	}

	inline	void	animation_layer::set_start_time( float time )
	{
		m_start_time = time;

	}

	inline	float	animation_layer::max_time( )const
	{
		return m_blend->max_time() * m_time_scale;
	}
	
	inline	float	animation_layer::min_time( )const
	{
		return m_blend->min_time() * m_time_scale;
	}

	inline	float	animation_layer::transition_factor( float time )const 
	{ 
		ASSERT( m_blend );
		if( m_blending_time <= math::epsilon_7)
			return 1.f;
		return m_blend->blend_factor( animation_time( time ), m_blending_time ) ;
	}


	//inline	float	animation_layer::animation_time_cyclic	( float time )const
	//{
	//	return animation_time( time ) + time_length() * m_cycles;
	//}

	inline	float	animation_layer::animation_time( float time )const
	{
		return ( time + m_start_time ) * m_time_scale;
	}

	inline	u32	animation_layer::cycle_animation_time	( float time, float &new_start_time )const
	{
		const float left = time_left( time );
		const float lenght = time_length(); //max_time() - min_time();
		new_start_time = m_start_time;

		if( left >= 0.f )
			return 0;
		
		const float cycle_time = -left;
		
		const u32 num_cycles =  math::floor( cycle_time/lenght ) + 1; //num_cycles - 1

		new_start_time = ( m_start_time - float( num_cycles ) * lenght ); 

		return num_cycles;

	}

	inline	float	animation_layer::time_left( float time )const
	{
		return max_time() - animation_time( time );
	}

	inline	bool animation_layer::in_time_interval	( float time )const
	{
		float anim_time = animation_time( time );
		return ( anim_time >= min_time() ) && ( anim_time <= max_time() );
	}

	inline	void	animation_layer::set_moving_root		( u32 root )
	{
		m_moving_root = root;
		//frame f;
		//m_blend->bone(m_moving_root).get_frame( min_time(), f );
		//f.rotation = float3(0,0,0);
		//set_root_start( f );
	}

	inline	void animation_layer::set_root_start( const frame &f )
	{
		m_root_start = f;
	}

	inline	void animation_layer::add_root_start		( const frame &f )
	{
		add( m_root_start , f );
	}

	inline	void	animation_layer::get_bone_displacement_non_cyclic	( frame &disp, u32 bone, const float2 &interval )const
	{
		frame f0;
		
		//math::clamp( interval.x, min_time(), interval.x );
		//math::clamp( interval.y, interval.y,  max_time() );

		ASSERT( interval.x < interval.y );
		ASSERT( interval.x >= min_time() );
		ASSERT( interval.y <= max_time() );



		get_frame_non_cyclic( bone, interval.y, zero, disp );
		get_frame_non_cyclic( bone, interval.x, zero, f0 );

		
		//to_close_xyz( f0.rotation, disp.rotation, math::pi_d4 );

		sub( disp, f0 );
		
	}


	inline	void	animation_layer::get_bone_displacement	( frame &disp, u32 bone, const float2 &interval )const
	{
		frame f0;

		ASSERT( interval.x < interval.y );
		ASSERT( interval.x >= min_time() );
		ASSERT( interval.y <= max_time() );
		get_frame( bone, interval.y, disp );
		get_frame( bone, interval.x, f0 );
		sub( disp, f0 );
	}

	inline	void	animation_layer::get_frame_non_cyclic( bone_index_type bone, float anim_time, const frame &start_frame, frame &f )const
	{
		m_blend->bone(bone).get_frame( anim_time, f );
		if( bone == moving_root( ) )
		{
			//float4x4 mf = create_rotation ( f.rotation );
			//float4x4 mr = create_rotation (  m_root_start.rotation );
			//mr.try_invert( float4x4( mr ) );
			//mf = mf * mr;

			f.rotation -= start_frame.rotation;
			
			//f.rotation = mf.get_angles_xyz(  );

			f.translation -= start_frame.translation;
		}
		
	}

	inline	void	animation_layer::get_frame( bone_index_type bone, float time, frame &f )const
	{
		
		frame start_frame ;
		float anim_time = cycle_animation_frame( time, start_frame );
		
		get_frame_non_cyclic( bone, anim_time, start_frame, f );

	}

	inline	float	animation_layer::cycle_animation_frame	( float time, frame &new_start_frame )const
	{
		
		
		float start_time = m_start_time;
		new_start_frame = m_root_start;

		u32 cycles = cycle_animation_time( time, start_time );
		
		if( cycles <= 0 )
			return animation_time( time );
		
		frame disp;
		get_bone_displacement_non_cyclic( disp, moving_root( ), float2( min_time(), max_time() ) );

		const float4x4 cycle_disp = frame_matrix( disp );
		
		float4x4 ful_disp = cycle_disp;

		for( u32 i = 0; i < cycles - 1 ; ++i )
			ful_disp = cycle_disp * ful_disp;

		ful_disp.try_invert( float4x4( ful_disp ) );
		
		const float4x4 current_start  = frame_matrix( m_root_start );

		const float4x4 new_start = current_start * ful_disp; // (m *n)^-1 = n^-1 * m^-1

		new_start_frame = matrix_to_frame( new_start );

		return   ( time + start_time ) * m_time_scale; 
	}


}  // namespace animation
}  // namespace xray