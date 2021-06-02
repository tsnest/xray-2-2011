////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_select.h"
#include "level_editor.h"
#include "project.h"
#include "object_base.h"


namespace xray {
namespace editor {

command_select::command_select( project^ p )
:m_project			( p ),
m_selection_method	(enum_selection_method_set),
m_item_ids			(gcnew item_id_list)
{
}

command_select::command_select( project^ p, project_items_list^ items, enum_selection_method method )
:m_project			( p ),
m_selection_method	(method),
m_item_ids			(gcnew item_id_list)
{
	for each (project_item_base^ item in items)
		m_item_ids->Add(item->id());

	R_ASSERT(m_item_ids->Count>0);
}

command_select::command_select( project^ p, project_item_base^ item, enum_selection_method method )
:m_project			( p ),
m_selection_method	(method),
m_item_ids			(gcnew item_id_list)
{
	m_item_ids->Add( item->id() );
	if(m_item_ids->Count==0)
		LOG_INFO("sel empty");
}

command_select::command_select( project^ p, item_id_list^ objects, enum_selection_method method )
:m_project			( p ),
m_item_ids			(gcnew item_id_list(objects)),
m_selection_method	(method)
{
	if(m_item_ids->Count==0)
		LOG_INFO("sel empty");
}

command_select::command_select( project^ p, u32 id, enum_selection_method method )
:m_project			( p ),
m_item_ids			(gcnew item_id_list),
m_selection_method	(method)
{
	m_item_ids->Add	( id );
	if(m_item_ids->Count==0)
		LOG_INFO("sel empty");
}


bool command_select::commit	() 
{
	return work( m_selection_method );
}

bool command_select::work		( enum_selection_method method )
{
	//////////////////////////////////////////////////////////////////////////
	// Here need to be code identifying the object by ID
	//////////////////////////////////////////////////////////////////////////
	project_items_list^ old_selection		=  m_project->selection_list();

	if( m_item_ids->Count == old_selection->Count  && m_item_ids->Count == 0 )
		return false;

	item_id_list^ tmp_objects = gcnew item_id_list;

	for each(project_item_base^ itm in old_selection )
		tmp_objects->Add	( itm->id() );

	m_project->select_ids( m_item_ids, method );

	m_item_ids	= tmp_objects;

	// Followed calls will only set the list of object IDs.
	m_selection_method = enum_selection_method_set;

	return true;
}

void command_select::rollback	()
{
	(void)work( m_selection_method );
}

} // namespace editor
} // namespace xray
