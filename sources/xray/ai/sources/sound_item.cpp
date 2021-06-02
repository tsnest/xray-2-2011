////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/sound_item.h>

namespace xray {
namespace ai {

sound_item::sound_item	( resources::unmanaged_resource_ptr const& new_emitter, pcstr filename ) :
	sound				( new_emitter ),
	name				( filename )
{
}

} // namespace ai
} // namespace xray