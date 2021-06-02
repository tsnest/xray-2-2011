////////////////////////////////////////////////////////////////////////////
//	Created		: 16.12.2009
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_editor.h"
#include "dialog_editor_resources_cooker.h"
#include "dialog_cooker.h"
#include "game_dialog_cooker.h"
#include "string_table_ids_storage.h"
#include "dialog_document.h"
#include "string_table_id_options.h"
#include "dialog_language_panel.h"
#include "string_table_id.h"
#include "dialog_text_editor.h"
#include "dialogs_manager.h"
#include "dialog_item_properties_panel.h"
#include "dialog_editor_form.h"
#include <xray/editor/world/world.h>

using xray::dialog_editor::dialog_editor_impl;
using namespace xray::dialog_editor::string_tables;

dialog_editor_impl::dialog_editor_impl(System::String^ rp, xray::editor_base::tool_window_holder^ holder )
:m_holder		( holder ),
m_closed		( false )
{
	XRAY_UNREFERENCED_PARAMETER(rp);
	m_name						= "dialog_editor";
	loc_file_loaded				= false;
	ref_file_loaded				= false;
	resources_path				= "resources/dialogs/";
	m_dialog_resources_cooker	= NEW(dialog_editor_resources_cooker)();
	m_dialog_cooker				= NEW(dialog_cooker)();
	m_game_dialog_cooker		= NEW(game_dialog_cooker)();

	m_references_table			= NEW(references_table)();
	m_last_translators_ids		= gcnew last_translators_ids_type();
	creating_new_document		= false;

	initialize					( );
	m_holder->register_tool_window ( this );

	query_result_delegate* rq = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &dialog_editor_impl::on_references_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/dialogs/texts/references.cfg",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq, _1),
		g_allocator
		);

	query_result_delegate* rq2 = NEW(query_result_delegate)(gcnew query_result_delegate::Delegate(this, &dialog_editor_impl::on_languages_list_loaded), g_allocator);
	xray::resources::query_resource(
		"resources/dialogs/texts/localizations.cfg",
		xray::resources::lua_config_class,
		boost::bind(&query_result_delegate::callback, rq2, _1),
		g_allocator);


}

void dialog_editor_impl::close_internal( )
{
	if(m_closed)
		return;

	save_settings		( base_registry_key::get() );
	clear_resources		( );
	m_closed			= true;
	m_holder->unregister_tool_window ( this );
	delete				this;
}

dialog_editor_impl::~dialog_editor_impl( )
{
}

void dialog_editor_impl::initialize( )
{
	create_string_tables();
	dialog_manager::create_dialogs_manager();

	resources::register_cook(m_dialog_resources_cooker);
	resources::register_cook(m_dialog_cooker);
	resources::register_cook(m_game_dialog_cooker);

	initialize_components();
}

void dialog_editor_impl::clear_resources( )
{
	destroy_string_tables();
	dialog_manager::destroy_dialogs_manager();

	resources::unregister_cook(resources::game_dialog_class);
	resources::unregister_cook(resources::dialog_class);
	resources::unregister_cook(resources::dialog_resources_class);
	DELETE(m_game_dialog_cooker);
	DELETE(m_dialog_cooker);
	DELETE(m_dialog_resources_cooker);

	references_table::iterator it = m_references_table->begin();
	for(; it!=m_references_table->end(); ++it)
	{
		FREE(it->first);
		DELETE(it->second);
	}

	DELETE(m_references_table);

	destroy_components( );
}

bool dialog_editor_impl::close_query()
{
	return save_confirmation();
}

