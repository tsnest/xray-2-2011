////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_document.h"
#include "dialog_editor.h"
#include "dialog_graph_node_style.h"
#include "dialog_node_connection_editor.h"
#include "dialog_graph_link.h"
#include "dialog_links_manager.h"
#include "dialog_files_view_panel.h"
#include <xray/strings_stream.h>
#include "dialog.h"
#include "string_table_ids_storage.h"
#include "dialog_graph_node_layout.h"
#include "dialog_hypergraph.h"
#include "dialogs_manager.h"
#include "dialog_editor_resource.h"
#include "dialog_graph_node_base.h"
#include "dialog_graph_node_phrase.h"
#include "dialog_graph_node_dialog.h"
#include "dialog_phrase.h"

using namespace xray::dialog_editor;
using xray::dialog_editor::dialog_document;
using xray::dialog_editor::dialog_graph_nodes_style_mgr;
using xray::dialog_editor::dialog_links_manager;
using xray::dialog_editor::dialog_files_view_panel;
using xray::dialog_editor::dialog_node_connection_editor;
using xray::editor::controls::hypergraph::hypergraph_control;
using xray::editor::controls::hypergraph::connection_editor;
using xray::editor::controls::hypergraph::connection_point;
using xray::dialog_editor::drag_drop_create_operation;
using xray::editor::controls::scalable_scroll_bar;
using xray::editor::controls::scroller_event_args;

dialog_document::dialog_document(dialog_editor_impl^ ed)
:xray::editor::controls::document_base(ed->multidocument_base), 
m_owner(ed)
{
	m_resources = NEW(dialog_resources_ptr)();
	m_hypergraph = (gcnew dialog_hypergraph(this));

	SuspendLayout();

	// m_hypergraph
	m_hypergraph->AllowDrop = true;
	m_hypergraph->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
		| System::Windows::Forms::AnchorStyles::Left) 
		| System::Windows::Forms::AnchorStyles::Right));
	m_hypergraph->BackColor = System::Drawing::SystemColors::Window;
	m_hypergraph->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
	m_hypergraph->connection_editor = gcnew dialog_node_connection_editor(this);
	m_hypergraph->current_view_mode = xray::editor::controls::hypergraph::view_mode::full;
	m_hypergraph->focused_link = nullptr;
	m_hypergraph->focused_node = nullptr;
	m_hypergraph->Location = System::Drawing::Point(2, 27);
	m_hypergraph->max_scale = 5.0F;
	m_hypergraph->min_scale = 0.8F;
	m_hypergraph->move_mode = xray::editor::controls::hypergraph::nodes_movability::movable;
	m_hypergraph->Name = L"m_hypergraph";
	m_hypergraph->node_size_brief = System::Drawing::Size(80, 40);
	m_hypergraph->nodes_style_manager = get_editor()->get_node_style_mgr();
	m_hypergraph->scale = 1.0F;					
	m_hypergraph->Size = System::Drawing::Size(288, 244);
	m_hypergraph->TabIndex = 0;
	m_hypergraph->visibility_mode = xray::editor::controls::hypergraph::nodes_visibility::all;
	m_hypergraph->Click += gcnew System::EventHandler(this, &dialog_document::on_hypergraph_click);
	m_hypergraph->DoubleClick += gcnew System::EventHandler(this, &dialog_document::on_hypergraph_double_click);
	m_hypergraph->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &dialog_document::on_hypergraph_drag_over);
	m_hypergraph->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &dialog_document::on_hypergraph_drag_drop);
	m_hypergraph->on_selection_changed += gcnew xray::editor::controls::hypergraph::selection_changed(this, &dialog_document::on_hypergraph_selection_changed);
	m_links_manager = gcnew	dialog_links_manager(m_hypergraph);
	m_hypergraph->links_manager	= m_links_manager;
	m_hypergraph->clear();
	Controls->Add(m_hypergraph);
	this->Enter += gcnew System::EventHandler(this, &dialog_document::on_activated);
	this->VisibleChanged += gcnew System::EventHandler(this, &dialog_document::on_activated);
	ResumeLayout(false);
	source_path = get_editor()->resources_path+"configs/";
	extension = ".dlg";
}

dialog_document::~dialog_document()
{
	this->!dialog_document();
	if(components)
		delete components;
}

