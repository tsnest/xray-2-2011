////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_create_remove_link.h"
#include "object_base.h"

namespace xray {
namespace editor {

command_create_remove_link::command_create_remove_link ( enum_action action, link_storage^ link_storage, object_base^ src_object, System::String^ src_name, object_base^ dst_object, System::String^ dst_name ):
m_action		( action ),
m_link_storage	( link_storage ),
m_src_id		( src_object->id() ),
m_dst_id		( dst_object->id()),
m_src_name		( gcnew System::String( src_name )),
m_dst_name		( gcnew System::String( dst_name ))
{
	ASSERT( link_storage != nullptr );
}

bool command_create_remove_link::commit	()
{
	NOT_IMPLEMENTED( return true );
	//if( m_action == enum_action::enum_link_action_create )
	//	m_link_storage->create_link	(m_level_editor->object_by_id( m_src_id ), m_src_name, m_level_editor->object_by_id( m_dst_id ), m_dst_name );
	//else if( m_action == enum_action::enum_link_action_remove )
	//	m_link_storage->remove_link (m_level_editor->object_by_id( m_src_id ), m_src_name, m_level_editor->object_by_id( m_dst_id ), m_dst_name );
	//else 
	//	return false;
}

void command_create_remove_link::rollback	()
{
	NOT_IMPLEMENTED();
	//if( m_action == enum_action::enum_link_action_create )
	//	m_link_storage->remove_link (object_base::object_by_id( m_src_id ), m_src_name, object_base::object_by_id( m_dst_id ), m_dst_name );
	//else if( m_action == enum_action::enum_link_action_remove )
	//	m_link_storage->create_link	(object_base::object_by_id( m_src_id ), m_src_name, object_base::object_by_id( m_dst_id ), m_dst_name );
}

} // namespace editor
} // namespace xray
