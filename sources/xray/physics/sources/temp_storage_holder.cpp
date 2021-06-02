////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.01.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
//	all vectors that clear after every island step
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "temp_storage_holder.h"
#include "contact_storage.h"

temp_storage_holder::temp_storage_holder( )
:m_contacts					( XRAY_NEW_IMPL( xray::physics::g_ph_allocator, contact_storage ) ),
m_space_collision_results	( XRAY_NEW_IMPL( xray::physics::g_ph_allocator, xray::collision::objects_type )( xray::physics::g_ph_allocator ) ),
m_island_objects_pointers	( xray::physics::g_ph_allocator ),
m_joints					( xray::physics::g_ph_allocator ),
m_bodies					( xray::physics::g_ph_allocator )
{
}

temp_storage_holder::~temp_storage_holder()
{
	XRAY_DELETE_IMPL( xray::physics::g_ph_allocator, m_contacts );
	XRAY_DELETE_IMPL( xray::physics::g_ph_allocator, m_space_collision_results );
}