bool dialog_editor_impl::save_confirmation()
{
	m_properties_panel->show_properties(nullptr);
	m_form->multidocument_base->save_panels(m_form);

	u32 unsaved_docs_counter = 0;
	System::String^ txt = "";
	for each(dialog_document^ doc in m_form->multidocument_base->opened_documents)
	{
		if(!(doc->is_saved))
		{
			txt += doc->Name+".dlg\n";
			++unsaved_docs_counter;
		}
	}

	if(unsaved_docs_counter==0)
	{
		System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
		lst->AddRange(m_form->multidocument_base->opened_documents);
		for each(dialog_document^ doc in lst)
			doc->Close();

		save_options();
		save_string_table();
		return true;
	}

	::DialogResult res = MessageBox::Show(m_form, "Save changes to the following items?\n\n"+txt, 
		m_form->Text, MessageBoxButtons::YesNoCancel, MessageBoxIcon::Question);

	if(res== ::DialogResult::Cancel)
		return false;
	else if(res== ::DialogResult::Yes)
	{
		save_options();
		save_string_table();
		for each(dialog_document^ doc in m_form->multidocument_base->opened_documents)
		{
			if(!(doc->is_saved))
				doc->save();
		}
	}

	System::Collections::Generic::List<xray::editor::controls::document_base^ >^ lst = gcnew System::Collections::Generic::List<xray::editor::controls::document_base^>();
	lst->AddRange(m_form->multidocument_base->opened_documents);
	for each(dialog_document^ doc in lst)
	{
		doc->is_saved = true;
		doc->Close();
	}
	m_form->multidocument_base->active_document = nullptr;

	return true;
}

System::Windows::Forms::Form^ dialog_editor_impl::main_form( ) 
{ 
	return m_form; 
}

void dialog_editor_impl::tick()
{
	m_input_engine->execute( );
	dialog_manager::get_dialogs_manager()->tick( );
}

void dialog_editor_impl::on_references_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());	

	if(!data.is_successful())
		return;

	configs::lua_config_ptr config = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
	// loading references
	configs::lua_config::const_iterator b = config->get_root()["references"].begin();
	for(; b!=config->get_root()["references"].end(); ++b) 
	{
		pstr str_id = strings::duplicate(g_allocator, b.key());
		string_table_id_options* str_opt = NEW(string_table_id_options)();
		str_opt->load(*b);
		m_references_table->insert(std::pair<pstr, string_table_id_options*>(str_id, str_opt));
	}

	b = config->get_root()["last_translators_id"].begin();
	for(; b!=config->get_root()["last_translators_id"].end(); ++b) 
		m_last_translators_ids->Add(gcnew System::String(b.key()), u32(*b));

	ref_file_loaded = true;
}

