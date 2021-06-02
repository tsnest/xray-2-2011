////////////////////////////////////////////////////////////////////////////
//	Created 	: 02.02.2008
//	Author		: Konstantin Slipchenko
//	Description : collision element interface
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "collide_element.h"

#include <xray\collision\geometry_instance.h>
#include <xray\collision\geometry.h>
#include <xray\collision\api.h>

void	collide_element::generate_contacts( xray::collision::on_contact& c, const xray::collision::geometry_instance& gi  )const
{
	XRAY_UNREFERENCED_PARAMETERS			( &c, &gi );
//	m_geometry_instance.generate_contacts	( c, og );
}

void	collide_element::render( render::scene_ptr const& scene, render::debug::renderer& renderer ) const
{
	m_geometry_instance.render				( scene, renderer );
}

collide_element::~collide_element( )
{
	//collision::geometry *g = &(*m_geometry_instance.get_geometry());
	//collision::destroy( g );
	//collision::destroy(&m_geometry_instance);
}

math::aabb& collide_element::aabb( math::aabb &box ) const
{
	box = m_geometry_instance.get_aabb();
	return box;
}

void collide_element::check_collision_validity( )const
{
	ASSERT( m_geometry_instance.is_valid(), "is not valid geometry instance!" );
}

bool collide_element::aabb_test( math::aabb const& aabb )const
{
	return m_geometry_instance.aabb_test( aabb );
}