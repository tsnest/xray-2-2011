////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_hypergraph.h"
#include "dialog_document.h"
#include "dialog_editor.h"
#include "dialog_links_manager.h"
#include "dialog_graph_link.h"
#include "command_move_nodes.h"
#include "command_add_delete_links.h"
#include "command_add_delete_group.h"
#include "command_change_node_properties.h"
#include "command_change_node_string_table.h"
#include "dialog_graph_node_base.h"
#include "dialog_graph_node_phrase.h"

using xray::dialog_editor::dialog_hypergraph;
using xray::dialog_editor::dialog_document;
using xray::dialog_editor::dialog_editor_impl;
using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::node_style;
using xray::editor::controls::hypergraph::link;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::view_mode;
using xray::dialog_editor::dialog_graph_link;
using xray::dialog_editor::dialog_link_action;
using xray::dialog_editor::dialog_link_precondition;
using xray::editor_base::command_engine;

dialog_hypergraph::dialog_hypergraph(dialog_document^ owner)
:m_owner(owner),
m_move_nodes_command(nullptr),
m_start_move_point(0, 0),
m_links_move_mode(false),
m_moving_links(gcnew links()),
m_moving_links_start_point(nullptr)
{
	m_command_engine = gcnew command_engine(100);
	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	cut_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	copy_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	paste_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	delete_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	m_context_menu->SuspendLayout();
	SuspendLayout();
	m_context_menu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^ >(4)
		{cut_menu_item, copy_menu_item, paste_menu_item, delete_menu_item});
	m_context_menu->Name = L"m_context_menu";
	m_context_menu->Size = System::Drawing::Size(153, 92);

	cut_menu_item->Name = L"cut_menu_item";
	cut_menu_item->Size = System::Drawing::Size(152, 22);
	cut_menu_item->Text = L"Cut";
	cut_menu_item->Click += gcnew System::EventHandler(this, &dialog_hypergraph::cut);

	copy_menu_item->Name = L"copy_menu_item";
	copy_menu_item->Size = System::Drawing::Size(152, 22);
	copy_menu_item->Text = L"Copy";
	copy_menu_item->Click += gcnew System::EventHandler(this, &dialog_hypergraph::copy);

	paste_menu_item->Name = L"paste_menu_item";
	paste_menu_item->Size = System::Drawing::Size(152, 22);
	paste_menu_item->Text = L"Paste";
	paste_menu_item->Click += gcnew System::EventHandler(this, &dialog_hypergraph::paste);

	delete_menu_item->Name = L"delete_menu_item";
	delete_menu_item->Size = System::Drawing::Size(152, 22);
	delete_menu_item->Text = L"Delete";
	delete_menu_item->Click += gcnew System::EventHandler(this, &dialog_hypergraph::del);

	m_context_menu->ResumeLayout(false);
	ContextMenuStrip = m_context_menu;
	ResumeLayout(false);
}

dialog_hypergraph::~dialog_hypergraph()
{
	if(m_move_nodes_command != nullptr)
		delete m_move_nodes_command;

	delete m_command_engine;
}

void dialog_hypergraph::undo()
{
	m_command_engine->rollback(-1);
	Invalidate(false);
}

void dialog_hypergraph::redo()
{
	m_command_engine->rollback(1);
	Invalidate(false);
}

void dialog_hypergraph::cut(System::Object^ , System::EventArgs^ )
{
	get_owner()->copy(true);
}

void dialog_hypergraph::copy(System::Object^ , System::EventArgs^ )
{
	get_owner()->copy(false);
}

void dialog_hypergraph::paste(System::Object^ , System::EventArgs^ )
{
	get_owner()->paste();
}

void dialog_hypergraph::del(System::Object^ , System::EventArgs^ )
{
	get_owner()->del();
}

void dialog_hypergraph::destroy_connection(link^ lnk)
{
	links^ lnks = gcnew links();
	lnks->Add(lnk);
	destroy_connection(lnks);
}

