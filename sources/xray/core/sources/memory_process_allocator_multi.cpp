////////////////////////////////////////////////////////////////////////////
//	Created 	: 30.01.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "memory_process_allocator.h"

using xray::memory::process_allocator;

size_t process_allocator::total_size	( ) const
{
	return						( 0 );
}

size_t process_allocator::allocated_size( ) const
{
	return						( 0 );
}