////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.10.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/logging_log_file.h>
#include <xray/fs_utils.h>
#include <xray/core/core.h>
#include "logging.h"

static const int log_file_group_size = 128;

xray::fs::path_string	xray::logging::g_log_file_name;

namespace xray {
namespace logging {

log_file::log_file				( xray::core::log_file_usage const log_file_usage, pcstr const file_name ) :
	m_line_groups			( LOG_NEW( line_groups_type ) ),
	m_transaction_thread_id (u32(-1))
{
	ASSERT				( file_name && *file_name );
	strings::copy			( m_file_name, file_name );

	ASSERT				( !g_log_file_name.length() );

	enum_flags<fs::open_file_enum> 
		open_file_flags	= fs::open_file_create | fs::open_file_write | fs::open_file_read;

	if ( log_file_usage == core::create_log )
		open_file_flags	|= fs::open_file_truncate;

	if ( log_file_usage == core::append_log  )
		open_file_flags	|= fs::open_file_append;

	if ( log_file_usage != core::no_log )
	{
		fs::make_dir_r		( file_name, false );

		if ( !fs::open_file ( & m_file, open_file_flags, file_name, false ) )
			m_file		= NULL;
	}

	if ( m_file )
		g_log_file_name	= file_name;

	m_line_groups->push_back (0);

	m_last_line			= 0;
	m_file_size			= 0;
	m_cache_start		= -1;
	m_cache_size		= 0;
	m_current_pos		= 0;
}

log_file::~log_file				( )
{
	if ( !m_file )
		return;

	fflush				( m_file );
	fclose				( m_file );
	m_file				= 0;

	LOG_DELETE			( m_line_groups );
}

void log_file::flush			( pcstr file_name )
{
	fflush				( m_file );

	if ( !file_name )
		return;

	int const seek_error= 
		fseek			( m_file, 0, SEEK_SET );
	ASSERT_U			( !seek_error );

	FILE * file			=	NULL;
	if ( !fs::open_file ( &file, fs::open_file_create | fs::open_file_truncate | fs::open_file_write, file_name ) )
		return;

	char				buffer[4096];
	for (;;) {
		size_t const	read = fread ( buffer, 1, sizeof( buffer ), m_file );
		if ( read == sizeof( buffer ) ) {
			fwrite		( buffer, sizeof( buffer ), 1, file );
			continue;
		}

		fwrite			( buffer, read, 1, file );
		break;
	}

	fclose				( file );
}

void log_file::append			( pcstr data, u32 const length )
{
	ASSERT				( data );
	ASSERT				( length );
	ASSERT				( m_file );
	int const seek_res	= fseek ( m_file, 0, SEEK_END );
	ASSERT_U			( !seek_res );

	u32 pos				= ftell( m_file );
	
	size_t const		num_written = fwrite(data, 1, length, m_file);
	ASSERT				( num_written == length );

	m_file_size			+= (int)num_written;

	pcstr const			last_symbol = data + length;
	while ( *data ) {
		pcstr const		next_line = strchr( data, '\n' );
		const size_t	line_length = next_line ? ( next_line-data+1 ) : ( last_symbol - data );

		pos				+= (u32)line_length;
		if ( next_line )
		{
			++m_last_line;

			u32 const line_groups_needed = (m_last_line / log_file_group_size) + 1;
		
			if ( m_line_groups->size() < line_groups_needed &&
				 !(m_last_line % log_file_group_size) ) 
				m_line_groups->push_back(pos);
		}

		data			+= line_length;
	}
}

void log_file::start_transaction	( )
{
	get_log_mutex().lock			( );
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
	get_log_mutex().unlock			( );
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
	u32 const group		= line / log_file_group_size;
	ASSERT				( group < m_line_groups->size( ) );

	m_current_pos		= (*m_line_groups)[group];

	int const seek_error = fseek ( m_file, m_current_pos, SEEK_SET );
	ASSERT_U			( !seek_error );

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
	fseek				( m_file, m_cache_start, SEEK_SET );

	m_cache_size		= (int)fread( m_cache, 1, cache_size, m_file );
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
