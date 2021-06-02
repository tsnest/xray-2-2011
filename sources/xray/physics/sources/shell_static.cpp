////////////////////////////////////////////////////////////////////////////
//	Created 	: 16.02.2008
//	Author		: Konstantin Slipchenko
//	Description : shell static
////////////////////////////////////////////////////////////////////////////


#include "pch.h"
#include "shell_static.h"
#include "shell_element.h"
#include "static_element.h"

shell_static::shell_static( static_element& e )
:m_element	( e )
{
}

shell_static::~shell_static( )
{
}

void shell_static::collide( island& i, space_object& o, bool reverted_order   )						
{
	o.collide_detail( i, *this, !reverted_order );
}

bool shell_static::collide_detail( island& i, space_object& o, bool reverted_order )
{
	return o.collide( i, m_element, reverted_order );
	
}

bool shell_static::collide( island& i, collide_element_interface& e, bool reverted_order )
{
	return m_element.collide( i, e, reverted_order );
}

//void	shell_static::island_connect( island	&i )
//{
//	UNREACHABLE_CODE();
//}

void shell_static::render( render::scene_ptr const& scene, render::debug::renderer& renderer )const
{
	XRAY_UNREFERENCED_PARAMETERS( &scene, &renderer );
	//m_element->render			( renderer );
}

void shell_static::aabb( float3 &center, float3 &radius )const	
{
	math::aabb box(math::create_invalid_aabb());;
	center = m_element.aabb(box).center( radius );
}

bool shell_static::aabb_test( math::aabb const& aabb ) const
{
	return m_element.aabb_test( aabb );
}

void shell_static::space_check_validity( )const
{
	m_element.check_collision_validity();
}
