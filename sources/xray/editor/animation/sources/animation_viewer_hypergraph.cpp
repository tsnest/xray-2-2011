////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_hypergraph.h"
#include "viewer_document.h"
#include "viewer_editor.h"
#include "animation_viewer_links_manager.h"
#include "animation_viewer_graph_link.h"
#include "animation_viewer_graph_link_weighted.h"
#include "animation_viewer_graph_link_timed.h"
#include "animation_viewer_graph_node_base.h"
#include "animation_viewer_add_delete_group.h"
#include "animation_viewer_move_nodes.h"
#include "animation_viewer_add_delete_links.h"
#include "animation_viewer_change_property.h"
#include "animation_viewer_graph_node_operator.h"
#include "animation_viewer_graph_node_animation.h"
#include "animation_viewer_graph_node_root.h"
#include "animation_viewer_change_lexeme.h"

using xray::animation_editor::animation_viewer_hypergraph;
using xray::animation_editor::viewer_document;
using xray::animation_editor::animation_viewer_graph_link;
using xray::animation_editor::animation_viewer_graph_node_base;
using xray::editor::controls::hypergraph::node;
using xray::editor::controls::hypergraph::node_style;
using xray::editor::controls::hypergraph::link;
using xray::editor::controls::hypergraph::connection_point;
using xray::editor::controls::hypergraph::view_mode;
using xray::editor_base::command_engine;

animation_viewer_hypergraph::animation_viewer_hypergraph(viewer_document^ owner)
:m_owner(owner),
m_move_nodes_command(nullptr),
m_start_move_point(0, 0)
{
	m_nodes_ids = gcnew System::Collections::Generic::List<u32>();
	m_command_engine = gcnew command_engine(100);
	m_context_menu = gcnew System::Windows::Forms::ContextMenuStrip();
	cut_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	copy_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	paste_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	delete_menu_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	save_clip_item = gcnew System::Windows::Forms::ToolStripMenuItem();
	m_context_menu->SuspendLayout();
	SuspendLayout();
	m_context_menu->Items->AddRange(gcnew cli::array<System::Windows::Forms::ToolStripItem^>(5) {
		cut_menu_item, copy_menu_item, paste_menu_item, delete_menu_item, save_clip_item});
	m_context_menu->Name = L"m_context_menu";
	m_context_menu->Size = System::Drawing::Size(153, 92);

	cut_menu_item->Name = L"cut_menu_item";
	cut_menu_item->Size = System::Drawing::Size(152, 22);
	cut_menu_item->Text = L"Cut";
	cut_menu_item->Click += gcnew System::EventHandler(this, &animation_viewer_hypergraph::cut);

	copy_menu_item->Name = L"copy_menu_item";
	copy_menu_item->Size = System::Drawing::Size(152, 22);
	copy_menu_item->Text = L"Copy";
	copy_menu_item->Click += gcnew System::EventHandler(this, &animation_viewer_hypergraph::copy);

	paste_menu_item->Name = L"paste_menu_item";
	paste_menu_item->Size = System::Drawing::Size(152, 22);
	paste_menu_item->Text = L"Paste";
	paste_menu_item->Click += gcnew System::EventHandler(this, &animation_viewer_hypergraph::paste);

	delete_menu_item->Name = L"delete_menu_item";
	delete_menu_item->Size = System::Drawing::Size(152, 22);
	delete_menu_item->Text = L"Delete";
	delete_menu_item->Click += gcnew System::EventHandler(this, &animation_viewer_hypergraph::del);

	save_clip_item->Name = L"save_clip_item";
	save_clip_item->Size = System::Drawing::Size(152, 22);
	save_clip_item->Text = L"Save node as clip";
	save_clip_item->Click += gcnew System::EventHandler(this, &animation_viewer_hypergraph::save_clip);
	
	m_context_menu->ResumeLayout(false);
	ContextMenuStrip = m_context_menu;
	ResumeLayout(false);
}

animation_viewer_hypergraph::~animation_viewer_hypergraph()
{
	if(m_move_nodes_command != nullptr)
		delete m_move_nodes_command;

	delete m_command_engine;
}

