////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_document.h"
#include "dialog_editor.h"
#include "dialog_hypergraph.h"
#include "dialog_graph_node_style.h"
#include "dialog_node_connection_editor.h"
#include "dialog_graph_link.h"
#include "dialog.h"
#include "dialog_phrase.h"
#include "dialog_graph_node_base.h"
#include "dialog_graph_node_phrase.h"
#include <xray/strings_stream.h>

using namespace xray::editor;
using xray::dialog_editor::dialog_document;
using xray::dialog_editor::dialog_graph_nodes_style_mgr;
using xray::dialog_editor::dialog_links_manager;
using xray::dialog_editor::dialog_graph_link;
using xray::dialog_editor::dialog_node_connection_editor;
using xray::editor::controls::hypergraph::hypergraph_control;
using xray::editor::controls::hypergraph::connection_editor;
using xray::editor::controls::hypergraph::connection_point;
using xray::dialog_editor::drag_drop_create_operation;
using xray::dialog_editor::dialog;
using xray::dialog_editor::dialog_phrase;

dialog_graph_link^ dialog_document::create_connection(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination, bool create_dialog_link)
{
	dialog_graph_node_base^ src	= safe_cast<dialog_graph_node_base^>(source);
	dialog_graph_node_base^ dst	= safe_cast<dialog_graph_node_base^>(destination);
	xray::editor::controls::hypergraph::connection_point^ src_pt = src->get_connection_point("child");
	xray::editor::controls::hypergraph::connection_point^ dst_pt = dst->get_connection_point("parent");
	if(src_pt==nullptr)
		src_pt = src->add_connection_point(dialog_graph_node_base::typeid, "child");

	if(dst_pt==nullptr)
		dst_pt = dst->add_connection_point(dialog_graph_node_base::typeid, "parent");

	src_pt->draw_text = false;
	dst_pt->draw_text = false;
	m_hypergraph->display_rectangle = m_hypergraph->display_rectangle;
	if(m_hypergraph->can_make_connection(src_pt, dst_pt))
	{
		dialog_graph_link^ new_link = gcnew dialog_graph_link(src_pt, dst_pt, create_dialog_link);
		return new_link;
	}
	return nullptr;
}

void dialog_document::add_connection(xray::editor::controls::hypergraph::node^ source, xray::editor::controls::hypergraph::node^ destination)
{
	if(source==destination)
		return;

	dialog_graph_node_base^ src	= safe_cast<dialog_graph_node_base^>(source);
	dialog_graph_node_base^ dst	= safe_cast<dialog_graph_node_base^>(destination);
	if(dialog_hypergraph::find_link_by_ids(m_hypergraph->links_manager->visible_links(), src->id, dst->id))
		return;

	dialog_graph_link^ lnk = create_connection(source, destination, true);
	if(lnk!=nullptr)
	{
		links^ lnks = gcnew links();
		lnks->Add(lnk);
		m_hypergraph->add_connection(lnks);
	}
}

void dialog_document::add_connection(links^ lnks)
{
	m_hypergraph->add_connection(lnks);
}

void dialog_document::generate_node_string_table_id(xray::dialog_editor::dialog_graph_node_phrase^ n)
{
	System::String^ str_id = this->Name->Replace("/", "_")+"_string_table_id_";
	u32 i = 0;
	while(get_editor()->get_text_by_id(str_id+i)!=nullptr)
		i++;

	get_editor()->add_new_id(str_id+i, "");
	n->string_table = str_id+i;
}

static System::Drawing::Point on_drop_offset = System::Drawing::Point(0,0);
void dialog_document::on_drop_single_node(System::Windows::Forms::DragEventArgs^ e)
{
	on_drop_offset = m_hypergraph->PointToClient(Point(e->X, e->Y));
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &dialog_document::on_template_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/dialogs/templates/single_node.template",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void dialog_document::on_drop_question_answer(System::Windows::Forms::DragEventArgs^ e)
{
	on_drop_offset = m_hypergraph->PointToClient(Point(e->X, e->Y));
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &dialog_document::on_template_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/dialogs/templates/question_answer.template",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);
}

void dialog_document::on_drop_dialog(System::Windows::Forms::DragEventArgs^ e)
{
	System::String^ node_name = safe_cast<System::String^>(e->Data->GetData(System::String::typeid));
	add_dialog_node(node_name->Remove(0, 8), Point(e->X, e->Y));
}

static pstr st_dialog_name = NULL;
void dialog_document::add_dialog_node(System::String^ node_name, Point p)
{
	on_drop_offset = m_hypergraph->PointToClient(p);
	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &dialog_document::on_dialog_loaded), g_allocator);
	xray::resources::query_resource(
		unmanaged_string("resources/dialogs/templates/single_node.template").c_str(),
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);

	if(st_dialog_name!=NULL)
		FREE(st_dialog_name);
	st_dialog_name = strings::duplicate(g_allocator, unmanaged_string(node_name).c_str());
}

void dialog_document::on_template_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());	

	if(!data.is_successful())
		return;

	if(data.size() > 0)
	{
		configs::lua_config_ptr cfg_ptr = static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
		xray::strings::stream stream(g_allocator);
		cfg_ptr->save(stream);
		stream.append("%c", 0);
		m_hypergraph->add_group((pcstr)stream.get_buffer(), on_drop_offset);
	}
}

void dialog_document::on_dialog_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());	

	if(!data.is_successful())
		return;

	if(data.size() > 0)
	{
		configs::lua_config_ptr cfg_ptr = static_cast_resource_ptr<configs::lua_config_ptr>(data[0].get_unmanaged_resource());
		xray::strings::stream stream(g_allocator);
		configs::lua_config_value cfg = *(cfg_ptr->get_root()["dialog"]["nodes"].begin());
		cfg["string_table"] = st_dialog_name;
		cfg["is_dialog"] = true;
		cfg_ptr->save(stream);
		stream.append("%c", 0);
		m_hypergraph->add_group((pcstr)stream.get_buffer(), on_drop_offset);
	}
}

