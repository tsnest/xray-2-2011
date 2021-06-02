////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_DNF_WORLD_STATE_H_INCLUDED
#define XRAY_AI_SEARCH_DNF_WORLD_STATE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

class dnf_world_state
{
public:
	typedef property_id_type		property_id_type;
	typedef property_value_type		property_value_type;
	typedef world_state_property	property_type;
	typedef vector< property_type >	properties_type;	

public:
	inline							dnf_world_state	( );
	
	inline	bool 					operator==		( dnf_world_state const& other ) const;

	inline	properties_type const&	properties		( ) const;
	inline	properties_type&		properties		( );
	inline	u32						hash_value		( ) const;
	inline	void					add				( property_type const& property, properties_type::iterator it_begin );

private:
	properties_type					m_properties;
	u32								m_hash;
}; // class dnf_world_state

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/dnf_world_state_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_DNF_WORLD_STATE_H_INCLUDED