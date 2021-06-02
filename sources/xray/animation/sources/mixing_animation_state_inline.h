////////////////////////////////////////////////////////////////////////////
//	Created		: 21.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_ANIMATION_STATE_INLINE_H_INCLUDED
#define MIXING_ANIMATION_STATE_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline animation_state::animation_state	(
		animation_clip const& animation,
		animation::bone_mixer_data& bone_mixer_data,
		float const time,
		float const weight
	) :
	animation		( &animation ),
	bone_mixer_data	( &bone_mixer_data ),
	time			( time ),
	weight			( weight )
{
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_ANIMATION_STATE_INLINE_H_INCLUDED