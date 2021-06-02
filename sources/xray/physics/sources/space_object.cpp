////////////////////////////////////////////////////////////////////////////
//	Created 	: 02.02.2008
//	Author		: Konstantin Slipchenko
//	Description : space_object
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "space_object.h"
#include "space_item.h"

void space_object::space_insert( xray::collision::space_partitioning_tree & space )
{
	ASSERT( !get_space_item() );

	space_check_validity();

	m_space_item = XRAY_NEW_IMPL( xray::physics::g_ph_allocator, xray::physics::space_item)(*this);
	m_space_item->space_insert	( space );
}

void	space_object::space_remove( )
{
	ASSERT( get_space_item() );
	m_space_item->space_remove();
}

space_object::~space_object( )
{
	XRAY_DELETE_IMPL	( xray::physics::g_ph_allocator, m_space_item );
}

void space_object::move( )
{
	ASSERT( get_space_item() );
	get_space_item()->on_move( );
}