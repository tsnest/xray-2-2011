////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_DNF_WORLD_STATE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_DNF_WORLD_STATE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline dnf_world_state::dnf_world_state	( ) :
	m_hash								( 0 )
{
}

inline dnf_world_state::properties_type const& dnf_world_state::properties	( ) const
{
	return			m_properties;
}

inline dnf_world_state::properties_type& dnf_world_state::properties		( )
{
	return			m_properties;
}

inline u32 dnf_world_state::hash_value	( ) const
{
	return			m_hash;
}

inline bool dnf_world_state::operator==	( dnf_world_state const& other ) const
{
	if ( hash_value() != other.hash_value() )
		return		false;

	return			gtl::equal( properties(), other.properties() );
}

inline void dnf_world_state::add		( property_type const& property, properties_type::iterator it_begin )
{
	properties_type::iterator iter		= std::lower_bound( it_begin, m_properties.end(), property );
	if ( ( iter != m_properties.end() ) && ( (*iter).id() == property.id() ) )
	{
		LOG_WARNING						( "you're trying to add the world state property which is already presented" );
		return;
	}

	m_properties.insert					( iter, property );
	m_hash								^= property.hash_value();
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_DNF_WORLD_STATE_INLINE_H_INCLUDED