void animation_viewer_hypergraph::undo()
{
	m_command_engine->rollback(-1);
	Invalidate(false);
}

void animation_viewer_hypergraph::redo()
{
	m_command_engine->rollback(1);
	Invalidate(false);
}

void animation_viewer_hypergraph::cut(System::Object^ , System::EventArgs^ )
{
	get_owner()->copy(true);
}

void animation_viewer_hypergraph::copy(System::Object^ , System::EventArgs^ )
{
	get_owner()->copy(false);
}

void animation_viewer_hypergraph::paste(System::Object^ , System::EventArgs^ )
{
	get_owner()->paste();
}

void animation_viewer_hypergraph::del(System::Object^ , System::EventArgs^ )
{
	get_owner()->del();
}

void animation_viewer_hypergraph::destroy_connection(link^ lnk)
{
	links^ lnks = gcnew links();
	lnks->Add(lnk);
	destroy_connection(lnks);
}

void animation_viewer_hypergraph::destroy_connection(links^ lnks)
{
	m_owner->is_saved &= !m_command_engine->run(gcnew animation_viewer_remove_links(this, lnks));
}

void animation_viewer_hypergraph::add_connection(links^ lnks)
{
	m_owner->is_saved &= !m_command_engine->run(gcnew animation_viewer_add_links(this, lnks));
}

Void animation_viewer_hypergraph::drag_start()
{
	super::drag_start();
	m_start_move_point = m_selection_mode_start_point;
}

Void animation_viewer_hypergraph::drag_stop()
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
		typedef System::Collections::Generic::List<animation_viewer_graph_node_base^> nodes_lst;
		nodes_lst^ lst = gcnew nodes_lst();
		for each(animation_viewer_graph_node_base^ n in selected_nodes)
			lst->Add(n);

		m_owner->is_saved &= !m_command_engine->run(gcnew animation_viewer_move_nodes(this, lst, offset));
	}
}

void animation_viewer_hypergraph::hypergraph_Paint(System::Object^ , System::Windows::Forms::PaintEventArgs^ e)
{
	if(m_command_engine->saved_flag() && !m_owner->models_changed)
		m_owner->is_saved = true;
	else
		m_owner->is_saved = false;

	super::hypergraph_Paint(nullptr, e);
}

void animation_viewer_hypergraph::on_key_down(Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
{
	if(e->KeyCode!=Keys::Delete)
		super::on_key_down(sender, e);

	m_owner->get_editor()->on_editor_key_down(sender, e);
}

void animation_viewer_hypergraph::hypergraph_area_MouseDown(Object^ , System::Windows::Forms::MouseEventArgs^ e)
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
			save_clip_item->Enabled = false;
		}
		else
		{
			if(m_selected_nodes->Count!=0)
			{
				cut_menu_item->Enabled = true;
				copy_menu_item->Enabled = true;
				if(dynamic_cast<animation_viewer_graph_node_animation^>(m_selected_nodes[0]))
					save_clip_item->Enabled = true;
			}
			delete_menu_item->Enabled = true;
		}
	}

	super::hypergraph_area_MouseDown(nullptr, e);
}

void animation_viewer_hypergraph::hypergraph_area_MouseUp(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
		move_stop();

	super::hypergraph_area_MouseUp(nullptr, e);
}

void animation_viewer_hypergraph::on_hypergraph_scroll(Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Delta>0)
		change_scale(0.1f);
	else
		change_scale(-0.1f);

	display_rectangle = Drawing::Rectangle(display_rectangle.Location, Drawing::Size((int)(ClientRectangle.Width / this->scale), (int)(ClientRectangle.Height / this->scale)));
}

animation_viewer_hypergraph::links^ animation_viewer_hypergraph::copy_links()
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

void animation_viewer_hypergraph::save(xray::configs::lua_config_value cfg, nodes^ nds, links^ lnks)
{
//	saving nodes
	if(nds!=nullptr)
	{
		for each(animation_viewer_graph_node_base^ n in nds)
			n->save(cfg["nodes"][n->id]);
	}

//	saving links
	if(lnks!=nullptr)
	{
		u32 link_id = 0;
		for each(animation_viewer_graph_link^ lnk in lnks)
			lnk->save(cfg["links"][++link_id]);
	}
}