void dialog_editor_impl::on_languages_list_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());	

	if(!data.is_successful())
		return;

	if(data.size() > 0)
	{
		configs::lua_config_ptr config = static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
		if(!config->get_root().value_exists("localizations") || 
			!config->get_root()["localizations"].value_exists("all") ||
			!config->get_root()["localizations"].value_exists("current"))
		{
			MessageBox::Show(m_form, "Wrong [localizations.cfg] file format!!!", 
				m_form->Text, MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		if(!config->get_root()["localizations"]["all"].size())
		{
			MessageBox::Show(m_form, "Localization languages are missing. Please check file [localizations.cfg] or add new language from menu", 
				m_form->Text, MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}

		configs::lua_config::const_iterator b = config->get_root()["localizations"]["all"].begin();
		System::String^ cur_lang_name = gcnew System::String(config->get_root()["localizations"]["current"]);
		for(; b!=config->get_root()["localizations"]["all"].end(); ++b)
		{
			System::String^ lang_name = gcnew System::String(*b);
			m_language_panel->add_new_language(lang_name, lang_name==cur_lang_name);
		}

		loc_file_loaded = true;
	}
}

void dialog_editor_impl::save_string_table()
{
	String^ fn = (resources_path+"texts/localizations.cfg");
	configs::lua_config_ptr const& config = configs::create_lua_config(unmanaged_string(fn).c_str());
	configs::lua_config_value config_root = config->get_root()["localizations"];
	config_root["current"] = get_string_tables()->cur_language_name();
	u8 counter = 0;
	string_table_ids_storage::localization_languages_type::const_iterator it = get_string_tables()->languages_list()->begin();
	for(; it!=get_string_tables()->languages_list()->end(); ++it)
	{
		config_root["all"][counter] = *it;
		++counter;
		String^ file_name = (resources_path+"texts/"+gcnew String(*it)+".localization");
		configs::lua_config_ptr const& cfg = configs::create_lua_config(unmanaged_string(file_name).c_str());
		configs::lua_config_value root = cfg->get_root()["string_table_ids"];
		string_table_ids_storage::string_table_ids_type::const_iterator str_it = get_string_tables()->string_table_ids()->begin();
		for(; str_it!=get_string_tables()->string_table_ids()->end(); ++str_it)
		{
			pcstr key = str_it->first;
			references_table::iterator opt_it = m_references_table->find(key);
			R_ASSERT(opt_it!=m_references_table->end());
			if(opt_it->second->references_count()==0 && str_it->second->check_on_empty_text())
				continue;

			root[key] = str_it->second->text_by_loc_name(*it);
		}

		cfg->save(configs::target_sources);
	}
	config->save(configs::target_sources);
}

void dialog_editor_impl::save_options()
{
	System::String^ source_path = (resources_path+"texts/references.cfg");
	configs::lua_config_ptr const& cfg = configs::create_lua_config(unmanaged_string(source_path).c_str());

	xray::configs::lua_config_value root = cfg->get_root()["references"];
	references_table::iterator it = m_references_table->begin();
	for(; it!=m_references_table->end(); ++it)
	{
		pcstr key = it->first;
		u32 rc = it->second->references_count();
		if(rc==0 && get_string_tables()->check_id_on_empty_text(key))
			continue;

		it->second->save(root[key]);
	}

	root = cfg->get_root()["last_translators_id"];
	for each(Collections::Generic::KeyValuePair<String^, u32>^ kvp in m_last_translators_ids)
	{
		unmanaged_string str = unmanaged_string(kvp->Key);
		root[(pcstr)str.c_str()] = kvp->Value;
	}

	cfg->save(configs::target_sources);
}

void dialog_editor_impl::save_all(System::Object^ , System::EventArgs^ )
{
	save_string_table();
	save_options();
}

void dialog_editor_impl::save_active(System::Object^ , System::EventArgs^ )
{
	save_string_table();
	save_options();
}

void dialog_editor_impl::add_new_id(System::String^ id, System::String^ txt)
{
	unmanaged_string unm_str_id = unmanaged_string(id);
	get_string_tables()->add_new_id(unm_str_id.c_str(), unmanaged_string(txt).c_str());
	pstr str_id = strings::duplicate(g_allocator, unm_str_id.c_str());
	string_table_id_options* str_opt = NEW(string_table_id_options)();
	str_opt->set_category(string_table_id_options::dialogs);
	string_table_ids_storage::localization_languages_type::const_iterator it = get_string_tables()->languages_list()->begin();
	for(; it!=get_string_tables()->languages_list()->end(); ++it)
		str_opt->add_batch_file_name_for_localization(*it);

	m_references_table->insert(std::pair<pstr, string_table_id_options*>(str_id, str_opt));
	LOGI_INFO("dialog_editor_impl", "added new id [%s]", str_id);
}

bool dialog_editor_impl::change_ids_text(System::String^ id, System::String^ txt, bool use_message)
{
	references_table::iterator it = m_references_table->find(unmanaged_string(id).c_str());
	R_ASSERT(it!=m_references_table->end());
	if(use_message && it->second->references_count()>1)
	{
		::DialogResult res = MessageBox::Show(m_form, "String table id ["+id+"] is used in more than one node!", 
			m_form->Text, MessageBoxButtons::OKCancel, MessageBoxIcon::Warning);

		if(res == ::DialogResult::Cancel)
			return false;
	}

	get_string_tables()->change_ids_text(unmanaged_string(id).c_str(), unmanaged_string(txt).c_str());
	it->second->clear_batch_file_names();
	update_documents_text();
	return true;
}

u32 dialog_editor_impl::id_category(System::String^ id)
{
	unmanaged_string str(id);
	references_table::iterator it = m_references_table->find(str.c_str());
	R_ASSERT(it!=m_references_table->end(), "dialog_editor_impl::id_category. [%s] id is not found", id);
	return it->second->category();
}

void dialog_editor_impl::set_id_category(System::String^ id, u32 new_cat)
{
	references_table::iterator it = m_references_table->find(unmanaged_string(id).c_str());
	R_ASSERT(it!=m_references_table->end());
	it->second->set_category(new_cat);
}

System::String^ dialog_editor_impl::get_text_by_id(System::String^ id)
{
	pcstr txt = get_string_tables()->get_text_by_id(unmanaged_string(id).c_str());
	if(txt!=NULL)
		return gcnew String(txt);
	
	return nullptr;
}

void dialog_editor_impl::change_references_count(System::String^ id1, System::String^ id2)
{
	if(id1!=nullptr)
	{
		unmanaged_string str_id = unmanaged_string(id1);
		references_table::iterator it = m_references_table->find(str_id.c_str());
		if(it==m_references_table->end())
		{
			add_new_id(id1, "");
			it = m_references_table->find(str_id.c_str());
			R_ASSERT(it!=m_references_table->end());
		}

		u32 rc = it->second->references_count();
		it->second->set_references_count(rc+1);
	}

	if(id2!=nullptr)
	{
		unmanaged_string str_id = unmanaged_string(id2);
		references_table::iterator it = m_references_table->find(str_id.c_str());
		if(it==m_references_table->end())
		{
			add_new_id(id2, "");
			it = m_references_table->find(str_id.c_str());
			R_ASSERT(it!=m_references_table->end());
		}

		u32 rc = it->second->references_count();
		it->second->set_references_count(rc-1);
	}
}

void dialog_editor_impl::set_cur_language(System::String^ lang)
{
	get_string_tables()->set_cur_language_name(unmanaged_string(lang).c_str());
	m_text_editor->assign_string_table_ids();
	update_documents_text();
}

void dialog_editor_impl::add_language(System::String^ lang_name)
{
	Int32 d = 0;
	if(Int32::TryParse(lang_name, d))
		MessageBox::Show(m_form, "Language name must be a string!", m_form->Text, MessageBoxButtons::OK, MessageBoxIcon::Error);
	else
	{
		loc_file_loaded = true;
		unmanaged_string ln = unmanaged_string(lang_name);
		get_string_tables()->new_language(ln.c_str());
		if(m_last_translators_ids->ContainsKey(lang_name))
		{
			m_language_panel->refresh_languages();
			return;
		}

		m_last_translators_ids->Add(lang_name, 0);
		references_table::iterator it = m_references_table->begin();
		for(; it!=m_references_table->end(); ++it)
			it->second->add_batch_file_name_for_localization(ln.c_str());

		m_language_panel->refresh_languages();
	}
}

void dialog_editor_impl::remove_language(System::String^ lang_name)
{
	unmanaged_string ln = unmanaged_string(lang_name);
	get_string_tables()->remove_language(ln.c_str());
	m_last_translators_ids->Remove(lang_name);
	references_table::iterator it = m_references_table->begin();
	for(; it!=m_references_table->end(); ++it)
		it->second->remove_batch_file_name_for_localization(ln.c_str());

	m_language_panel->refresh_languages();
}

u32 dialog_editor_impl::last_translators_id(System::String^ lang_name)
{
	if(m_last_translators_ids->ContainsKey(lang_name))
		return m_last_translators_ids[lang_name];
	else
		return u32(-1);
}

void dialog_editor_impl::set_last_translators_id(System::String^ lang_name, u32 new_last_id)
{
	m_last_translators_ids[lang_name] = new_last_id;
}

u32 dialog_editor_impl::references_count(System::String^ id)
{
	unmanaged_string str_id = unmanaged_string(id);
	references_table::iterator it = m_references_table->find(str_id.c_str());
	R_ASSERT(it!=m_references_table->end());
	return it->second->references_count();
}

void dialog_editor_impl::set_batch_file_name(System::String^ id, System::String^ lang_name, System::String^ new_file_name)
{
	unmanaged_string str_id = unmanaged_string(id);
	references_table::iterator it = m_references_table->find(str_id.c_str());
	R_ASSERT(it!=m_references_table->end());
	it->second->set_batch_file_name(unmanaged_string(lang_name).c_str(), unmanaged_string(new_file_name).c_str());
}

bool dialog_editor_impl::is_batch_file_name_empty(System::String^ id, System::String^ lang_name)
{
	unmanaged_string str_id = unmanaged_string(id);
	references_table::iterator it = m_references_table->find(str_id.c_str());
	R_ASSERT(it!=m_references_table->end());
	pcstr bfn = it->second->batch_file_name(unmanaged_string(lang_name).c_str());
	if(bfn==NULL || strings::compare(bfn, "")==0)
		return true;

	return false;
}

String^ dialog_editor_impl::batch_file_name(System::String^ id, System::String^ lang_name)
{
	unmanaged_string str_id = unmanaged_string(id);
	references_table::iterator it = m_references_table->find(str_id.c_str());
	R_ASSERT(it!=m_references_table->end());
	pcstr bfn = it->second->batch_file_name(unmanaged_string(lang_name).c_str());
	return gcnew System::String(bfn);
}

void	dialog_editor_impl::show_properties	(Object^ obj)
{
	m_properties_panel->show_properties(obj);
}