dialog_document::!dialog_document()
{
	m_owner->show_properties(nullptr);
	*m_resources = NULL;
	DELETE(m_resources);
}

dialog* dialog_document::get_dialog()
{
	return (*m_resources)->get_dialog();
}

void dialog_document::save()
{
	if(is_saved)
		return;

	String^ file_path = (source_path+this->Name);
	if((*m_resources)->m_layout)
		(*m_resources)->m_layout->m_layout.clear();
	else
		(*m_resources)->m_layout = NEW(dialog_graph_node_layout)();

	for each(dialog_graph_node_base^ n in m_hypergraph->all_nodes())
		(*m_resources)->m_layout->m_layout[n->id] = math::int2(n->position.X, n->position.Y);

	(*m_resources)->save(unmanaged_string(file_path).c_str());
	m_hypergraph->save();
}

void dialog_document::load()
{
	dialog_resources_delegate* cb = NEW(dialog_resources_delegate)(gcnew dialog_resources_delegate::Delegate(this, &dialog_document::on_resources_loaded));
	dialog_manager::get_dialogs_manager()->request_editor_dialog(unmanaged_string(this->Name).c_str(), boost::bind(&dialog_resources_delegate::callback, cb, _1));
}

void dialog_document::on_resources_loaded(xray::dialog_editor::dialog_resources_ptr res)
{
	(*m_resources) = res.c_ptr();
	load_impl((*m_resources)->get_dialog(), (*m_resources)->m_layout);
	m_hypergraph->clear_command_stack();
	m_hypergraph->save();
	is_saved = true;
}

void dialog_document::load_impl(dialog* d, dialog_graph_node_layout* l)
{
	nodes^ nds = gcnew nodes();
	links^ lnks = gcnew links();
	const vector<dialog_node_base*>* nodes_vec = d->get_phrases();
	vector<dialog_node_base*>::const_iterator i = nodes_vec->begin();
	for(; i!=nodes_vec->end(); ++i)
	{
		dialog_graph_node_base^ n = nullptr;
		if(dynamic_cast<dialog_phrase*>(*i))
			n = gcnew dialog_graph_node_phrase();
		else
			n = gcnew dialog_graph_node_dialog();

		n->object = *i;
		n->position = Point(0,0);
		if(!n->is_dialog())
		{
			System::String^ str_id = safe_cast<dialog_graph_node_phrase^>(n)->string_table;
			unmanaged_string str(str_id);
			if(xray::dialog_editor::string_tables::get_string_tables()->get_text_by_id(str.c_str())==NULL)
			{
				get_editor()->add_new_id(str_id, "");
				get_editor()->change_references_count(str_id, nullptr);
			}
		}

		if(l)
		{
			dialog_graph_node_layout::layout_vector::const_iterator it = l->m_layout.find(n->id);
			if(it!=l->m_layout.end())
				n->position = Point(it->second.x, it->second.y);
		}

		nds->Add(n);
	}

	for each(dialog_graph_node_base^ n in nds)
	{
		m_hypergraph->nodes_style_manager->assign_style(n, "dialog_node");
		n->set_scale(m_hypergraph->scale);
		n->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_down);
		n->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(m_hypergraph, &dialog_hypergraph::on_key_up);
		m_hypergraph->Controls->Add(n);
		m_hypergraph->all_nodes()->Add(n);
		n->on_added(m_hypergraph);
		m_hypergraph->links_manager->on_node_added(n);
	}

	i = nodes_vec->begin();
	for(; i!=nodes_vec->end(); ++i)
	{
		const links_list* lnks_list = (*i)->get_links();
		dialog_link* cur_dlg_link = lnks_list->front();
		while(cur_dlg_link!=NULL)
		{
			dialog_graph_node_base^ n1 = dialog_hypergraph::find_node_by_id(nds, (*i)->id()); 
			dialog_graph_node_base^ n2 = dialog_hypergraph::find_node_by_id(nds, cur_dlg_link->child()->id()); 
			if(n1 && n2)
			{
				dialog_graph_link^ lnk = create_connection(n1, n2, false);
				lnk->m_link = cur_dlg_link;
				lnks->Add(lnk);
			}
			cur_dlg_link = lnks_list->get_next_of_object(cur_dlg_link);
		}
	}

	for each(xray::editor::controls::hypergraph::link^ lnk in lnks)
		m_hypergraph->links_manager->visible_links()->Add(lnk);

	m_hypergraph->center_nodes();
}

