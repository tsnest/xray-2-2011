////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_LEXEME_H_INCLUDED
#define ANIMATION_LEXEME_H_INCLUDED

#include "mixing_binary_tree_animation_node.h"
#include "mixing_base_lexeme.h"

namespace xray {
namespace animation {
namespace mixing {

class animation_lexeme;

typedef intrusive_ptr<
			animation_lexeme,
			binary_tree_base_node,
			threading::single_threading_policy
> animation_lexeme_ptr;

class animation_lexeme :
	public binary_tree_animation_node,
	public base_lexeme
{
public:
	inline					animation_lexeme	(
								mutable_buffer& buffer,
								animation_clip* const animation,
								base_interpolator const* const time_scale_interpolator,
								float const time_scale = 1.f,
								float const animation_time_offset = 0.f,
								clip_mix_self const mix_self = clip_mix_continue_self
							);
	inline					animation_lexeme	(
								mutable_buffer& buffer,
								animation_clip* const animation,
								animation_lexeme& animation_to_synchronize_with
							);
	inline					animation_lexeme	( animation_lexeme& other, bool );
	inline	animation_lexeme* cloned_in_buffer	( );

private:
	animation_lexeme_ptr	m_cloned_instance;
}; // class animation_lexeme

} // namespace mixing
} // namespace animation
} // namespace xray

#include "mixing_animation_lexeme_inline.h"

#endif // #ifndef ANIMATION_LEXEME_H_INCLUDED