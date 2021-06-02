////////////////////////////////////////////////////////////////////////////
//	Created		: 21.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMTION_MIXING_ANIMATION_INTERVAL_H_INCLUDED
#define XRAY_ANIMTION_MIXING_ANIMATION_INTERVAL_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

class animation_clip;

class animation_interval {
public:
	inline							animation_interval	(
										skeleton_animation_ptr const& animation,
										float start_time,
										float length
									);
	inline	skeleton_animation_ptr const&	animation	( ) const { return m_animation; }
	inline	float					start_time			( ) const { return m_start_time; }
	inline	float					length				( ) const { return m_length; }

private:
	skeleton_animation_ptr			m_animation;
	float							m_start_time;
	float							m_length;
}; // class animation_interval

	inline	bool					operator ==			( animation_interval const& left, animation_interval const& right )
	{
		return
			left.animation() == right.animation() &&
			left.start_time() == right.start_time() &&
			left.length() == right.length();
	}

	inline	bool					operator !=			( animation_interval const& left, animation_interval const& right )
	{
		return						!(left == right);
	}

	inline	bool					operator <			( animation_interval const& left, animation_interval const& right )
	{
		if ( left.animation() < right.animation() )
			return					true;

		if ( left.animation() > right.animation() )
			return					false;

		if ( left.start_time() < right.start_time() )
			return					true;

		if ( left.start_time() > right.start_time() )
			return					false;

		return						left.length() < right.length();
	}

	inline	bool					operator >			( animation_interval const& left, animation_interval const& right )
	{
		if ( left.animation() > right.animation() )
			return					true;

		if ( left.animation() < right.animation() )
			return					false;

		if ( left.start_time() > right.start_time() )
			return					true;

		if ( left.start_time() < right.start_time() )
			return					false;

		return						left.length() > right.length();
	}

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_animation_interval_inline.h>

#endif // #ifndef XRAY_ANIMTION_MIXING_ANIMATION_INTERVAL_H_INCLUDED