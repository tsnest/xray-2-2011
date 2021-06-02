////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_WORLD_STATE_PROPERTY_H_INCLUDED
#define XRAY_AI_SEARCH_WORLD_STATE_PROPERTY_H_INCLUDED

#include <xray/ai/search/base_types.h>

namespace xray {
namespace ai {
namespace planning {

class world_state_property
{
public:
	typedef property_id_type			property_id_type;
	typedef property_value_type			property_value_type;

public:
	inline								world_state_property( property_id_type const& id, property_value_type const& value );
	
	inline	property_id_type const&		id					( ) const;
	inline	property_value_type const&	value				( ) const;
	inline	u32							hash_value			( ) const;
	
	inline	bool						operator<			( world_state_property const& property ) const;
	inline	bool						operator==			( world_state_property const& property ) const;

protected:
	property_id_type					m_id;
	u32									m_hash;
	property_value_type					m_value;
}; // class world_state_property

} // namespace planning
} // namespace ai
} // namespace xray

#include <xray/ai/search/world_state_property_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_WORLD_STATE_PROPERTY_H_INCLUDED