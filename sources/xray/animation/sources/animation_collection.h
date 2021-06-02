////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_COLLECTION_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_COLLECTION_H_INCLUDED

#include <xray/animation/animation_expression_emitter.h>
#include <xray/math_randoms_generator.h>

namespace xray {
namespace animation {

namespace mixing {
	class animation_lexeme;
} // namespace mixing

enum collection_playing_types
{
	collection_playing_type_random,
	collection_playing_type_sequential
};

class animation_collection : public animation_expression_emitter
{
public:
									animation_collection	(
										collection_playing_types type,
										bool can_repeat_successively,
										bool cyclic_repeating_index,
										pvoid buffer,
										size_t max_count,
										u32 last_time
									);
	virtual							~animation_collection	( );

public:
	virtual mixing::expression		emit					( mutable_buffer& buffer, bool& is_last_animation ) const;
	virtual mixing::expression		emit					( mutable_buffer& buffer, mixing::animation_lexeme& driving_animation, bool& is_last_animation ) const;
	virtual animation_types_enum	type					( ) const;
	virtual void					serialize				( memory::writer& w ) const;
	virtual void					deserialize				( memory::reader& r );

			void					add_animation			( animation_expression_emitter_ptr emitter );
		
private:
			mixing::expression		emit_impl				( mutable_buffer& buffer, mixing::animation_lexeme* const driving_animation, bool& is_last_animation ) const;

private:
	typedef buffer_vector< animation_expression_emitter_ptr > animations_type;

private:
	animations_type					m_animations;
	mutable math::random32			m_random_number;
	mutable u32						m_current_animation_index;
	collection_playing_types		m_type;
	bool							m_is_cyclic_repeating;
	bool							m_can_repeat_successively;
	mutable bool					m_is_child_last_animation;
}; // class animation_collection

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_COLLECTION_H_INCLUDED