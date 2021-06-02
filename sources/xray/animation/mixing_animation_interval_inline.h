////////////////////////////////////////////////////////////////////////////
//	Created		: 24.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MIXING_ANIMATION_INTERVAL_INLINE_H_INCLUDED
#define MIXING_ANIMATION_INTERVAL_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline animation_interval::animation_interval	(
		skeleton_animation_ptr const& animation,
		float const start_time,
		float const length
	) :
	m_animation		( animation ),
	m_start_time	( start_time ),
	m_length		( length )
{
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef MIXING_ANIMATION_INTERVAL_INLINE_H_INCLUDED