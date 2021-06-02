////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "containment_double_dispatcher.h"
#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>

namespace xray{
namespace collision	{

containment_double_dispatcher::containment_double_dispatcher(
				geometry_instance const& bounding_volume,
				geometry_instance const& testee
			):
				m_bounding_volume	( bounding_volume ),
				m_testee			( testee ),
				m_result			( false )
#ifndef MASTER_GOLD
				,
				m_renderer			( NULL ),
				m_scene				( NULL ) 
#endif
				{ }

#ifndef MASTER_GOLD
containment_double_dispatcher::containment_double_dispatcher(
				geometry_instance const& bounding_volume,
				geometry_instance const& testee,
				render::debug::renderer* renderer,
				render::scene_ptr const* scene
			):
				m_bounding_volume	( bounding_volume ),
				m_testee			( testee ),
				m_result			( false ),
				m_renderer			( renderer ),
				m_scene				( scene ) { }
#endif

bool containment_double_dispatcher::check_contains ( )
{
	//geometry const* bounding_volume	= &*m_bounding_volume.get_geometry( );
	//geometry const* testee			= &*m_testee.get_geometry( );

	m_bounding_volume.accept( *this, m_testee );

	return m_result;
}

} // namespace collision
} // namespace xray
