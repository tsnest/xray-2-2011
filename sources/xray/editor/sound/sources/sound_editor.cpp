////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "sound_editor_contexts_manager.h"
#include "sound_document_editor_base.h"
#include "sound_editor.h"
#include "toolbar_panel.h"
#include "instance_properties_panel.h"
#include "object_properties_panel.h"
#include "tree_view_panel.h"
#include "sound_object_wrapper.h"
#include "single_sound_wrapper.h"
#include "composite_sound_wrapper.h"
#include "sound_collection_wrapper.h"
#include "sound_scene_document.h"
#include "sound_collection_document.h"
#include "composite_sound_document.h"
#include "apply_revert_form.h"
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>

using xray::sound_editor::sound_editor;
using xray::sound_editor::sound_object_wrapper;
using xray::sound_editor::single_sound_wrapper;
using xray::sound_editor::composite_sound_wrapper;
using xray::sound_editor::sound_collection_wrapper;
using xray::sound_editor::sound_object_type;
using namespace xray::sound;
using namespace System::Windows::Forms;

sound_editor::sound_editor(String^ res_path, xray::sound::world& sound_world, xray::render::world& render_world, tool_window_holder^ holder)
:m_sound_world(&sound_world),
m_holder(holder),
m_render_world(render_world),
m_state(editor_state::none)
{
	m_name = "sound_editor";
	sound_resources_path = "resources/sounds/";
	absolute_sound_resources_path = res_path+"/sources/sounds/";
	m_input_engine = gcnew input_engine(input_keys_holder::ref, gcnew sound_editor_contexts_manager(this));
  	m_gui_binder = gcnew gui_binder(m_input_engine);
	m_single_sounds = gcnew single_sounds_list();
	m_composite_sounds = gcnew composite_sounds_list();
	m_sound_collections = gcnew sound_collections_list();
	m_modified_sounds = gcnew modified_list();

	initialize();
	m_holder->register_tool_window(this);
}

sound_editor::~sound_editor()
{
}

void sound_editor::initialize()
{
	m_form = gcnew sound_editor_form(this);
	m_form->FormClosing += gcnew FormClosingEventHandler(this, &sound_editor::on_editor_closing);
	m_form->Activated += gcnew EventHandler(this, &sound_editor::on_form_activated);
	multidocument_base->content_reloading = gcnew content_reload_callback(this, &sound_editor::find_dock_content);

	m_toolbar_panel = gcnew toolbar_panel(this);
	m_instance_properties_panel = gcnew instance_properties_panel(this);
	m_object_properties_panel = gcnew object_properties_panel(this);
	m_tree_view_panel = gcnew tree_view_panel(this);
	bool loaded = multidocument_base->load_panels(m_form);
	if(!loaded)
	{
		m_toolbar_panel->Show(multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockBottom);
		m_object_properties_panel->Show(multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockRight);
		m_instance_properties_panel->Show(m_object_properties_panel->Pane, WeifenLuo::WinFormsUI::Docking::DockAlignment::Bottom, 0.5f);
		m_tree_view_panel->Show(multidocument_base->main_dock_panel, WeifenLuo::WinFormsUI::Docking::DockState::DockLeft);
	}

	action_delegate^ a = nullptr;
	a = gcnew action_delegate("Save active", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::save_active));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_save_scene));
	m_input_engine->register_action(a, "Control+S(Active_document)");

	a = gcnew action_delegate("Save all", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::save_all));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_save_all));
	m_input_engine->register_action(a, "");

	a = gcnew action_delegate("Open scene", gcnew execute_delegate_managed(multidocument_base, &sound_document_editor_base::open_scene));
	m_input_engine->register_action(a, "Control+O(sound_editor)");

	a = gcnew action_delegate("Undo", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::undo));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+Z(Active_document)");

	a = gcnew action_delegate("Redo", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::redo));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+Y(Active_document)");

	a = gcnew action_delegate("Cut", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::cut));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+X(Active_document)");

	a = gcnew action_delegate("Copy", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::copy));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+C(Active_document)");

	a = gcnew action_delegate("Paste", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::paste));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+V(Active_document)");

	a = gcnew action_delegate("Select all", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::select_all));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Control+A(Active_document)");

	a = gcnew action_delegate("Delete", gcnew execute_delegate_managed(multidocument_base, &document_editor_base::del));
	a->set_enabled(gcnew enabled_delegate_managed(this, &sound_editor::predicate_active_scene));
	m_input_engine->register_action(a, "Delete(Active_document)");

	image_loader loader;
	m_gui_binder->set_image_list("base", loader.get_shared_images16());
	m_gui_binder->bind(multidocument_base->saveToolStripMenuItem, "Save active");
	m_gui_binder->bind(multidocument_base->saveAllToolStripMenuItem, "Save all");
	m_gui_binder->bind(multidocument_base->m_menu_file_open, "Open scene");
}

