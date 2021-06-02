////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "tool_utils.h"
#include "utils_tool_tab.h"
#include "world.h"
#include "object_camera.h"
#include "object_track.h"
#include <xray/editor/engine.h>
#include <xray/game/resource_manager.h>

using xray::editor::tool_utils;
using xray::editor::utils_tool_tab;
using xray::editor::object_base;
using xray::editor::object_camera;

tool_utils::tool_utils(xray::editor::detail::world& w)
:super(w)
{
	m_tool_tab 		= gcnew utils_tool_tab(*this);
	m_ui			= m_tool_tab;
	load_library	();
}

tool_utils::~tool_utils()
{}
	
void tool_utils::select_library_name(System::String^ name)
{
	super::select_library_name			(name);
	m_tool_tab->selected_label->Text	= "selected: ";
	m_tool_tab->selected_label->Text	+= (name!=nullptr) ? get_selected_library_name() : "<empty>";
}

void tool_utils::save_library()
{
	fs::path_string name			= xray::fs::get_executable_path();
	name += "resources/library/utils/utils.library";
	m_library_cfg->save_as(name.c_str());
}

object_base* tool_utils::create_raw_object(pcstr id)
{
	object_base* result = NULL;
	if(0==strings::compare(id, "base:camera"))
	{
		result = NEW(object_camera)(*this);
	}else
	if(0==strings::compare(id, "base:track"))
	{
		result = NEW(object_track)(*this);
	}else
		NODEFAULT;

	m_object_list.push_back(result);
	result->initialize_property_holder(); //tmp
	return result;
}

object_base* tool_utils::load_object(xray::configs::lua_config_value const& t)
{
	pcstr library_name			= t["lib_name"];
	lib_item* li				= get_library_item(library_name);
	object_base* result			= create_raw_object(li->m_config["lib_name"]);
	result->load				(t);
	result->set_lib_name		(library_name);

	return						result;
}

object_base* tool_utils::create_object(pcstr name)
{
	lib_item* li				= get_library_item(name);
	object_base* result			= create_raw_object(li->m_config["lib_name"]);
	result->load				(li->m_config);
	result->set_lib_name		(name);

	return						result;
}

void tool_utils::destroy_object(object_base* o)
{
	object_list::iterator it	= std::find(m_object_list.begin(), m_object_list.end(), o);
	R_ASSERT					(it!=m_object_list.end());
	m_object_list.erase			(it);
	DELETE						(o);
}

void tool_utils::load_library()
{
	xray::resources::query_resources(
		"resources/library/utils/utils.library",
		xray::resources::config_lua_class,
		resources::query_callback(this, &tool_base::on_library_files_loaded),
		g_allocator
		);

	world().engine().get_resource_manager().query( "editor/camera", game::grt_solid_visual, game::query_resource_callback(this, &tool_utils::on_preview_visual_ready));
}

void tool_utils::on_preview_visual_ready(xray::resources::unmanaged_resource_ptr data)
{
	render::base::visual_ptr v = static_cast_checked<render::base::visual*>(data.get());

	m_preview_models.push_back	(v);
}

void tool_utils::on_library_loaded()
{
	super::on_library_loaded();
	fill_ui					();
}

static tree_node^ make_hierrarchy(tree_node_collection^ root, pcstr name)
{
	string_path		tok;
	pcstr p			= name;
	tree_node_collection^ current_root = root;
	tree_node^		result;
	do{
		p						= xray::strings::get_token(p, tok, '/');
		System::String^ folder	= gcnew System::String(tok);

		array<tree_node^>^ node_arr = current_root->Find(folder ,false);
		if(node_arr->Length!=0)
		{
			current_root = node_arr[0]->Nodes;
		}else
		{

			result						= current_root->Add(folder);
			result->ImageKey			= "group";
			result->SelectedImageKey	= "group";
			result->Tag					= "group";
			result->Name				= folder;
			current_root				= result->Nodes;
		}
	}while(p);
	R_ASSERT		(result, "item[%s] already exist", name);
	return			result;
}

void tool_utils::fill_ui()
{
	m_tool_tab->treeView->Nodes->Clear();

	library_it it		= m_library.begin();
	library_it it_e		= m_library.end();

	for(;it!=it_e; ++it)
	{
		lib_item* itm					= *it;
		pcstr short_name				= itm->m_lib_item_name;
		tree_node^	node				= make_hierrarchy(m_tool_tab->treeView->Nodes, short_name);
		node->ImageKey					= "camera";
		node->SelectedImageKey			= "camera";
		node->Tag						= "utils";
	}
	m_tool_tab->toolStripStatusLabel1->Text = System::String::Format("Total:{0} items", m_library.size());
}
