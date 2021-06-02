////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_WORLD_STATE_PROPERTY_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_WORLD_STATE_PROPERTY_INLINE_H_INCLUDED

#include <xray/math_randoms_generator.h>

namespace xray {
namespace ai {
namespace planning {

#define WORLD_PROPERTY planning::world_state_property

inline WORLD_PROPERTY::world_state_property	( property_id_type const& id, property_value_type const& value )
{
	m_id						= id;
	m_value						= value;
	math::random32				rnd( u32(id) + 1 );
	m_hash						= rnd.random( 0xffffffff );
	rnd.seed					( m_hash + u32(value) );
	m_hash						^= rnd.random( 0xffffffff );
}

inline WORLD_PROPERTY::property_id_type const& WORLD_PROPERTY::id		( ) const
{
	return						m_id;
}

inline WORLD_PROPERTY::property_value_type const& WORLD_PROPERTY::value	( ) const
{
	return						m_value;
}

inline u32  WORLD_PROPERTY::hash_value	( ) const
{
	return						m_hash;
}

inline bool WORLD_PROPERTY::operator<	( world_state_property const& property ) const
{
	if ( id() < property.id() )
		return true;
	
	if ( id() > property.id() )
		return false;
	
	return		value() < property.value();
}

inline bool WORLD_PROPERTY::operator==	( world_state_property const& property ) const
{
	return ( id() == property.id() && value() == property.value() );
}

#undef WORLD_PROPERTY

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_WORLD_STATE_PROPERTY_INLINE_H_INCLUDED