void sound_editor::clear_resources()
{
	for each(KeyValuePair<String^, single_sound_wrapper^> p in m_single_sounds)
		delete p.Value;

	for each(KeyValuePair<String^, composite_sound_wrapper^> p in m_composite_sounds)
		delete p.Value;

	for each(KeyValuePair<String^, sound_collection_wrapper^> p in m_sound_collections)
		delete p.Value;

	delete m_single_sounds;
	delete m_composite_sounds;
	delete m_sound_collections;
	delete m_modified_sounds;
	delete m_input_engine;
	delete m_gui_binder;
	delete m_toolbar_panel;
	delete m_instance_properties_panel;
	delete m_object_properties_panel;
	delete m_tree_view_panel;
	delete m_form;
}

bool sound_editor::close_query()
{
	multidocument_base->save_panels(m_form);
	u32 unsaved_docs_counter = 0;
	System::String^ txt = "";
	if(m_modified_sounds->Count>0)
	{
		apply_revert_form^ frm = gcnew apply_revert_form(this);
		frm->fill_list_box_items(m_modified_sounds);
		if(frm->ShowDialog(m_form)==DialogResult::Cancel)
			return false;
	}

	for each(document_base^ doc in multidocument_base->opened_documents)
	{
		if(!(doc->is_saved))
		{
			txt += doc->Name+"\n";
			++unsaved_docs_counter;
		}
	}

	if(unsaved_docs_counter==0)
	{
		List<document_base^>^ lst = gcnew List<document_base^>();
		lst->AddRange(multidocument_base->opened_documents);
		for each(document_base^ doc in lst)
			doc->Close();

		return true;
	}

	::DialogResult res = MessageBox::Show(m_form, "Save changes to the following items?\n\n"+txt, 
		"Sound editor", MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question);

	if(res== ::DialogResult::Cancel)
		return false;
	else if(res== ::DialogResult::Yes)
	{
		for each(document_base^ doc in multidocument_base->opened_documents)
		{
			if(!(doc->is_saved))
				doc->save();
		}
	}

	List<document_base^>^ lst = gcnew List<document_base^>();
	lst->AddRange(multidocument_base->opened_documents);
	for each(document_base^ doc in lst)
	{
		doc->is_saved = true;
		doc->Close();
	}

	multidocument_base->active_document = nullptr;
	return true;
}

void sound_editor::Show(System::String^, String^)
{
	m_form->Show();
}

bool sound_editor::predicate_active_scene()
{
	if(multidocument_base->active_document==nullptr)
		return false;

	return true;
}

bool sound_editor::predicate_save_scene()
{
	if(multidocument_base->active_document==nullptr || multidocument_base->active_document->is_saved)
		return false;

	return true;
}

bool sound_editor::predicate_save_all()
{
	for each(document_base^ d in multidocument_base->opened_documents)
	{
		if(!d->is_saved)
			return true;
	}

	return false;
}

void sound_editor::on_editor_closing(Object^, FormClosingEventArgs^ e)
{
	e->Cancel = true;
	m_state = editor_state::closing;
	if(close_query())
		close_internal();
	else
		m_state = editor_state::none;
}

void sound_editor::close_internal()
{
	m_state	= editor_state::closed;
	clear_resources();
	m_holder->unregister_tool_window(this);
	delete this;
}

IDockContent^ sound_editor::find_dock_content(System::String^ persist_string)
{
	if(persist_string=="xray.sound_editor.toolbar_panel")
		return m_toolbar_panel;

	if(persist_string=="xray.sound_editor.instance_properties_panel")
		return m_instance_properties_panel;

	if(persist_string=="xray.sound_editor.object_properties_panel")
		return m_object_properties_panel;

	if(persist_string=="xray.sound_editor.tree_view_panel")
		return m_tree_view_panel;

	return(nullptr);
}