void animation_viewer_hypergraph::save(xray::configs::lua_config_value cfg)
{
	save(cfg, m_nodes, links_manager->visible_links());
}

void animation_viewer_hypergraph::set_saved_flag()
{
	m_command_engine->set_saved_flag();
	Invalidate(false);
}

void animation_viewer_hypergraph::clear_command_stack()
{
	m_command_engine->clear_stack();
	Invalidate(false);
}

void animation_viewer_hypergraph::add_group(pcstr config_string, Point offset)
{
	m_owner->is_saved &= !m_command_engine->run(gcnew animation_viewer_add_group(this, config_string, offset));
}

void animation_viewer_hypergraph::remove_group(nodes^ nds, links^ lnks)
{
	m_owner->is_saved &= !m_command_engine->run(gcnew animation_viewer_remove_group(this, nds, lnks));
}

void animation_viewer_hypergraph::on_property_changed(wpf_property_container^ pc, System::String^ l, Object^ new_val, Object^ old_val)
{
	m_owner->is_saved &= !m_command_engine->run(gcnew animation_viewer_change_property(this, pc, l, new_val, old_val));
}

u32 animation_viewer_hypergraph::assign_node_id(xray::animation_editor::animation_viewer_graph_node_base^ n, u32 new_id)
{
	u32 id = 0;
	if(new_id==u32(-1) || m_nodes_ids->Contains(new_id))
	{	
		while(m_nodes_ids->Contains(id))
			++id;
	}
	else
		id = new_id;

	n->id = id;
	m_nodes_ids->Add(id);
	return id;
}

void animation_viewer_hypergraph::remove_node_id(xray::animation_editor::animation_viewer_graph_node_base^ n)
{
	m_nodes_ids->Remove(n->id);
}

bool animation_viewer_hypergraph::can_make_connection(animation_viewer_graph_node_base^ src, animation_viewer_graph_node_base^ dst)
{
	if(src->parent==dst)
		return false; // already connected;

	animation_viewer_graph_node_animation^ an1 = dynamic_cast<animation_viewer_graph_node_animation^>(src);
	animation_viewer_graph_node_animation^ an2 = dynamic_cast<animation_viewer_graph_node_animation^>(dst);
	if(an1 && an2)
	{
		if(!an1->animation_clip || !an2->animation_clip)
			return false; // if one of animation nodes has no animation clip;

		for each(animation_viewer_graph_link^ lnk in links_manager->visible_links())
		{
			if(dynamic_cast<animation_viewer_graph_node_animation^>(lnk->destination_node) &&
				an1==dynamic_cast<animation_viewer_graph_node_animation^>(lnk->source_node))
				return false; // if source animation is synchronized with other animation;
		}

		if(an2->synchronized_with_node(an1))
			return false; // if source animation is synchronized with destination animation through other animation;

		if(an1->synchronization_group()!=an2->synchronization_group())
			return false; // if animations are in different synchronization groups ;

		if(an1->root()!=an2->root())
			return false; // if animations have different root targets;
	}

	if(!an1 && an2)
		return false; // if we're trying to connect operator or root to animation;

	animation_viewer_graph_node_root^ root1 = dynamic_cast<animation_viewer_graph_node_root^>(src);
	animation_viewer_graph_node_root^ root2 = dynamic_cast<animation_viewer_graph_node_root^>(dst);
	if(root1 && !root2)
		return false; // if we're trying to connect root to operator or animation;


	if(!root1 && root2 && root2->childs->Count>0)
		return false; // if we're trying to connect root which already has a child node;

	return true;//super::can_make_connection(src_pt, dst_pt);	
}

xray::animation_editor::animation_viewer_graph_node_base^ animation_viewer_hypergraph::find_node_by_id(nodes^ nds, u32 id)
{
	for each(animation_viewer_graph_node_base^ n in nds)
	{
		if(id==n->id)
			return n;
	}
	return nullptr;
}