void dialog_document::on_activated(System::Object^ , System::EventArgs^ )
{
	if(this->Visible)
	{
		m_owner->view_panel->track_active_item(Name);
		m_hypergraph->scale += 0;
		for each(xray::editor::controls::hypergraph::node^ n in m_hypergraph->all_nodes())
		{
			dialog_graph_node_dialog^ dlg = dynamic_cast<dialog_graph_node_dialog^>(n);
			if(dlg)
				dlg->set_expanded(false);
		}
	}
}

dialog_editor_impl^ dialog_document::get_editor()
{
	return m_owner;
}

void dialog_document::on_hypergraph_click(System::Object^ , System::EventArgs^ e)
{
	if((ModifierKeys & Keys::Shift) == Keys::Shift ||		
		(ModifierKeys & Keys::Alt) == Keys::Alt)
		return;

	if(safe_cast<System::Windows::Forms::MouseEventArgs^>(e)->Button==System::Windows::Forms::MouseButtons::Left)
	{
		links^ l = m_hypergraph->get_links_under_cursor();
		if(l->Count!=0)
			m_owner->show_properties(l[l->Count-1]);
		else if(m_hypergraph->selected_nodes->Count==0)
			m_owner->show_properties(nullptr);
	}
}

void dialog_document::on_hypergraph_double_click(System::Object^ , System::EventArgs^ e)
{
	if((ModifierKeys & Keys::Control) == Keys::Control ||
		(ModifierKeys & Keys::Shift) == Keys::Shift ||		
		(ModifierKeys & Keys::Alt) == Keys::Alt)
		return;

	if(safe_cast<System::Windows::Forms::MouseEventArgs^>(e)->Button==System::Windows::Forms::MouseButtons::Left)
	{
		if(m_hypergraph->focused_link!=nullptr)
			get_editor()->show_prec_act_editor(safe_cast<dialog_graph_link^ >(m_hypergraph->focused_link));
	}
}

void dialog_document::undo()
{
	m_hypergraph->undo();
}

void dialog_document::redo()
{
	m_hypergraph->redo();
}

void dialog_document::select_all()
{
	m_hypergraph->selected_links->AddRange(m_links_manager->visible_links());
	m_hypergraph->set_selected_node(m_hypergraph->all_nodes());
}

void dialog_document::copy(bool del)
{
	nodes^ nds = m_hypergraph->selected_nodes;
	links^ lnks = m_hypergraph->copy_links();
	links^ copied_lnks = gcnew links();
	copied_lnks->AddRange(lnks);
	for each(xray::editor::controls::hypergraph::link^ lnk in lnks)
	{
		if(!(nds->Contains(lnk->source_node) && nds->Contains(lnk->destination_node)))
			copied_lnks->Remove(lnk);
	}

	if(nds->Count==0)
		return;

	safe_cast<dialog_files_view_panel^>(get_editor()->view_panel)->copied_node = nullptr;
	configs::lua_config_ptr cfg = configs::create_lua_config();
	xray::strings::stream stream(g_allocator);

	dialog* tmp_dialog = NEW(dialog)(cfg->get_root());
	tmp_dialog->set_name("tmp_dialog");
	dialog_graph_node_layout* tmp_layout = NEW(dialog_graph_node_layout)(cfg->get_root());
	for each(dialog_graph_node_base^ n in nds)
	{
		tmp_dialog->add_node(n->object);
		tmp_layout->m_layout[n->id] = math::int2(n->position.X, n->position.Y);
	}

	tmp_dialog->save(cfg->get_root());
	tmp_layout->save(cfg->get_root());
	tmp_dialog->clear();
	DELETE(tmp_dialog);
	DELETE(tmp_layout);

	cfg->save(stream);
	stream.append("%c", 0);
	xray::os::copy_to_clipboard((pcstr)stream.get_buffer());
	if(del)
		m_hypergraph->remove_group(nds, lnks);
}

