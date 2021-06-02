////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_ANIMATION_BONE_ANIMATION_INLINE_H_INCLUDED
#define XRAY_ANIMATION_BONE_ANIMATION_INLINE_H_INCLUDED

namespace xray {
namespace animation {

inline bool hemi_distant( float angle_dist )
{
	return abs( angle_dist ) >= math::pi_d2-math::epsilon_6;
}

//angle_rotation_representation_reduce


inline void evaluate_frame( float time, const animation_curve_type channels[ channel_max ], frame &f, current_frame_position &frame_pos )
{
	for ( u32  ch = channel_translation_x; ch < channel_max; ++ch )
		channels[ ch ].evaluate( time, f.channels[ ch ], frame_pos.current_domain( enum_channel_id ( ch ) ) );
}

inline		void	 bone_animation::get_end_frame_position	( current_frame_position &frame_pos )const
{
	for ( u32  ch = channel_translation_x; ch < channel_max; ++ch )
	{
		ASSERT( m_channels[ ch ].num_points() > 0 );
		frame_pos.current_domain( enum_channel_id ( ch ) ) = m_channels[ ch ].num_points() - 1;
	}
}

inline void bone_animation::get_frame( float time, frame &f, current_frame_position &frame_pos )const
{
	evaluate_frame( time, m_channels, f, frame_pos );
}

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_BONE_ANIMATION_INLINE_H_INCLUDED