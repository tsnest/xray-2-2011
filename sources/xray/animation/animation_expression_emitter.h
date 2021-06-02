////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_EXPRESSION_EMITTER_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_EXPRESSION_EMITTER_H_INCLUDED

#include <xray/animation/api.h>

namespace xray {
namespace animation {

namespace mixing {
	class expression;
	class animation_lexeme;
} // namespace mixing

class XRAY_ANIMATION_API animation_expression_emitter : public resources::unmanaged_resource
{
public:
	virtual mixing::expression		emit		( mutable_buffer& buffer, bool& is_last_animation ) const = 0;
	virtual mixing::expression		emit		( mutable_buffer& buffer, mixing::animation_lexeme& driving_animation, bool& is_last_animation ) const = 0;
	virtual void					serialize	( memory::writer& w ) const = 0;
	virtual void					deserialize	( memory::reader& r ) = 0;
	virtual animation_types_enum	type		( ) const = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR		( animation_expression_emitter );

}; // class animation_emitter

typedef resources::resource_ptr < animation_expression_emitter, resources::unmanaged_intrusive_base > animation_expression_emitter_ptr;

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_EXPRESSION_EMITTER_H_INCLUDED