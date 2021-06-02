////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_WORLD_STATE_H_INCLUDED
#define XRAY_AI_SEARCH_WORLD_STATE_H_INCLUDED

#include <xray/ai/search/world_state_property.h>

namespace xray {
namespace ai {
namespace planning {

class world_state
{
public:
	typedef property_id_type		property_id_type;
	typedef property_value_type		property_value_type;
	typedef world_state_property	property_type;
	typedef vector< property_type >	properties_type;

public:
	inline							world_state	( );
	
	inline	bool 					operator==	( world_state const& object ) const;
	inline	bool					operator!=	( world_state const& object ) const;
	inline	bool 					operator<	( world_state const& object ) const;
	
	inline	properties_type const&	properties	( ) const;
	inline	void					add			( property_type const& property );
	inline	void					add			( properties_type::const_iterator& iter, property_type const& property );
	inline	void					add_back	( property_type const& property );
	inline	void					remove		( property_id_type const& id );
	inline	void					clear		( );
	inline	u32						hash_value	( ) const;
	inline	void					erase		( properties_type::const_iterator const& begin, properties_type::const_iterator const& end );
	
	inline	property_type const*	property	( property_id_type const& id ) const;
	inline	property_type*			property	( property_id_type const& id );

private:
	properties_type					m_properties;
	u32								m_hash;
}; // class world_state

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/world_state_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_WORLD_STATE_H_INCLUDED