////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_CLIP_H_INCLUDED
#define ANIMATION_CLIP_H_INCLUDED

#include "skeleton_animation.h"

namespace xray {
namespace animation {

struct base_interpolator;
class skeleton_animation;

namespace mixing {

class animation_clip : private boost::noncopyable {
public:
	enum animation_type {
		full,
		additive,
	}; // enum animation_type

public:
	inline							animation_clip	(
										pcstr const identifier,
										base_interpolator const* const interpolator,
										skeleton_animation const* animation,
										animation_type const type
									);
	inline	animation_type			type			( ) const;
	inline	float					time			( ) const;
	inline	base_interpolator const& interpolator	( ) const;
	inline	skeleton_animation const& animation		( ) const;
	inline	pcstr					identifier		( ) const;
			bool					operator ==		( animation_clip const& other ) const;
	inline	bool					operator !=		( animation_clip const& other ) const { return !(*this == other); }

private:
	pcstr const						m_identifier;
	base_interpolator const* const	m_interpolator;
	skeleton_animation const* const m_animation;
	animation_type const			m_type;
}; // class animation_clip

			bool					operator <		( animation_clip const& left, animation_clip const& right );

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_animation_clip_inline.h"

#endif // #ifndef ANIMATION_CLIP_H_INCLUDED