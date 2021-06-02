////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource_chooser_external_editor.h"
#include "resource_chooser.h"

namespace xray{
namespace editor_base{

void resource_chooser_external_editor::run_editor ( property^ prop )
{
	String^ result;
	resource_chooser::choose( additional_data, safe_cast<String^>( prop->value ), "", result, true );

	prop->value = result;
}

} // namespace editor_base
} // namespace xray