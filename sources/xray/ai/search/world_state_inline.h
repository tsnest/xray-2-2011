////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_WORLD_STATE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_WORLD_STATE_INLINE_H_INCLUDED

#include <cs/gtl/comparison/compare.h>

namespace xray {
namespace ai {

#define WORLD_STATE planning::world_state

namespace vertex_manager {

inline u32 hash_value			( WORLD_STATE const& property )
{
	return						property.hash_value();
}

} // namespace vertex_manager
	
namespace planning {

inline WORLD_STATE::world_state	( ) :
	m_hash						( 0 )
{
}

inline bool WORLD_STATE::operator==	( world_state const& object ) const
{
	if ( hash_value() != object.hash_value() )
		return					false;

	return						gtl::equal( properties(), object.properties() );
}

inline bool WORLD_STATE::operator!=	( world_state const& object ) const
{
	return						!operator==( object );
}

inline bool WORLD_STATE::operator<					( world_state const& object ) const
{
	properties_type::const_iterator iter			= properties().begin();
	properties_type::const_iterator iter_end		= properties().end();
	properties_type::const_iterator iter_object		= object.properties().begin();
	properties_type::const_iterator iter_object_end	= object.properties().end();
	
	for ( ; ( iter != iter_end ) && ( iter_object != iter_object_end ); ++iter, ++iter_object )
	{
		if ( *iter < *iter_object )
			return									true;

		if ( *iter_object < *iter )
			return									false;

		continue;
	}

	if ( iter == iter_end )
	{
		if ( iter_object == iter_object_end )
			return									false;

		return										true;
	}

	return											false;
}

inline WORLD_STATE::properties_type const& WORLD_STATE::properties	( ) const
{
	return m_properties;
}

inline void WORLD_STATE::add		( property_type const& property )
{
	properties_type::iterator iter	= std::lower_bound( m_properties.begin(), m_properties.end(), property );
	if ( ( iter != m_properties.end() ) && ( (*iter).id() == property.id() ) )
	{
		LOG_WARNING					( "you're trying to add the world state property which is already presented" );
		return;
	}

	m_properties.insert				( iter, property );
	m_hash							^= property.hash_value();
}

inline void WORLD_STATE::add		( properties_type::const_iterator& iter, property_type const& property )
{
	m_properties.insert				( m_properties.begin() + ( iter - m_properties.begin() ), property );
	m_hash							^= property.hash_value();
}

inline void WORLD_STATE::add_back	( property_type const& property )
{
	R_ASSERT						( m_properties.empty() || ( m_properties.back().id() < property.id() ), "property order is broken" );
	m_properties.push_back			( property );
	m_hash							^= property.hash_value();
}

inline void WORLD_STATE::remove		( property_id_type const& id )
{
	properties_type::iterator iter	= std::lower_bound( m_properties.begin(), m_properties.end(), property_type( id, property_value_type( 0 ) ) );
	if ( ( iter == m_properties.end() ) || ( (*iter).id() != id ) )
	{
		LOG_WARNING					( "you're trying to remove the world state property which is absent" );
		return;
	}
	m_hash							^= (*iter).hash_value();
	m_properties.erase				( iter );
}

inline void WORLD_STATE::clear		( )
{
	m_properties.clear				( );
	m_hash							= 0;
}

inline u32 WORLD_STATE::hash_value	( ) const
{
	return							m_hash;
}

inline WORLD_STATE::property_type const* WORLD_STATE::property	( property_id_type const& id ) const
{
	properties_type::const_iterator	iter	= std::lower_bound( m_properties.begin(), m_properties.end(), property_type( id, property_value_type( 0 ) ) );
	if ( iter == properties().end() )
		return								0;

	if ( (*iter).id() != id )
		return								0;

	return									&*iter;
}

inline WORLD_STATE::property_type* WORLD_STATE::property		( property_id_type const& id )
{
	properties_type::iterator iter			= std::lower_bound( m_properties.begin(), m_properties.end(), property_type( id, property_value_type( 0 ) ) );
	if ( iter == properties().end() )
		return								0;

	if ( (*iter).id() != id )
		return								0;

	return									&*iter;
}

inline void WORLD_STATE::erase	( properties_type::const_iterator const& begin, properties_type::const_iterator const& end )
{
	m_properties.erase			( m_properties.begin() + ( begin - m_properties.begin() ), m_properties.begin() + ( end - m_properties.end() ) );
}

#undef WORLD_STATE

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_WORLD_STATE_INLINE_H_INCLUDED