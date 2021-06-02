////////////////////////////////////////////////////////////////////////////
//	Created		: 01.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/threading_reader_writer_lock.h>

namespace xray {
namespace threading {

reader_writer_lock::~reader_writer_lock ()
{ 
	R_ASSERT_CMP							(m_readers_writers_counter.writer_thread_id, ==, 0); 
	R_ASSERT_CMP							(m_readers_writers_counter.writers_count, ==, 0); 
	R_ASSERT_CMP							(m_readers_writers_counter.readers_count, ==, 0); 
}

bool   reader_writer_lock::lock_read_impl	(bool try_lock) const
{
	while ( identity(true) )
	{
		counters_type const previous_counters	=	m_readers_writers_counter;
		if ( previous_counters.writers_count != 0 )
		{
			if ( try_lock )
				return						false;
			threading::yield				();
			continue;
		}

		counters_type new_counters		=	previous_counters;
		++new_counters.readers_count;
		
		if ( threading::interlocked_compare_exchange(m_readers_writers_counter.whole, 
													 new_counters.whole,
													 previous_counters.whole) != previous_counters.whole )
		{
			if ( try_lock )
				return						false;
			threading::yield				();
			continue;
		} 

		return								true;
	}

	NODEFAULT								(return false);
}

bool   reader_writer_lock::lock_write_impl	(bool try_lock) const
{
	while ( identity(true) )
	{
		counters_type const previous_counters	=	m_readers_writers_counter;

		if ( previous_counters.readers_count != 0 ||
			 (previous_counters.writer_thread_id != threading::current_thread_id() &&
			  previous_counters.writer_thread_id != 0) )
		{
			if ( try_lock )
				return						false;
			threading::yield				();
			continue;
		}

		counters_type	new_counter		=	previous_counters;
		new_counter.writer_thread_id	=	threading::current_thread_id();
		++new_counter.writers_count;
		
		if ( threading::interlocked_compare_exchange(m_readers_writers_counter.whole, 
													 new_counter.whole,
													 previous_counters.whole) != previous_counters.whole )
		{
			if ( try_lock )
				return						false;
			threading::yield				();
			continue;
		} 

		return								true;
	}

	NODEFAULT								(return false);
}
	
void   reader_writer_lock::unlock_read	() const
{
	R_ASSERT								(m_readers_writers_counter.readers_count != 0);
	R_ASSERT								(m_readers_writers_counter.writers_count == 0);
	while ( identity(true) )
	{
		counters_type const previous_counters	=	m_readers_writers_counter;

		counters_type	new_counter		=	previous_counters;
		--new_counter.readers_count;
		
		if ( threading::interlocked_compare_exchange(m_readers_writers_counter.whole, 
													 new_counter.whole,
													 previous_counters.whole) != previous_counters.whole )
		{
			threading::yield				();
			continue;
		} 

		return;
	}
}

void   reader_writer_lock::unlock_write () const 
{
	R_ASSERT								(m_readers_writers_counter.writers_count != 0);
	R_ASSERT								(m_readers_writers_counter.writer_thread_id == threading::current_thread_id());
	
	while ( identity(true) )
	{
		counters_type const previous_counters	=	m_readers_writers_counter;

		counters_type	new_counter		=	previous_counters;
		if ( previous_counters.writers_count == 1 )
			new_counter.writer_thread_id	=	0;

		--new_counter.writers_count;
		
		if ( threading::interlocked_compare_exchange(m_readers_writers_counter.whole, 
													 new_counter.whole,
													 previous_counters.whole) != previous_counters.whole )
		{
			threading::yield				();
			continue;
		} 

		return;
	}

	NODEFAULT								();
}

bool   reader_writer_lock::try_lock		(lock_type_enum	lock_type) const 
{ 
	return									(lock_type == lock_type_read) ? try_lock_read() : try_lock_write(); 
}

void   reader_writer_lock::lock			(lock_type_enum	lock_type) const
{ 
	if ( lock_type == lock_type_read ) 
		lock_read							(); 
	else 
		lock_write							(); 
}
void   reader_writer_lock::unlock		(lock_type_enum lock_type) const
{ 
	if ( lock_type == lock_type_read )
		unlock_read							();
	else 
		unlock_write						(); 
}

} // namespace threading
} // namespace xray
