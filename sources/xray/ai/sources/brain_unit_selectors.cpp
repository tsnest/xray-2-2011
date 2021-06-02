////////////////////////////////////////////////////////////////////////////
//	Created		: 09.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "brain_unit.h"
#include "enemy_target_selector.h"
#include "pickup_item_target_selector.h"
#include "threat_target_selector.h"
#include "disturbance_target_selector.h"
#include "weapon_target_selector.h"
#include "animation_target_selector.h"
#include "sound_target_selector.h"
#include "position_target_selector.h"

namespace xray {
namespace ai {

static selectors::target_selector_base* create_selector_by_type	(
		ai_world& world,
		pcstr selector_type,
		working_memory const& memory,
		blackboard& board,
		brain_unit& brain
	)
{
	if ( strings::equal( selector_type, "enemy" ) )
		return NEW( selectors::enemy_target_selector )( world, memory, board, selector_type );

	if ( strings::equal( selector_type, "weapon" ) )
		return NEW( selectors::weapon_target_selector )( world, memory, board, brain, selector_type );

	if ( strings::equal( selector_type, "animation" ) )
		return NEW( selectors::animation_target_selector )( world, memory, board, brain, selector_type );

	if ( strings::equal( selector_type, "sound" ) )
		return NEW( selectors::sound_target_selector )( world, memory, board, brain, selector_type );

	if ( strings::equal( selector_type, "position" ) )
		return NEW( selectors::position_target_selector )( world, memory, board, brain, selector_type );

	if ( strings::equal( selector_type, "threat" ) )
		return NEW( selectors::threat_target_selector )( world, memory, board, selector_type );

	if ( strings::equal( selector_type, "pickup_item" ) )
		return NEW( selectors::pickup_item_target_selector )( world, memory, board, selector_type );

	if ( strings::equal( selector_type, "disturbance" ) )
		return NEW( selectors::disturbance_target_selector )( world, memory, board, selector_type );
	
	return 0;
}

void brain_unit::create_selectors	( configs::binary_config_value const& options )
{
	configs::binary_config_value const& selectors_value				= options["target_selectors"];

	configs::binary_config_value::const_iterator it					= selectors_value.begin();
	configs::binary_config_value::const_iterator it_end				= selectors_value.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& selector_type_value		= *it;
		pcstr selector_type											= (pcstr)selector_type_value;
		if ( selectors::target_selector_base* selector				=
				create_selector_by_type								(
					m_world,
					selector_type,
					m_working_memory,
					m_blackboard,
					*this
				)
			)
		{
			m_target_selectors.push_back							( selector );
		}
		else
			LOG_ERROR	( "Unknown target selector type was declared - %s", selector_type );
	}
}

} // namespace ai
} // namespace xray