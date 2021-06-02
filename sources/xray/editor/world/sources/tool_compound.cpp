////////////////////////////////////////////////////////////////////////////
//	Created		: 09.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_compound.h"
#include "object_compound.h"
#include "level_editor.h"
#include "compound_tool_tab.h"
#include "compound_object_tab.h"
#include "property_container.h"
#include "editor_world.h"
#include <xray/editor/base/managed_delegate.h>

namespace xray{
namespace editor{

tool_compound::tool_compound(xray::editor::level_editor^ le)
:super(le, "compound")
{
	m_tool_tab 		= gcnew compound_tool_tab(this);
	m_object_tab 	= gcnew compound_object_tab(this);
	load_library	();
}

tool_compound::~tool_compound()
{
}

xray::editor::tool_tab^ tool_compound::ui()
{
	return m_tool_tab;
}

void tool_compound::save_library()
{
	fs::path_string name			= m_level_editor->get_editor_world().engine().get_resource_path();
	name += "/library/compound/compound.library";
	(*m_library_cfg)->save_as(name.c_str());
}

void tool_compound::load_library()
{
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &tool_compound::on_library_files_loaded));
	xray::resources::query_resource(
		"resources/library/compound/compound.library",
		xray::resources::config_lua_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

object_base^ tool_compound::create_object(System::String^ name)
{
	lib_item^ li			= get_library_item(name, true);
	System::String^ ln		= gcnew System::String((*(li->m_config))["lib_name"]);
	object_base^ result		= create_raw_object(ln);
	result->load			(*li->m_config);
	result->set_lib_name	(name);
	//::property_holder* ph	= dynamic_cast<::property_holder*>(result->get_property_holder());
	//li->m_property_restrictor->apply_to	(ph);

	return					result;
}


object_base^ tool_compound::load_object(xray::configs::lua_config_value const& t)
{
	XRAY_UNREFERENCED_PARAMETER	( t );
	UNREACHABLE_CODE(return nullptr);
}

object_base^ tool_compound::create_raw_object(System::String^ id)
{
	XRAY_UNREFERENCED_PARAMETER	( id );
	object_base^ result			=  gcnew object_compound(this);
	m_object_list->Add			(result);
	initialize_property_holder	(result); //tmp
	return						result;
}

void tool_compound::destroy_object(object_base^ o)
{
	R_ASSERT				(m_object_list->Contains(o));
	m_object_list->Remove	(o);
	
	object_compound^ oc		= safe_cast<object_compound^>(o);
	u32 count				= oc->count();
	
	for(u32 i=0; i<count; ++i)
	{
		object_base^ child	= oc->get_object(i);
		child->owner()->destroy_object(child);
	}
	delete o;
}

void tool_compound::get_objects_list(xray::editor::names_list list, xray::editor::enum_list_mode mode)
{
	XRAY_UNREFERENCED_PARAMETERS	( list, mode );
}

void tool_compound::on_library_loaded()
{
	super::on_library_loaded			();
	fill_tree_view						(m_tool_tab->treeView->nodes);
}

tree_node^ make_hierrarchy(tree_node_collection^ root, System::String^ name);

void tool_compound::fill_tree_view(tree_node_collection^ nodes)
{
	nodes->Clear();

	for each (lib_item^ itm in m_tool_library)
	{
		System::String^ short_name		= itm->m_lib_item_name;
		tree_node^	node				= make_hierrarchy(nodes, short_name);
		node->ImageKey					= "joint";
		node->SelectedImageKey			= "joint";
	}
	m_tool_tab->toolStripStatusLabel1->Text = System::String::Format("Total:{0} items", m_tool_library->Count);
}

void tool_compound::on_make_new_library_object()
{
	object_compound^ o					= safe_cast<object_compound^>(create_raw_object(nullptr));
	o->assign_id						(0);
	System::String^ new_name			= "compound_object";
	check_correct_lib_name				(new_name, 1);
	o->set_name							(new_name, true);
	m_object_tab->set_compound_object	(o, xray::editor::mode_new);

	ide()->show_tab		(m_object_tab);
}

void tool_compound::on_edit_library_object(System::String^ name)
{
	object_base^ o						= create_object(name);
	m_object_tab->set_compound_object	(safe_cast<object_compound^>(o), xray::editor::mode_edit_library);
	m_object_tab->Show					();
}


} // namespace editor
} // namespace xray