void sound_editor::tick()
{
	if(multidocument_base->active_document!=nullptr)
	{
		sound_scene_document^ ssdoc = dynamic_cast<sound_scene_document^>(multidocument_base->active_document);
		if(ssdoc)
			ssdoc->tick();
	}

	m_input_engine->execute();
	m_gui_binder->update_items();
}

void sound_editor::show_properties(Object^ obj, bool is_instance)
{
	if(is_instance)
		m_instance_properties_panel->show_properties(obj);
	else
	{
		m_object_properties_panel->show_properties(obj);
		m_toolbar_panel->show_rms(obj);
	}
}

void sound_editor::clear_properties()
{
	m_instance_properties_panel->show_properties(nullptr);
	m_object_properties_panel->show_properties(nullptr);
	m_toolbar_panel->show_rms(nullptr);
}

bool sound_editor::check_modified(ReadOnlyCollection<tree_node^>^ nodes)
{
	for each(tree_node^ n in nodes)
	{
		if(n->m_node_type==tree_node_type::single_item)
		{
			if(!m_modified_sounds->Contains(n->FullPath))
				return false;
		}
		else
		{
			if(!(n->m_image_index_collapsed==folder_closed_modified))
				return false;
		}
	}

	return true;
}

void sound_editor::apply_changes(List<String^>^ names)
{
	for each(String^ nm in names)
	{
		tree_node^ n = m_tree_view_panel->get_node(nm);
		R_ASSERT(n!=nullptr);
		if(n->m_node_type==tree_node_type::single_item)
		{
			if(m_modified_sounds->Contains(nm))
			{
				set_nodes_images(n, node_sound_file, folder_open, folder_closed);
				get_sound(nm, sound_object_type::single)->apply_changes(!(m_state == editor_state::closing));
				m_modified_sounds->Remove(nm);
			}
		}
		else
		{
			if(n->m_image_index_collapsed==folder_closed_modified)
			{
				List<String^>^ lst = gcnew List<String^>();
				for each(TreeNode^ child in n->nodes)
					lst->Add(child->FullPath);

				apply_changes(lst);
			}
		}
	}
}

void sound_editor::revert_changes(List<String^>^ names)
{
	for each(String^ nm in names)
	{
		tree_node^ n = m_tree_view_panel->get_node(nm);
		R_ASSERT(n!=nullptr);
		if(n->m_node_type==tree_node_type::single_item)
		{
			if(m_modified_sounds->Contains(nm))
			{
				set_nodes_images(n, node_sound_file, folder_open, folder_closed);
				get_sound(nm, sound_object_type::single)->revert_changes();
				m_modified_sounds->Remove(nm);
			}
		}
		else
		{
			if(n->m_image_index_collapsed==folder_closed_modified)
			{
				List<String^>^ lst = gcnew List<String^>();
				for each(TreeNode^ child in n->nodes)
					lst->Add(child->FullPath);

				revert_changes(lst);
			}
		}
	}
}

void sound_editor::set_nodes_images(tree_node^ node, images16x16 img, images16x16 fo_img, images16x16 fc_img)
{
	while(node!=nullptr)
	{
		if(node->m_node_type==tree_node_type::single_item)
		{
			node->m_image_index_collapsed = img;
			node->m_image_index_expanded = img;
			node->ImageIndex = img; 
			node->SelectedImageIndex = img; 
		}
		else
		{
			node->m_image_index_collapsed = fc_img;
			node->m_image_index_expanded = fo_img;
			node->ImageIndex = fo_img; 
			node->SelectedImageIndex = fo_img; 
		}

		node = node->Parent;
	}
}

void sound_editor::on_object_property_changed(Object^, value_changed_event_args^ e)
{
	using namespace xray::editor::controls;
	if(e->new_value->Equals(e->old_value))
		return;

	sound_collection_document^ scdoc = dynamic_cast<sound_collection_document^>(multidocument_base->active_document);
	if(scdoc)
		return scdoc->on_property_changed(nullptr, e);

	composite_sound_document^ csdoc = dynamic_cast<composite_sound_document^>(multidocument_base->active_document);
	if(csdoc)
		return csdoc->on_property_changed(nullptr, e);

	for each(tree_node^ node in m_tree_view_panel->get_selected_nodes())
	{
		set_nodes_images(node, node_resource_modified, folder_open_modified, folder_closed_modified);
		if(node->m_node_type==tree_node_type::single_item && !(m_modified_sounds->Contains(node->FullPath)))
			m_modified_sounds->Add(node->FullPath);
	}
}

