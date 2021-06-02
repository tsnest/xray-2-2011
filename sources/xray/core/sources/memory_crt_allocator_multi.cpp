////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.09.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/memory_crt_allocator.h>

using xray::memory::crt_allocator;

crt_allocator::crt_allocator		( ) :
	m_malloc_ptr				( 0 ),
	m_free_ptr					( 0 )
{
}

size_t crt_allocator::total_size	( ) const
{
	return						( 0 );
}

size_t crt_allocator::allocated_size( ) const
{
	return						( 0 );
}