////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "node_lock.h"

namespace xray {
namespace vfs {

void   vfs_reader_writer_lock::counters_type::change_unsafe (int change, lock_type_enum lock_type)
{
	if ( change == -1 )
	{
		if ( lock_type == lock_type_read )
		{
			R_ASSERT						(readers_count > 0);
			--readers_count;
		}
		else if ( lock_type == lock_type_soft_read )
		{
			R_ASSERT						(soft_readers_count > 0);
			--soft_readers_count;
		}
		else if ( lock_type == lock_type_write )
		{
			R_ASSERT						(writers_count > 0);
			--writers_count;
		}
		else if ( lock_type == lock_type_soft_write )
		{
			R_ASSERT						(soft_writers_count > 0);
			--soft_writers_count;
		}
		else
			NODEFAULT						(return);
	}
	else if ( change == +1 )
	{
		if ( lock_type == lock_type_read )
		{
			R_ASSERT						(readers_count < max_readers_count);
			++readers_count;
		}
		else if ( lock_type == lock_type_soft_read )
		{
			R_ASSERT						(soft_readers_count < max_soft_readers_count);
			++soft_readers_count;
		}
		else if ( lock_type == lock_type_write )
		{
			R_ASSERT						(writers_count < max_writers_count);
			++writers_count;
		}
		else if ( lock_type == lock_type_soft_write )
		{
			R_ASSERT						(soft_writers_count < max_soft_writers_count);
			++soft_writers_count;
		}
		else
			NODEFAULT						(return);
	}
	else
		NODEFAULT							(return);	
}

vfs_reader_writer_lock::~vfs_reader_writer_lock ()
{ 
	R_ASSERT_CMP							(m_counters.writers_count, ==, 0); 
	R_ASSERT_CMP							(m_counters.readers_count, ==, 0); 
}

bool   vfs_reader_writer_lock::try_lock		(lock_type_enum	lock_type) const 
{ 
	if ( lock_type == lock_type_read )
		return								try_lock_read();
	else if ( lock_type == lock_type_soft_read )
		return								try_lock_soft_read(); 
	else if ( lock_type == lock_type_write )
		return								try_lock_write(); 
	else if ( lock_type == lock_type_soft_write )
		return								try_lock_soft_write(); 
	else
		NODEFAULT							(return false);
}

void   vfs_reader_writer_lock::unlock		(lock_type_enum lock_type) const
{ 
	counters_type dec;
	dec.whole							=	0;
	dec.change_unsafe						(+1, lock_type);

	R_ASSERT								(m_counters.readers_count >= dec.readers_count);
	R_ASSERT								(m_counters.writers_count >= dec.writers_count);
	R_ASSERT								(m_counters.soft_readers_count >= dec.soft_readers_count);
	R_ASSERT								(m_counters.soft_writers_count >= dec.soft_writers_count);

	threading::interlocked_exchange_add		(m_counters.whole, - dec.whole);
}

bool   vfs_reader_writer_lock::lock		(lock_type_enum lock_type, lock_operation_enum operation) const
{
	while ( identity(true) )
	{
		counters_type const previous_counters	=	m_counters;
		
		bool check						=	false;
		if ( lock_type == lock_type_read )
			check						=	!previous_counters.writers_count && !previous_counters.soft_writers_count;
		else if ( lock_type == lock_type_soft_read )
			check						=	!previous_counters.writers_count;
		else if ( lock_type == lock_type_write )
			check						=	!previous_counters.whole;
		else if ( lock_type == lock_type_soft_write )
			check						=	!previous_counters.writers_count && !previous_counters.readers_count;

		if ( !check )
		{
			if ( operation == lock_operation_try_lock )
				return						false;
			threading::yield				();
			continue;
		}

		counters_type new_counters		=	previous_counters;
		new_counters.change_unsafe			(+1, lock_type);
	
		if ( threading::interlocked_compare_exchange(m_counters.whole, 
													 new_counters.whole,
													 previous_counters.whole) != previous_counters.whole )
		{
			if ( operation == lock_operation_try_lock )
				return						false;
			threading::yield				();
			continue;
		} 

		return								true;
	}

	NODEFAULT								(return false);
}

bool   vfs_reader_writer_lock::upgrade	(lock_type_enum from_lock, lock_type_enum to_lock, lock_operation_enum operation) const
{
	counters_type allowed;
	allowed.whole						=	0;
	allowed.change_unsafe					(+1, from_lock);
	if ( from_lock == lock_type_read )
		R_ASSERT							(m_counters.readers_count != 0);
	else if ( from_lock == lock_type_soft_read )
		R_ASSERT							(m_counters.soft_readers_count != 0);
	else if ( from_lock == lock_type_write )
		R_ASSERT							(m_counters.writers_count != 0);
	else if ( from_lock == lock_type_soft_write )
		R_ASSERT							(m_counters.soft_writers_count != 0);

	while ( identity(true) )
	{
		counters_type const previous_counters	=	m_counters;
		
		bool check						=	false;
		if ( to_lock == lock_type_read )
		{
			check						=	previous_counters.writers_count <= allowed.writers_count && 
											previous_counters.soft_writers_count <= allowed.soft_writers_count;
		}
		else if ( to_lock == lock_type_soft_read )
		{
			check						=	previous_counters.writers_count <= allowed.writers_count;
		}
		else if ( to_lock == lock_type_write )
		{
			check						=	previous_counters.writers_count <= allowed.writers_count && 
											previous_counters.soft_writers_count <= allowed.soft_writers_count &&
											previous_counters.readers_count <= allowed.readers_count && 
											previous_counters.soft_readers_count <= allowed.soft_readers_count;
		}
		else if ( to_lock == lock_type_soft_write )
		{
			check						=	previous_counters.writers_count <= allowed.writers_count && 
											previous_counters.readers_count <= allowed.readers_count;
		}

		if ( !check )
		{
			if ( operation == lock_operation_try_lock )
				return						false;
			threading::yield				();
			continue;
		}

		counters_type new_counters		=	previous_counters;
		new_counters.change_unsafe			(-1, from_lock);
		new_counters.change_unsafe			(+1, to_lock);
	
		if ( threading::interlocked_compare_exchange(m_counters.whole, 
													 new_counters.whole,
													 previous_counters.whole) != previous_counters.whole )
		{
			if ( operation == lock_operation_try_lock )
				return						false;
			threading::yield				();
			continue;
		} 

		return								true;
	}

	NODEFAULT								(return false);
}

bool   vfs_reader_writer_lock::has_lock	(lock_type_enum lock_type) const
{
	if ( lock_type == lock_type_read )
		return								m_counters.readers_count != 0;
	else if ( lock_type == lock_type_soft_read )
		return								m_counters.soft_readers_count != 0;
	else if ( lock_type == lock_type_write )
		return								m_counters.writers_count != 0;
	else if ( lock_type == lock_type_soft_write )
		return								m_counters.soft_writers_count != 0;
	else
		NODEFAULT							(return false);
}

threading::lock_type_enum	to_threading_lock_type		(lock_type_enum lock_type)
{
	if ( lock_type == lock_type_read )
		return								threading::lock_type_read;
	else if ( lock_type == lock_type_write )
		return								threading::lock_type_write;
	else
		NODEFAULT							(return threading::lock_type_uninitialized);
}

} // namespace vfs
} // namespace xray