animation_viewer_graph_link^ animation_viewer_hypergraph::find_link_by_ids(links^ lnks, u32 id1, u32 id2)
{
	for each(animation_viewer_graph_link^ lnk in lnks)
	{
		animation_viewer_graph_node_base^ n1 = safe_cast<animation_viewer_graph_node_base^>(lnk->source_node);
		animation_viewer_graph_node_base^ n2 = safe_cast<animation_viewer_graph_node_base^>(lnk->destination_node);
		if(id1==n1->id && id2==n2->id)
			return lnk;
	}
	return nullptr;
}

animation_viewer_graph_link^ animation_viewer_hypergraph::create_link(animation_viewer_graph_node_base^ src_node, animation_viewer_graph_node_base^ dst_node)
{
	R_ASSERT(src_node && dst_node);
	xray::editor::controls::hypergraph::connection_point^ src_pt;
	xray::editor::controls::hypergraph::connection_point^ dst_pt;

	animation_viewer_graph_node_root^ root1 = dynamic_cast<animation_viewer_graph_node_root^>(src_node);
	animation_viewer_graph_node_root^ root2 = dynamic_cast<animation_viewer_graph_node_root^>(dst_node);
	if(root1 && root2)
	{
		src_pt = src_node->get_connection_point("trans_child");
		dst_pt = dst_node->get_connection_point("trans_parent");
		if(src_pt==nullptr)
			src_pt = src_node->add_connection_point(animation_viewer_graph_node_base::typeid, "trans_child");

		if(dst_pt==nullptr)
			dst_pt = dst_node->add_connection_point(animation_viewer_graph_node_base::typeid, "trans_parent");

		return (gcnew animation_viewer_graph_link_timed(src_pt, dst_pt));
	}

	animation_viewer_graph_node_animation^ an1 = dynamic_cast<animation_viewer_graph_node_animation^>(src_node);
	animation_viewer_graph_node_animation^ an2 = dynamic_cast<animation_viewer_graph_node_animation^>(dst_node);
	if(an1 && an2)
	{
		src_pt = src_node->get_connection_point("sync_child");
		dst_pt = dst_node->get_connection_point("sync_parent");
		if(src_pt==nullptr)
			src_pt = src_node->add_connection_point(animation_viewer_graph_node_base::typeid, "sync_child");

		if(dst_pt==nullptr)
			dst_pt = dst_node->add_connection_point(animation_viewer_graph_node_base::typeid, "sync_parent");

		an1->init_properties(an2);
		return (gcnew animation_viewer_graph_link(src_pt, dst_pt));
	}

	src_pt = src_node->get_connection_point("child");
	dst_pt = dst_node->get_connection_point("parent");
	if(src_pt==nullptr)
		src_pt = src_node->add_connection_point(animation_viewer_graph_node_base::typeid, "child");

	if(dst_pt==nullptr)
		dst_pt = dst_node->add_connection_point(animation_viewer_graph_node_base::typeid, "parent");

	if(src_node->parent==nullptr)
		src_node->parent = dst_node;

	if(!dst_node->childs->Contains(src_node))
		dst_node->childs->Add(src_node);

	animation_viewer_graph_node_operator^ oper = dynamic_cast<animation_viewer_graph_node_operator^>(dst_node);
	if(oper)
		return (gcnew animation_viewer_graph_link_weighted(src_pt, dst_pt));
	else
		return (gcnew animation_viewer_graph_link(src_pt, dst_pt));
}

xray::animation_editor::viewer_editor^ animation_viewer_hypergraph::get_editor() 
{
	return m_owner->get_editor();
}

void animation_viewer_hypergraph::set_animation_node_new_intervals_list(animation_viewer_graph_node_base^ n, List<xray::animation_editor::animation_node_interval^>^ lst, u32 cycle_index)
{
	m_owner->is_saved &= !m_command_engine->run(gcnew animation_viewer_change_lexeme(this, n, lst, cycle_index));
}

void animation_viewer_hypergraph::save_clip(System::Object^ , System::EventArgs^)
{
	m_owner->save_node_as_clip();
}

command_engine^ animation_viewer_hypergraph::get_command_engine ( )
{
	return m_command_engine;
}