void dialog_document::paste()
{
	if(safe_cast<dialog_files_view_panel^>(get_editor()->view_panel)->copied_node!=nullptr)
	{
		System::String^ node_name = safe_cast<dialog_files_view_panel^>(get_editor()->view_panel)->copied_node->FullPath;
		if(node_name->Remove(0, 8)==Name)
			MessageBox::Show(this, "Can't insert dialog to itself.", "Dialog editor", MessageBoxButtons::OK, MessageBoxIcon::Error);
		else
			add_dialog_node(node_name, MousePosition);

		return;
	}

	pstr string	= xray::os::paste_from_clipboard(*g_allocator);
	// clipboard doesn't contain text information
	if(!string)
	{
		// we could use function xray::os::is_clipboard_empty() to check
		// whether there non-text information and show some message in that case
		return;
	}

	m_hypergraph->add_group(string, m_hypergraph->PointToClient(MousePosition));
	FREE(string);
}

void dialog_document::del()
{
	links^ tmp_links = gcnew links();
	tmp_links->AddRange(m_hypergraph->copy_links());
	for each(dialog_graph_link^ lnk in m_hypergraph->selected_links)
		if(!tmp_links->Contains(lnk))
			tmp_links->Add(lnk);

	nodes^ nods = gcnew nodes();
	nods->AddRange(m_hypergraph->selected_nodes);
	m_hypergraph->set_selected_node((nodes^)nullptr);

	if(nods->Count!=0 || tmp_links->Count!=0)
		m_hypergraph->remove_group(nods, tmp_links);

	m_hypergraph->selected_links->Clear();
}

void dialog_document::on_hypergraph_selection_changed()
{
	if(m_hypergraph->selected_nodes->Count!=0)
	{
		dialog_graph_node_base^ n = safe_cast<dialog_graph_node_base^>(m_hypergraph->selected_nodes[m_hypergraph->selected_nodes->Count-1]);
		get_editor()->on_selection_changed(n);
		m_owner->show_properties(n);
	}
	else if(m_hypergraph->selected_links->Count!=0)
		m_owner->show_properties(m_hypergraph->selected_links[0]);
	else
		m_owner->show_properties(nullptr);
}

void dialog_document::on_node_property_changed(dialog_graph_node_base^ n, String^ l, Object^ new_val, Object^ old_val)
{
	m_hypergraph->on_node_property_changed(n, l, new_val, old_val);
}

void dialog_document::on_node_text_changed(dialog_graph_node_phrase^ n, String^ new_str, String^ new_text, String^ old_str, String^ old_text)
{
	m_hypergraph->on_node_text_changed(n, new_str, new_text, old_str, old_text);
}

void dialog_document::update_texts()
{
	for each(dialog_graph_node_base^ n in m_hypergraph->all_nodes())
	{
		if(!(n->is_dialog()))
			n->Invalidate(false);
	}
	m_owner->show_properties(nullptr);
	on_hypergraph_selection_changed();
}

void dialog_document::on_hypergraph_drag_over(System::Object^ , System::Windows::Forms::DragEventArgs^ e)
{
	if(e->Data->GetDataPresent(drag_drop_create_operation::typeid))
	{
		e->Effect = DragDropEffects::Copy;
		return;
	}

	if(e->Data->GetDataPresent(System::String::typeid))
	{
		System::String^ node_name = safe_cast<System::String^>(e->Data->GetData(System::String::typeid));
		node_name = node_name->Remove(0,8);
		System::String^ file_path = (source_path+node_name+extension);
		if(System::IO::File::Exists(file_path) && node_name!=Name)
			e->Effect = DragDropEffects::Copy;
		else
			e->Effect = DragDropEffects::None;

		return;
	}

	e->Effect = DragDropEffects::None;
}

void dialog_document::on_hypergraph_drag_drop(System::Object^ , System::Windows::Forms::DragEventArgs^ e)
{
	if(e->Data->GetDataPresent(drag_drop_create_operation::typeid))
	{
		drag_drop_create_operation^ o = safe_cast<drag_drop_create_operation^>(e->Data->GetData(drag_drop_create_operation::typeid));
		if(e->Effect==DragDropEffects::Copy)
		{
			switch(*o)
			{
			case drag_drop_create_operation::single_node:
				{
					on_drop_single_node(e);
					break;
				}
			case drag_drop_create_operation::question_answer:
				{
					on_drop_question_answer(e);
					break;
				}
			}
		}
	}
	else if(e->Data->GetDataPresent(System::String::typeid))
	{
		if(e->Effect==DragDropEffects::Copy)
			on_drop_dialog(e);
	}
	else
		e->Effect = DragDropEffects::None;
}