void sound_editor::on_instance_property_changed(Object^, value_changed_event_args^ e)
{
	if(e->new_value->Equals(e->old_value))
		return;

	R_ASSERT(multidocument_base->active_document!=nullptr);
	sound_scene_document^ ssdoc = dynamic_cast<sound_scene_document^>(multidocument_base->active_document);
	if(ssdoc)
		ssdoc->on_property_changed(e->changed_property->name, e->old_value, e->new_value);
}

sound_object_wrapper^ sound_editor::get_sound(String^ sound_name, sound_object_type t)
{
	if(t==sound_object_type::single)
		return get_single_sound(sound_name);
	else if(t==sound_object_type::composite)
		return get_composite_sound(sound_name);
	else if(t==sound_object_type::collection)
		return get_sound_collection(sound_name);
	else
		UNREACHABLE_CODE( return nullptr );
}

single_sound_wrapper^ sound_editor::get_single_sound(String^ sound_name)
{
	if(m_single_sounds->ContainsKey(sound_name))
		return m_single_sounds[sound_name];

	Action<sound_object_wrapper^>^ opt_cb = gcnew Action<sound_object_wrapper^>(this, &sound_editor::options_loaded_callback);
	Action<sound_object_wrapper^>^ snd_cb = gcnew Action<sound_object_wrapper^>(this, &sound_editor::sound_loaded_callback);
	single_sound_wrapper^ w = gcnew single_sound_wrapper(sound_name);
	w->load(opt_cb, snd_cb);
	w->spl_curve->edit_completed += gcnew System::EventHandler(this, &sound_editor::on_single_sound_spl_curve_changed);
	m_single_sounds->Add(sound_name, w);
	return w;
}

composite_sound_wrapper^ sound_editor::get_composite_sound(String^ sound_name)
{
	if(m_composite_sounds->ContainsKey(sound_name))
		return m_composite_sounds[sound_name];

	Action<sound_object_wrapper^>^ opt_cb = gcnew Action<sound_object_wrapper^>(this, &sound_editor::options_loaded_callback);
	Action<sound_object_wrapper^>^ snd_cb = gcnew Action<sound_object_wrapper^>(this, &sound_editor::sound_loaded_callback);
	composite_sound_wrapper^ w = gcnew composite_sound_wrapper(sound_name);
	w->load(opt_cb, snd_cb);
	//w->load(nullptr, nullptr);
	m_composite_sounds->Add(sound_name, w);
	return w;
}

sound_collection_wrapper^ sound_editor::get_sound_collection(String^ sound_name)
{
	if(m_sound_collections->ContainsKey(sound_name))
		return m_sound_collections[sound_name];

	Action<sound_object_wrapper^>^ opt_cb = gcnew Action<sound_object_wrapper^>(this, &sound_editor::options_loaded_callback);
	Action<sound_object_wrapper^>^ snd_cb = gcnew Action<sound_object_wrapper^>(this, &sound_editor::sound_loaded_callback);
	sound_collection_wrapper^ w = gcnew sound_collection_wrapper(sound_name);
	w->load(opt_cb, snd_cb);
	//w->load(nullptr, nullptr);
	m_sound_collections->Add(sound_name, w);
	return w;
}

void sound_editor::options_loaded_callback(sound_object_wrapper^ w)
{
	if(m_tree_view_panel->is_selected_node(w->name))
		show_properties(w, false);
}

void sound_editor::sound_loaded_callback(sound_object_wrapper^ w)
{
	if(m_tree_view_panel->is_selected_node(w->name))
		show_properties(w, false);
}

void sound_editor::remove_sound(String^ sound_name, sound_object_type t)
{
	if(t==sound_object_type::single && m_single_sounds->ContainsKey(sound_name))
	{
		delete m_single_sounds[sound_name];
		m_single_sounds->Remove(sound_name);
	}
	else if(t==sound_object_type::composite && m_composite_sounds->ContainsKey(sound_name))
	{
		delete m_composite_sounds[sound_name];
		m_composite_sounds->Remove(sound_name);
	}
	else if(t==sound_object_type::collection && m_sound_collections->ContainsKey(sound_name))
	{
		delete m_sound_collections[sound_name];
		m_sound_collections->Remove(sound_name);
	}
}

