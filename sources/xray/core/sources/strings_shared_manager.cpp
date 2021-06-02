////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "strings_shared_manager.h"

using xray::strings::shared::manager;
using xray::shared_string;
using xray::strings::shared::profile;

static xray::uninitialized_reference<manager>	s_manager;

void xray::strings::initialize			( )
{
	XRAY_CONSTRUCT_REFERENCE	( s_manager, manager );
}

void xray::strings::finalize			( )
{
	XRAY_DESTROY_REFERENCE		( s_manager );
}

shared_string::shared_string			( pcstr value ) :
	m_pointer					( s_manager->string ( value ) )
{
}

void xray::strings::shared::remove		( profile* profile )
{
	s_manager->remove			( profile );
}

void manager::remove					( profile* profile )
{
	m_mutex.lock				( );
	ASSERT						( !profile->reference_count() );
	Storage::iterator i			= m_storage.find( profile );
	Storage::iterator e			= m_storage.end( );
	ASSERT						( i != e );

	for ( ; (i != e) && ((*i) != profile); ++i );

	ASSERT						( i != e );
	ASSERT						((*i) == profile);

	m_storage.erase				( i );
	m_mutex.unlock				( );

	profile::destroy			( m_mutex, profile );
}

manager::~manager						( )
{
	collect_garbage				( );

	if ( m_storage.empty( ) )
		return;

	LOGI_ERROR					( "strings:shared", "leaked %d strings", m_storage.size( ) );

	Storage::const_iterator		i = m_storage.begin( );
	Storage::const_iterator		e = m_storage.end( );
	for ( ; i != e; ++i)
		LOGI_ERROR				( "strings:shared", "[%d count] %s", ( *i )->reference_count( ), ( *i )->value( ) );
}

manager::profile_ptr manager::string	(pcstr value)
{
#pragma warning(push)
#pragma warning(disable:4815)
	profile						query;
#pragma warning(pop)
	
	value						= value ? value : "";
	profile::create_temp		(value,query);

	m_mutex.lock				( );
	Storage::const_iterator		i = m_storage.find( &query );
	Storage::const_iterator		e = m_storage.end( );
	for ( ; i != e; ++i) {
		if ( ( *i )->checksum( ) != query.checksum_no_check( ) )
			break;

		if ( ( *i )->length( ) != query.length_no_check( ) )
			continue;

		if ( compare( ( *i )->value( ),value ) )
			continue;

		m_mutex.unlock			( );
		return					( *i );
	}
	m_mutex.unlock				( );

	profile_ptr					temp = profile::create( m_mutex, value, query );

	m_mutex.lock				( );
	m_storage.insert			( temp );
	m_mutex.unlock				( );

	return						( temp );
}

#pragma optimize( "", off )
void manager::check_consistency			( )
{
	m_mutex.lock				( );
	
	Storage::const_iterator		i = m_storage.begin( );
	Storage::const_iterator		e = m_storage.end( );
	for ( ; i != e; ++i) {
		( *i )->length			( );
		( *i )->checksum		( );
	}

	m_mutex.unlock				( );
}
#pragma optimize( "", on )

struct remove_predicate : private boost::noncopyable {
	xray::threading::mutex&	m_mutex;

	inline		remove_predicate		( xray::threading::mutex&	mutex ) :
		m_mutex					( mutex )
	{
	}

	inline bool operator( )				( profile* const string) const
	{
		if ( string->reference_count( ) )
			return				( false );

		profile::destroy		( m_mutex, string );
		return					( true );
	}
};

void manager::collect_garbage			( )
{
	m_mutex.lock				( );

#if 1
	m_storage.clear				( remove_predicate( m_mutex) );
#else // #if 0
	m_storage.erase				(
		std::remove_if(
			m_storage.begin( ),
			m_storage.end( ),
			remove_predicate( m_mutex )
		),
		m_storage.end( )
	);
#endif // #if 0

	m_mutex.unlock				( );
}

int	manager::compute_stats				( Storage const& storage )
{
	int							result = 0;
	result						-= sizeof( *this ) + 0;
//	result						-= sizeof( Storage::allocator_type );
	const int					node_size = 20;
	Storage::const_iterator		i = storage.cbegin( );
	Storage::const_iterator		e = storage.cend( );
	for ( ; i != e; ++i) {
		if ( !(*i)->reference_count( ) )
			continue;

		result					-= node_size;	// for node in tree
		result					-= 16;			// for profile itself
		result					+= ( ( *i )->reference_count( ) - 1)*( ( *i )->length( ) + 1 );
	}

	return						( result );
}

int	manager::compute_stats				( )
{
	m_mutex.lock				( );
	int	const result			= compute_stats( m_storage );
	m_mutex.unlock				( );

	return						( result );
}

void manager::dump						( )
{
	m_mutex.lock				( );

	LOG_INFO					( "shared string manager dump: (%d objects)", m_storage.size( ) );
	LOG_INFO					( "Refs. Length    CRC    Value");
	
	Storage::const_iterator		i = m_storage.begin( );
	Storage::const_iterator		e = m_storage.end( );
	for ( ; i != e; ++i)
		LOG_INFO				( "[%4d][%4d][%8x] \"%s\"", ( *i )->reference_count( ), ( *i )->length( ), ( *i )->checksum( ), ( *i )->value( ) );

	LOG_INFO					( "economy : %i", compute_stats( m_storage ) );

	m_mutex.unlock				( );
}