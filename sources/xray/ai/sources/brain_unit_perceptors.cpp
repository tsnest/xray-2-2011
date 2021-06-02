////////////////////////////////////////////////////////////////////////////
//	Created		: 09.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "brain_unit.h"
#include "enemy_perceptor.h"
#include "pickup_item_perceptor.h"

namespace xray {
namespace ai {

static perceptors::perceptor_base* create_perceptor_by_type	(
		npc& npc,
		pcstr perceptor_type,
		working_memory& memory,
		brain_unit& brain
	)
{
	if ( strings::equal( perceptor_type, "enemy" ) )
		return NEW( perceptors::enemy_perceptor )( npc, brain, memory );

	if ( strings::equal( perceptor_type, "pickup_item" ) )
		return NEW( perceptors::pickup_item_perceptor )( npc, brain, memory );
	
	return 0;
}

void brain_unit::create_perceptors	( configs::binary_config_value const& options )
{
	configs::binary_config_value const& perceptors_value			= options["perceptors"];

	configs::binary_config_value::const_iterator it					= perceptors_value.begin();
	configs::binary_config_value::const_iterator it_end				= perceptors_value.end();

	for ( ; it != it_end; ++it )
	{
		configs::binary_config_value const& perceptor_type_value	= *it;
		pcstr perceptor_type										= (pcstr)perceptor_type_value;
		if ( perceptors::perceptor_base* perceptor					=
				create_perceptor_by_type							(
					m_npc,
					perceptor_type,
					m_working_memory,
					*this
				)
			)
		{
			m_perceptors.push_back									( perceptor );
		}
		else
		{
			LOG_ERROR	( "Unknown perceptor type was declared - %s", perceptor_type );
		}
	}
}

} // namespace ai
} // namespace xray

