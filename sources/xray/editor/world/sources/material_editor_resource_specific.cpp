////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_editor.h"
#include "material.h"
#include "material_instance.h"

namespace xray{
namespace editor{

resource_editor_resource^ material_editor::create_resource( )
{
	resource_editor_resource^ result = nullptr;
	if(m_mode==material_editor_mode::edit_material)
	{
		result			= gcnew material(m_editor_world);
	}else
	if(m_mode==material_editor_mode::edit_instances)
	{
		result	= gcnew material_instance(m_editor_world);
	}else
		UNREACHABLE_CODE();

	result->subscribe_on_loaded	( gcnew resource_load_delegate(this, &material_editor::on_resource_loaded));
	result->subscribe_on_changed	( gcnew resource_changed_delegate(this, &material_editor::on_resource_changed));

	return result;
}


} // namespace editor
} // namespace xray

