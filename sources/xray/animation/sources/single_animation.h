////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SINGLE_ANIMATION_H_INCLUDED
#define SINGLE_ANIMATION_H_INCLUDED

#include <xray/animation/animation_expression_emitter.h>

namespace xray {
namespace animation {

namespace mixing {
	class animation_clip;
	class animation_lexeme;
} // namespace mixing

struct base_interpolator;

class single_animation : public animation_expression_emitter
{
public:
									single_animation	( skeleton_animation_ptr animation, base_interpolator* interpolator );
	virtual							~single_animation	( );

public:
	virtual mixing::expression		emit				( mutable_buffer& buffer, bool& is_last_animation ) const;
	virtual mixing::expression		emit				( mutable_buffer& buffer, mixing::animation_lexeme& driving_animation, bool& is_last_animation ) const;
	virtual animation_types_enum	type				( ) const;
	virtual void					serialize			( memory::writer& w ) const;
	virtual void					deserialize			( memory::reader& r );

private:
	skeleton_animation_ptr			m_animation;
	base_interpolator*				m_interpolator;
}; // class single_animation

} // namespace animation
} // namespace xray

#endif // #ifndef SINGLE_ANIMATION_H_INCLUDED