////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.10.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/logging/log_file.h>
#include <xray/fs/device_utils.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/core/core.h>
#include "logging.h"
#include "globals.h"

static const int log_file_group_size = 256;

xray::fs_new::native_path_string	xray::logging::g_log_file_name;

namespace xray {
namespace logging {

using namespace fs_new;

log_file::log_file				( log_file_usage			log_file_usage, 
								  pcstr						file_name,
								  device_file_system_proxy	device ) 
	:
	m_transaction_thread_id (u32(-1)),
	m_file					( 0 ),
	m_device				( device )
{
	ASSERT					( file_name && *file_name );
	m_file_name			=	file_name;

	ASSERT					( !g_log_file_name.length() );

	file_mode::mode_enum mode	=	( log_file_usage == logging::create_log ) ? 
										file_mode::create_always : file_mode::append_or_create;

	if ( log_file_usage != logging::no_log )
	{
		create_folder_r	(m_device, file_name, false);

		file_type *	file				=	(file_type *)& m_file_pointer_storage;
		open_file_params	params			(mode, file_access::read_write, assert_on_fail_false);
		params.file_type_allocated_by_user	=	true;

		if ( m_device->open(& file, file_name, params) )
			threading::interlocked_exchange_pointer	( m_file, file );
	}

	if ( m_file )
		g_log_file_name					=	file_name;

	XRAY_CONSTRUCT_REFERENCE				(m_line_groups, line_groups_type);
	m_line_groups->push_back (0);

	m_last_line			= 0;
	m_file_size			= 0;
	m_cache_start		= -1;
	m_cache_size		= 0;
	m_current_pos		= 0;
}

log_file::~log_file				( )
{
	close				( );
	XRAY_DESTROY_REFERENCE					(m_line_groups);
}

void log_file::flush			( pcstr in_file_name )
{
	if ( !m_file )
		return;

	m_device->flush		( m_file );

	if ( !in_file_name )
		return;

	bool const success	= m_device->seek( m_file, 0, seek_file_begin );
	ASSERT_U			( success );

	native_path_string	file_name		=	native_path_string::convert(in_file_name);
	fs_new::create_folder_r					( m_device, file_name, false );

	file_type_pointer file				(file_name, m_device, file_mode::create_always, 
										 file_access::write, assert_on_fail_false, notify_watcher_false);

	if ( !file )
		return;

	char				buffer[4096];
	for (;;) {
		file_size_type const	read = m_device->read( m_file, buffer, sizeof( buffer ) );
		if ( read == sizeof( buffer ) ) {
			m_device->write				( file, buffer, sizeof( buffer ) );
			continue;
		}

		m_device->write			( file, buffer, read );
		break;
	}
}

void log_file::append			( pcstr data, u32 const length )
{
	ASSERT				( data );
	ASSERT				( length );
	ASSERT				( m_file );
	bool const seek_res	= m_device->seek(m_file, 0, seek_file_end);
	ASSERT_U			( seek_res );

	file_size_type pos	= m_device->tell( m_file );
	
	file_size_type const	num_written = m_device->write(m_file, data, length);
	ASSERT				( num_written == length );

	m_file_size			+= (int)num_written;

	pcstr const			last_symbol = data + length;
	while ( *data ) 
	{
		pcstr const		next_line = strchr( data, '\n' );
		size_t const 	line_length = next_line ? ( next_line-data+1 ) : ( last_symbol - data );

		pos				+= (u32)line_length;
		if ( next_line )
		{
			++m_last_line;

			u32 const line_groups_needed = (m_last_line / log_file_group_size) + 1;
		
			if ( m_line_groups->size() < line_groups_needed &&
				 m_line_groups->size() < m_line_groups->max_size() &&
				 !(m_last_line % log_file_group_size) ) 
			{
				m_line_groups->push_back((int)pos);
			}
		}

		data			+= line_length;
	}
}

void log_file::start_transaction	( )
{
	globals->log_mutex.lock			( );
	R_ASSERT						( m_transaction_thread_id == u32(-1),
									"transaction was not ended or mutex was unlocked by "
									"someone other then end_transaction");
	m_transaction_thread_id		=	threading::current_thread_id();
}

void log_file::assert_transaction_in_current_thread	( ) const
{
	R_ASSERT						( m_transaction_thread_id != u32(-1), 
									 "you must call start_transaction first");
	R_ASSERT						( m_transaction_thread_id == threading::current_thread_id(),
									 "transaction was started in another thread");
}

void log_file::end_transaction		( )
{
	assert_transaction_in_current_thread	( );
	m_transaction_thread_id		=	u32(-1);
	globals->log_mutex.unlock			( );
}

u32	log_file::get_lines_count		( ) const
{ 
	assert_transaction_in_current_thread	( );
	return							m_last_line; 
}

void log_file::goto_line		( u32 const line )
{
	assert_transaction_in_current_thread	( );
	ASSERT				(line  <= m_last_line);
	u32 group			= line / log_file_group_size;
	if ( group >= m_line_groups->size() )
		group			= m_line_groups->size() - 1;

	m_current_pos		= (* m_line_groups)[group];

	bool const seek_result = m_device->seek( m_file, m_current_pos, seek_file_begin );
	ASSERT_U			( seek_result );

	u32 num2skip		= line - log_file_group_size*group;

	for ( ; num2skip; --num2skip )
		skip_next_line	( );
}

char log_file::read_next_char	( )
{
	int const cache_offs= m_current_pos - m_cache_start;

	++m_current_pos;

	if ( ( m_cache_start != -1 ) && ( cache_offs >= 0 ) && ( cache_offs < m_cache_size ) )
		return			( m_cache[cache_offs] );

	m_cache_start		= m_current_pos - 1;
	m_device->seek		( m_file, m_cache_start, seek_file_begin );

	m_cache_size		= (int)m_device->read( m_file, m_cache, cache_size );
	ASSERT				(m_cache_size != 0);
	return				( m_cache[0] );
}

template <typename processor_type>
bool log_file::process_next_line ( u32 const buffer_size, processor_type const& processor )
{
	if ( m_current_pos == m_file_size )
	{
		return			false;
	}

	int const last_pos	= math::min(m_file_size, m_current_pos+buffer_size-1);
	char current_char	= 0;

	while ( (int)m_current_pos < last_pos ) {
		current_char	= read_next_char();
		processor		 (current_char);

		if ( current_char == '\n' )
			break;
	}

	processor			( 0 );

	// if not on eol, read till eol or eof
	while ( ( current_char != '\n' ) && ( m_current_pos != m_file_size ) )
		current_char	= read_next_char( );

	return				true;
}

struct processor {
	mutable pstr buffer_ptr;
	
	inline	processor		( pstr buffer ) : buffer_ptr(buffer) {}
	
	inline void operator( ) ( char const character ) const
	{
		*buffer_ptr = character;
		++buffer_ptr;
	}
};

bool log_file::read_next_line	(pstr const buffer, const u32 buffer_size)
{
	assert_transaction_in_current_thread	( );

	return				( process_next_line( buffer_size, processor( buffer ) ) );
}

bool log_file::skip_next_line	( )
{
	struct processor {
		static void dummy (char const character) { XRAY_UNREFERENCED_PARAMETER(character); }
	};

	return				( process_next_line ( 0, &processor::dummy ) );
}

} // namespace logging
} // namespace xray

#include <xray/os_include.h>	// for MemoryBarrier

namespace xray {
namespace logging {

void log_file::close		( )
{
	if ( !m_file )
		return;
	
	file_type* file	= m_file;

	threading::interlocked_exchange_pointer( m_file, (file_type*)0 );
	XRAY_MEMORY_BARRIER_FULL();

	m_device->flush		( file );
	m_device->close		( file );
}

void log_file::on_terminate			( )
{
	close			( );
}

} // namespace logging
} // namespace xray