xray::sound::calculation_type sound_editor::calculation::get()
{
	return m_sound_world->get_calculation_type();
}

void sound_editor::calculation::set(xray::sound::calculation_type new_val)
{
	m_sound_world->set_calculation_type(new_val);
}

void sound_editor::track_selected(List<String^>^ paths)
{
	m_tree_view_panel->track_active_items(paths);
}

void sound_editor::play_selected()
{
	if(multidocument_base->active_document!=nullptr)
	{
		sound_scene_document^ ssdoc = dynamic_cast<sound_scene_document^>(multidocument_base->active_document);
		if(ssdoc)
			ssdoc->play_selected();
	}
}

void sound_editor::pause_selected()
{
	if(multidocument_base->active_document!=nullptr)
	{
		sound_scene_document^ ssdoc = dynamic_cast<sound_scene_document^>(multidocument_base->active_document);
		if(ssdoc)
			ssdoc->pause_selected();
	}
}

void sound_editor::stop_selected()
{
	if(multidocument_base->active_document!=nullptr)
	{
		sound_scene_document^ ssdoc = dynamic_cast<sound_scene_document^>(multidocument_base->active_document);
		if(ssdoc)
			ssdoc->stop_selected();
	}
}

bool sound_editor::is_active_collection()
{
	if(multidocument_base->active_document==nullptr)
		return false;

	if(dynamic_cast<sound_collection_document^>(multidocument_base->active_document))
		return true;

	return false;
}

void sound_editor::add_items_to_collection(modified_list^ names, tree_node^ dest_node, sound_object_type t)
{
	if(names->Count==0)
		return;

	sound_collection_document^ scdoc = dynamic_cast<sound_collection_document^>(multidocument_base->active_document);
	composite_sound_document^ csdoc = dynamic_cast<composite_sound_document^>(multidocument_base->active_document);
	modified_list^ names_list = gcnew modified_list();
	for each(String^ nm in names)
	{
		tree_node^ n = m_tree_view_panel->get_node(nm);
		R_ASSERT(n!=nullptr);
		add_items_to_collection_impl(n, names_list);
	}

	if(scdoc)
		scdoc->add_items_to_collection(names_list, dest_node, t);
	else
	{
		R_ASSERT(csdoc!=nullptr);
		csdoc->add_items_to_collection(names_list, dest_node, t);
	}
}

void sound_editor::add_items_to_collection_impl(tree_node^ n, modified_list^ names_list)
{
	if(n->m_node_type==tree_node_type::single_item)
	{
		if(!names_list->Contains(n->FullPath))
			names_list->Add(n->FullPath);
	}
	else
	{
		for each(tree_node^ nd in n->nodes)
			add_items_to_collection_impl(nd, names_list);
	}
}

void sound_editor::on_single_sound_spl_curve_changed(Object^, EventArgs^)
{
	for each(tree_node^ node in m_tree_view_panel->get_selected_nodes())
	{
		set_nodes_images(node, node_resource_modified, folder_open_modified, folder_closed_modified);
		if(node->m_node_type==tree_node_type::single_item && !(m_modified_sounds->Contains(node->FullPath)))
			m_modified_sounds->Add(node->FullPath);
	}
}

void sound_editor::on_form_activated(Object^, EventArgs^)
{
	if(multidocument_base->active_document!=nullptr)
	{
		sound_scene_document^ ssdoc = dynamic_cast<sound_scene_document^>(multidocument_base->active_document);
		if(ssdoc)
		{
			get_sound_world()->get_editor_world_user().set_active_sound_scene(ssdoc->get_sound_scene(), 1000, 0);
			get_sound_world()->get_editor_world_user().set_listener_properties_interlocked(
				ssdoc->get_sound_scene(),
				ssdoc->viewport->get_inverted_view_matrix().c.xyz(),
				ssdoc->viewport->get_inverted_view_matrix().k.xyz(),
				ssdoc->viewport->get_inverted_view_matrix().j.xyz());
		}			
	}
}