void dialog_hypergraph::destroy_connection(links^ lnks)
{
	Generic::List<dialog_graph_link^>^ new_lnks = gcnew Generic::List<dialog_graph_link^>();
	for each(xray::editor::controls::hypergraph::link^ lnk in lnks)
		new_lnks->Add(safe_cast<dialog_graph_link^>(lnk));

	m_owner->is_saved = !m_command_engine->run(gcnew command_remove_links(this, new_lnks));
}

void dialog_hypergraph::make_connection(connection_point^ pt_src, connection_point^ pt_dst)
{
	dialog_graph_link^ lnk = gcnew dialog_graph_link(pt_src, pt_dst, true);
	links^ lnks = gcnew links();
	lnks->Add(lnk);
	add_connection(lnks);
}

void dialog_hypergraph::add_connection(links^ lnks)
{
	Generic::List<dialog_graph_link^>^ new_lnks = gcnew Generic::List<dialog_graph_link^>();
	for each(xray::editor::controls::hypergraph::link^ lnk in lnks)
		new_lnks->Add(safe_cast<dialog_graph_link^>(lnk));

	m_owner->is_saved = !m_command_engine->run(gcnew command_add_links(this, new_lnks));
}

Void dialog_hypergraph::drag_start()
{
	super::drag_start();
	m_start_move_point = m_selection_mode_start_point;
}

Void dialog_hypergraph::drag_stop()
{
	super::drag_stop();
	if(selected_nodes->Count==0)
		return;

	if(m_start_move_point.X!=m_selection_mode_start_point.X
		|| m_start_move_point.Y!=m_selection_mode_start_point.Y)
	{
		Point offset = Point(m_selection_mode_start_point.X-m_start_move_point.X, m_selection_mode_start_point.Y-m_start_move_point.Y);
		offset.X = int(Math::Round(offset.X/m_scale));
		offset.Y = int(Math::Round(offset.Y/m_scale));
		typedef System::Collections::Generic::List<dialog_graph_node_base^> nodes_lst;
		nodes_lst^ lst = gcnew nodes_lst();
		for each(dialog_graph_node_base^ n in selected_nodes)
			lst->Add(n);

		m_owner->is_saved = !m_command_engine->run(gcnew command_move_nodes(this, lst, offset));
	}
}

void dialog_hypergraph::links_move_start(links^ lnks, connection_point^ cp)
{
	m_links_move_mode = true;
	m_moving_links->AddRange(lnks);
	m_moving_links_start_point = cp;
}

void dialog_hypergraph::links_move_update()
{
	Invalidate(false);
}

void dialog_hypergraph::links_move_stop()
{
	m_links_move_mode = false;
	node^ cur_node = get_node_under_cursor();
	if(cur_node==nullptr)
	{
		for each(link^ lnk in m_moving_links)
			m_selected_links->Remove(lnk);

		destroy_connection(m_moving_links);
	}
	else
		m_owner->is_saved = !m_command_engine->run(gcnew command_change_links_connection_point(this, m_moving_links, cur_node, m_moving_links[0]->destination_point==m_moving_links_start_point));

	m_moving_links_start_point = nullptr;
	m_moving_links->Clear();
}

void dialog_hypergraph::hypergraph_Paint(System::Object^ , System::Windows::Forms::PaintEventArgs^ e)
{
	if(m_command_engine->saved_flag())
		m_owner->is_saved = true;
	else
		m_owner->is_saved = false;

	if(m_links_move_mode)
	{
		for each(link^ l in m_moving_links)
		{
			Point pt_start;
			Point pt_end;
			if(l->destination_point==m_moving_links_start_point)
			{
				pt_start = PointToClient(l->source_node->PointToScreen(l->source_point->get_point()));
				pt_end = PointToClient(MousePosition);
			}
			else
			{
				pt_start = PointToClient(MousePosition);
				pt_end = PointToClient(l->destination_node->PointToScreen(l->destination_point->get_point()));
			}
			Drawing::Color bk = m_link_pen->Color;
			System::Drawing::Drawing2D::DashStyle st = m_link_pen->DashStyle;
			m_link_pen->DashStyle = System::Drawing::Drawing2D::DashStyle::Dash;
			m_link_pen->Color = l->source_node->get_style()->get_link_color(node_style::e_states::active);
			draw_arrow(pt_start, pt_end, m_link_pen, e->Graphics);
			m_link_pen->DashStyle = st;
			m_link_pen->Color = bk;
		}
	}

	super::hypergraph_Paint(nullptr, e);
}

