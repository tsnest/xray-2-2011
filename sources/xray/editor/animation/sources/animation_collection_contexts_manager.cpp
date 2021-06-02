////////////////////////////////////////////////////////////////////////////
//	Created		: 12.10.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_collection_contexts_manager.h"
#include "animation_collections_editor.h"

namespace xray {
namespace animation_editor {

bool animation_collection_contexts_manager::context_fit ( String^ context )
{
	IDockContent^ active_content = m_editor->multidocument_base->main_dock_panel->ActiveContent;

	if( context == "Global" || context == "animation_collections_editor" )
		return true;

	else if( context == "active_document" )
		return active_content == m_editor->multidocument_base->active_document;

	return false;
}

List<String^>^ animation_collection_contexts_manager::get_contexts_list ( )
{
	return gcnew List<String^>( );
}

} // namespace animation_editor
} // namespace xray
