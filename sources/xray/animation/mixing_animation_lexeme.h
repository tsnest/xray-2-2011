////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_H_INCLUDED
#define XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_H_INCLUDED

#include <xray/animation/mixing_binary_tree_animation_node.h>
#include <xray/animation/mixing_base_lexeme.h>

namespace xray {
namespace animation {
namespace mixing {

class animation_lexeme;

typedef intrusive_ptr<
	animation_lexeme,
	binary_tree_base_node,
	threading::single_threading_policy
> animation_lexeme_ptr;

} // namespace mixing
} // namespace animation

#pragma warning( push )
#pragma warning( disable : 4231 )
template class XRAY_ANIMATION_API
	intrusive_ptr<
		animation::mixing::animation_lexeme,
		animation::mixing::binary_tree_base_node,
		threading::single_threading_policy
	>;
#pragma warning( pop )

namespace animation {
namespace mixing {

class XRAY_ANIMATION_API animation_lexeme :
	public binary_tree_animation_node,
	public base_lexeme
{
public:
			explicit			animation_lexeme	( animation_lexeme_parameters const& parameters );
	inline						animation_lexeme	( animation_lexeme& other, bool );
	inline						~animation_lexeme	( );
	inline	animation_lexeme*	cloned_in_buffer	( );

	using base_lexeme::buffer;

private:
	animation_lexeme_ptr	m_cloned_instance;
}; // class animation_lexeme

} // namespace mixing
} // namespace animation
} // namespace xray

#include <xray/animation/mixing_animation_lexeme_inline.h>

#endif // #ifndef XRAY_ANIMATION_MIXING_ANIMATION_LEXEME_H_INCLUDED