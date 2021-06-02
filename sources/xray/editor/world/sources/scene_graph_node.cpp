////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "scene_graph_node.h"
#include "object_base.h"
#include "tool_base.h"
#include "level_editor.h"
#include "project.h"
#include "command_delete_object.h"

using xray::editor::scene_graph_node;
using xray::editor::object_base;

void scene_graph_node::set_object_impl(object_base^ o)
{
	ASSERT			(m_object == nullptr); // only once
	m_object		= o;
	init_internal	();
}

System::String^	 scene_graph_node::caption()
{
	return object->get_name();
}

void scene_graph_node::init_internal()
{
	System::Type^ t					= object->GetType();
	array<System::Reflection::PropertyInfo^>^	type_props;
	type_props						= t->GetProperties();
	
	System::String^					property_name;
	System::Type^					property_type;
	array<System::Object^>^			attribs;
	for each (System::Reflection::PropertyInfo^ pi in type_props)
	{
		attribs							= pi->GetCustomAttributes(ConnectionAttribute::typeid, true);
		if(attribs->Length==0)
			continue;

		attribs							= pi->GetCustomAttributes(DisplayNameAttribute::typeid, true);
		DisplayNameAttribute^ att_name	= safe_cast<DisplayNameAttribute^>(attribs[0]);
		property_name					= att_name->DisplayName;
		property_type					= pi->PropertyType;
		add_connection_point	(property_type, property_name);
	}
}

void scene_graph_node::on_destroyed()
{
	level_editor^ le = object->owner_tool()->get_level_editor();
	le->get_command_engine()->run( gcnew xray::editor::command_delete_objects(le, object->object_id ));
}
