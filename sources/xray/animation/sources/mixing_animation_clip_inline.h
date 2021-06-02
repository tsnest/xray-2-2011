////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_CLIP_INLINE_H_INCLUDED
#define ANIMATION_CLIP_INLINE_H_INCLUDED

namespace xray {
namespace animation {
namespace mixing {

inline animation_clip::animation_clip						(
		pcstr const identifier,
		base_interpolator const* const interpolator,
		skeleton_animation const* animation,
		animation_type const type
	) :
	m_identifier	( identifier ),
	m_interpolator	( interpolator ),
	m_animation		( animation ),
	m_type			( type )
{
}

inline animation_clip::animation_type animation_clip::type	( ) const
{
	return			m_type;
}

inline float animation_clip::time							( ) const
{
	return			m_animation->max_time() * ( 1.f / default_fps );
}

inline base_interpolator const& animation_clip::interpolator( ) const
{
	ASSERT			( m_interpolator );
	return			*m_interpolator;
}

inline skeleton_animation const& animation_clip::animation	( ) const
{
	ASSERT			( m_animation );
	return			*m_animation;
}

inline pcstr animation_clip::identifier						( ) const
{
	ASSERT			( m_identifier );
	return			m_identifier;
}

} // namespace mixing
} // namespace animation
} // namespace xray

#endif // #ifndef ANIMATION_CLIP_INLINE_H_INCLUDED