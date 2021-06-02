////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_add_library_object.h"
#include "level_editor.h"
#include "tool_base.h"
#include "object_base.h"
#include "project_items.h"
#include "project.h"
#include "command_select.h"


namespace xray {
namespace editor {

command_add_library_object::command_add_library_object	(	level_editor^ le, 
															tool_base^ tool, 
															System::String^ library_name, 
															math::float4x4 transform,
															object_added_callback^ callback,
															bool select_after_adding ):
m_level_editor			( le ),
m_tool					( tool ),
m_object_id				( 0 ),
m_object_added_callback	( callback ),
m_select_after_adding	( select_after_adding )
{
	m_transform			= NEW (float4x4)(transform);
	ASSERT				( library_name!=nullptr );
	ASSERT				( library_name->Length!=0 );

	m_library_name		= library_name;

	// bug
	m_parent_folder_name	= m_level_editor->get_project()->get_focused_folder()->get_full_name();
}

command_add_library_object::~command_add_library_object			() 
{
	DELETE (m_transform);
}

bool command_add_library_object::commit		()
{
	// if we are in rollback stage than we need to reuse the last id.
	// else the project will generate a new one.

	project_item_object^ item	= m_level_editor->get_project()->add_new_item( m_tool, m_library_name, *m_transform, m_parent_folder_name, m_object_id );
	
	object_base^ o				= item->m_object_base;

	m_object_id					= o->id();

	if ( m_object_added_callback != nullptr )
		m_object_added_callback( item );

	if (m_select_after_adding)
	{
		m_level_editor->get_command_engine()->run( gcnew command_select( m_level_editor->get_project(), 
																		m_object_id, 
																		enum_selection_method_set ));
	}

	return true;
}

void command_add_library_object::rollback( )
{
	project_item_base^ item					= project_item_base::object_by_id	( m_object_id );
	m_level_editor->get_project()->remove	( item, true );
}

command_add_project_folder::command_add_project_folder( level_editor^ le, System::String^ name )
:m_level_editor		( le ),
m_name				( name ),
m_added_folder_id	( 0 ),
m_parent_folder_id	( u32(-1) )
{
}

bool command_add_project_folder::commit( )
{
	project^ p						= m_level_editor->get_project();

	project_item_folder^ parent		= (m_parent_folder_id==-1) ? 
										p->get_focused_folder() : 
										safe_cast<project_item_folder^>(project_item_base::object_by_id(m_parent_folder_id));

	project_item_folder^ folder		= p->add_new_folder( m_name, parent, m_added_folder_id );
	folder->after_name_edit			( m_name );
	
	m_added_folder_id				= folder->id();
	m_parent_folder_id				= parent->id();

	return true;
}

void command_add_project_folder::rollback( )
{
	project_item_base^ item				= project_item_base::object_by_id( m_added_folder_id );
	m_level_editor->get_project()->remove( item, true );
}

}// namespace editor
}// namespace xray