void dialog_hypergraph::on_key_down(Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode!=Keys::Delete)
		super::on_key_down(sender, e);

	get_owner()->get_editor()->on_editor_key_down(sender, e);
}

void dialog_hypergraph::on_key_up(Object^ , System::Windows::Forms::KeyEventArgs^ e)
{
	super::on_key_up(nullptr, e);
}

void dialog_hypergraph::hypergraph_area_MouseDown(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if((ModifierKeys & Keys::Alt)==Keys::Alt)
	{
		if(e->Button==System::Windows::Forms::MouseButtons::Left)
			move_start();

		return;
	}

	if(e->Button==System::Windows::Forms::MouseButtons::Right)
	{
		if(m_selected_links->Count==0 && m_selected_nodes->Count==0)
		{
			cut_menu_item->Enabled = false;
			copy_menu_item->Enabled = false;
			delete_menu_item->Enabled = false;
		}
		else
		{
			cut_menu_item->Enabled = m_selected_nodes->Count!=0;
			copy_menu_item->Enabled = m_selected_nodes->Count!=0;
			delete_menu_item->Enabled = true;
		}
	}

	node^ cur_node = get_node_under_cursor();
	connection_point^ cur_cp = nullptr;
	if(cur_node)
	{
		Drawing::Point node_mouse_pos = cur_node->PointToClient(MousePosition);
		cur_cp = cur_node->pick_connection_point(node_mouse_pos);
		if(cur_cp && e->Button==System::Windows::Forms::MouseButtons::Left)
		{

			links^ lnks = gcnew links();
			for each(link^ lnk in m_selected_links)
			{
				if(lnk->source_point==cur_cp || lnk->destination_point==cur_cp)
					lnks->Add(lnk);
			}
			if(lnks->Count!=0)
			{
				links_move_start(lnks, cur_cp);
				return;
			}
		}
	}
	super::hypergraph_area_MouseDown(nullptr, e);
}

void dialog_hypergraph::hypergraph_area_MouseMove(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(m_links_move_mode)
	{
		links_move_update();
		return;
	}

	super::hypergraph_area_MouseMove(nullptr, e);
}

void dialog_hypergraph::hypergraph_area_MouseUp(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
		move_stop();

	if(e->Button==System::Windows::Forms::MouseButtons::Left && m_links_move_mode)
		links_move_stop();

	super::hypergraph_area_MouseUp(nullptr, e);
}

void dialog_hypergraph::on_hypergraph_scroll(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Delta>0)
		change_scale(0.1f);
	else
		change_scale(-0.1f);

	display_rectangle = Drawing::Rectangle(display_rectangle.Location, Drawing::Size((int)(ClientRectangle.Width / this->scale), (int)(ClientRectangle.Height / this->scale)));
}

dialog_hypergraph::links^ dialog_hypergraph::copy_links()
{
	links^ tmp_lnks = gcnew links();
	for each(link^ lnk in links_manager->visible_links())
	{
		for each(node^ n in selected_nodes)
		{
			if(lnk->is_relative(n))
			{
				tmp_lnks->Add(lnk);
				break;
			}
		}
	}
	return tmp_lnks;
}

void dialog_hypergraph::save()
{
	m_command_engine->set_saved_flag();
	Invalidate(false);
}

void dialog_hypergraph::clear_command_stack()
{
	m_command_engine->clear_stack();
	Invalidate(false);
}

void dialog_hypergraph::add_group(pcstr config_string, Point offset)
{
	m_owner->is_saved = !m_command_engine->run(gcnew command_add_group(this, config_string, offset, get_node_under_cursor()));
}

void dialog_hypergraph::remove_group(nodes^ nds, links^ lnks)
{
	m_owner->is_saved = !m_command_engine->run(gcnew command_remove_group(this, nds, lnks));
}

