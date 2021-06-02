////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "single_animation.h"
#include <xray/animation/mixing_expression.h>
#include <xray/animation/mixing_animation_lexeme.h>
#include <xray/animation/mixing_animation_lexeme_parameters.h>
#include <xray/animation/base_interpolator.h>
#include <xray/resources_unmanaged_allocator.h>
#include <xray/animation/cubic_spline_skeleton_animation.h>

namespace xray {
namespace animation {

using xray::animation::mixing::expression;
using xray::animation::mixing::animation_lexeme;

single_animation::single_animation	( skeleton_animation_ptr animation, base_interpolator* interpolator ):
	m_animation						( animation ),
	m_interpolator					( interpolator )
{
	R_ASSERT						( m_interpolator );
}

single_animation::~single_animation ( )
{
	XRAY_DELETE_IMPL				( resources::unmanaged_allocator(), m_interpolator );
}

expression single_animation::emit	( mutable_buffer& buffer, bool& is_last_animation ) const
{
	is_last_animation				= true;
	mixing::animation_lexeme_parameters parameters (
		buffer,
		request_path(),
		m_animation
	);
	animation_lexeme				lexeme( parameters );
	return							expression( lexeme );
}

expression single_animation::emit	( mutable_buffer& buffer, animation_lexeme& driving_animation, bool& is_last_animation ) const
{
	is_last_animation				= true;
	mixing::animation_lexeme_parameters parameters (
		buffer,
		request_path(),
		m_animation,
		driving_animation
	);
	animation_lexeme				lexeme( parameters );
	return							expression( lexeme );
}

animation_types_enum single_animation::type ( ) const
{
	return							cubic_spline_skeleton_animation_pinned( m_animation ).c_ptr()->animation_type();
}

void single_animation::serialize	( memory::writer& w ) const
{
	XRAY_UNREFERENCED_PARAMETER		( w );
	NOT_IMPLEMENTED					( );
}

void single_animation::deserialize ( memory::reader& r )
{
	XRAY_UNREFERENCED_PARAMETER		( r );
	NOT_IMPLEMENTED					( );
}

} // namespace animation
} // namespace xray