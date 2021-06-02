////////////////////////////////////////////////////////////////////////////
//	Created		: 22.03.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_group_action.h"
#include "animation_controller_set.h"
#include "animation_group.h"

#include <xray/animation/anim_track_common.h>

namespace xray {
namespace animation {

	animation_group_action::animation_group_action( animation_group const *group, animation_controller_set *set ):
	m_group	( group ),
	m_set	( set ),
	m_id	( set->count() )
	{	
		ASSERT( group );
		ASSERT( set );
	}


	float animation_group_action::run(  i_animation_controller_set *set, const buffer_vector< float > &from_weights, const buffer_vector< float > &weights, u32 step, float time_global )const
	{
		ASSERT( m_set );
	
		
		animation_controller_set  *s =  static_cast_checked< animation_controller_set* > ( set );

		
		if( s->id_current() == u32(-1) )
		{
			s->init( id(), weights, time_global );
			s->id_step() = step;
			//LOG_DEBUG(" animation_group_action  set restart");
		}

		if( s->id_step() != step )
		{
			s->id_step() = step;
			LOG_DEBUG("animation_group_action start transition");
			s->start_transition( *this,  from_weights, weights, time_global );
			//LOG_DEBUG("animation_group_action start transition");
		}
		else
			s->update(  time_global, weights );

		return s->time_left(  *this, time_global );
	}

	u32		animation_group_action::animations_number( ) const
	{
		return animation().animation_count();
	}

	void	animation_group_action::get_displacement( frame &f, float3 &foot_displacement,  const buffer_vector< float > &weights )const
	{
			
			ASSERT( m_set );
			animation().get_displacement( f, foot_displacement, m_set->moving_moving_bone(), m_set->get_fixed_bones(), weights );
			float3 to_rotation = float3( 0, f.rotation.y, 0 );
			to_close_xyz( f.rotation, to_rotation, math::pi_d2  );//math::pi_d4
			f.rotation.x = 0;
			f.rotation.z = 0;
			f.translation.y = 0;
	}

	void animation_group_action::get_displacement_transform( float4x4 &m,  const buffer_vector< float > &weights )const
	{
		frame f;
		float3 foot_displacement;
		get_displacement( f, foot_displacement, weights );
		f.translation += foot_displacement;
		m = frame_matrix( f );

	}

	float	animation_group_action::duration( )const
	{ 
		return m_group->play_time();
	}



	u32		animation_group_action::bone_index( pcstr bone_name )const
	{
		ASSERT( m_group );
		return m_group->bone_index( bone_name );
	}

	void	animation_group_action::local_bone_matrix( float4x4 &m,  u32 bone_idx,  const buffer_vector< float > &weights )const
	{
		
		animation().get_end_matrix( m, m_set->moving_moving_bone(), weights );
		float4x4 i_root; i_root.try_invert( m );

		animation().get_end_matrix( m, bone_idx, weights );
		
		m = m * i_root;

	}


	pcstr	animation_group_action::name( )const
	{
		ASSERT( m_group );
		return m_group->name();
	}

	u32		animation_group_action::gait						( )const
	{ 
		ASSERT( m_group );
		return m_group->gait();
	}


} // namespace animation
} // namespace xray