void dialog_hypergraph::on_node_property_changed(node^ n, System::String^ l, System::Object^ v, System::Object^ ov)
{
	m_owner->is_saved = !m_command_engine->run(gcnew command_change_node_properties(this, safe_cast<dialog_graph_node_base^>(n), l, v, ov));
}

void dialog_hypergraph::on_node_text_changed(node^ n, String^ new_str, String^ new_text, String^ old_str, String^ old_text)
{
	m_owner->is_saved = !m_command_engine->run(gcnew command_change_node_string_table(this, safe_cast<dialog_graph_node_phrase^>(n), new_str, new_text, old_str, old_text));
}

void dialog_hypergraph::set_preconditions_and_actions(dialog_graph_link^ lnk, dialog_link_precondition^ prec_list, Generic::List<dialog_link_action^ >^ acts)
{
	m_owner->is_saved = !m_command_engine->run(gcnew command_set_actions_and_preconditions(this, lnk, acts, prec_list));
}

xray::dialog_editor::dialog_graph_node_base^ dialog_hypergraph::find_node_by_id(nodes^ nds, u32 id)
{
	for each(dialog_graph_node_base^ n in nds)
	{
		if(id==n->id)
			return n;
	}
	return nullptr;
}

dialog_graph_link^ dialog_hypergraph::find_link_by_ids(links^ lnks, u32 id1, u32 id2)
{
	for each(dialog_graph_link^ lnk in lnks)
	{
		dialog_graph_node_base^ n1 = safe_cast<dialog_graph_node_base^>(lnk->source_node);
		dialog_graph_node_base^ n2 = safe_cast<dialog_graph_node_base^>(lnk->destination_node);
		if(id1==n1->id && id2==n2->id)
			return lnk;
	}
	return nullptr;

}

void dialog_hypergraph::do_layout()
{
	if(m_nodes->Count==0)
		return;

	typedef Generic::List<node^ > layer;
	Generic::List<layer^ >^ layout = gcnew Generic::List<layer^ >();
	layer^ l = gcnew layer();
	layout->Add(l);
	nodes^ tmp_all_nodes = gcnew nodes();
	tmp_all_nodes->AddRange(m_nodes);
	nodes^ tmp_cur_nodes = gcnew nodes();
	tmp_cur_nodes->AddRange(m_nodes);

	for each(node^ n in tmp_cur_nodes)
	{
		bool is_dest = false;
		for each(link^ l in links_manager->visible_links())
		{
			if(l->destination_node==n)
			{
				is_dest = true;
				break;
			}
		}
		if(!is_dest)
		{
			l->Add(n);
			tmp_all_nodes->Remove(n);
		}
	}

	if(l->Count==0)
	{
		l->Add(m_nodes[0]);
		tmp_all_nodes->Remove(m_nodes[0]);
	}


	int layer_count = 1;
	while(tmp_all_nodes->Count>0)
	{
		layer^ cur_layer = gcnew layer();
		layout->Add(cur_layer);
		tmp_cur_nodes->Clear();
		tmp_cur_nodes->AddRange(tmp_all_nodes);
		for each(node^ parent in layout[layer_count-1])
		{
			for each(node^ n in tmp_cur_nodes)
			{
				bool is_child = false;
				for each(link^ l in links_manager->visible_links())
				{
					if(l->source_node==parent && l->destination_node==n)
					{
						is_child = true;
						break;
					}
				}
				if(is_child)
				{
					cur_layer->Add(n);
					tmp_all_nodes->Remove(n);
				}
			}
		}
		++layer_count;
	}

	int pos_x = 10;
	int pos_y = 10;
	for each(layer^ l in layout)
	{
		int max_height = 0;
		for each(node^ n in l)
		{
			n->position = Drawing::Point(pos_x, pos_y);
			pos_x += n->Width+10;
			if(max_height<n->Height)
				max_height = n->Height;
		}
		pos_y += max_height+10;
		pos_x = 10;
	}

	Invalidate(false);
}