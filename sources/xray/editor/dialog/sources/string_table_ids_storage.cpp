////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "string_table_ids_storage.h"
#include "string_table_id.h"

#pragma unmanaged
using xray::dialog_editor::string_tables::string_table_ids_storage;
using xray::dialog_editor::string_tables::string_table_id;
////////////////////////////////////////////////////////////////////////////
//- global functions ------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
static xray::uninitialized_reference<string_table_ids_storage> g_string_table_id_storage;
string_table_ids_storage* xray::dialog_editor::string_tables::create_string_tables()
{
	XRAY_CONSTRUCT_REFERENCE(g_string_table_id_storage, string_table_ids_storage)();
	return (&*g_string_table_id_storage);
}

void xray::dialog_editor::string_tables::destroy_string_tables()
{
	XRAY_DESTROY_REFERENCE(g_string_table_id_storage);
}

string_table_ids_storage* xray::dialog_editor::string_tables::get_string_tables()
{
	return (&*g_string_table_id_storage);
}
////////////////////////////////////////////////////////////////////////////
//- class string_table_ids_storage ----------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
string_table_ids_storage::string_table_ids_storage()
{
	m_string_table_ids = NEW(string_table_ids_type)();
	m_languages = NEW(localization_languages_type)();
	m_cur_language = NULL;
	xray::resources::query_resource(
		"resources/dialogs/texts/localizations.cfg",
		xray::resources::lua_config_class,
		boost::bind(&string_table_ids_storage::on_loc_table_loaded, this, _1),
		g_allocator);
}

string_table_ids_storage::~string_table_ids_storage()
{
	FREE(m_cur_language);
	string_table_ids_type::const_iterator it_str_id = m_string_table_ids->begin();
	for(; it_str_id!=m_string_table_ids->end(); ++it_str_id)
	{
		FREE(it_str_id->first);
		DELETE(it_str_id->second);
	}

	DELETE(m_string_table_ids);
	localization_languages_type::const_iterator it_lang = m_languages->begin();
	for(; it_lang!=m_languages->end(); ++it_lang)
		FREE(*it_lang);

	DELETE(m_languages);
}

void string_table_ids_storage::on_loc_table_loaded(xray::resources::queries_result& data)
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
			return;
		}

		m_cur_language = strings::duplicate(g_allocator, config->get_root()["localizations"]["current"]);
		configs::lua_config::const_iterator b = config->get_root()["localizations"]["all"].begin();
		for(; b!=config->get_root()["localizations"]["all"].end(); ++b)
			m_languages->insert(strings::duplicate(g_allocator, pcstr(*b)));

		// loading localizations
		xray::resources::query_resources_by_mask(
			"resources/dialogs/texts/*.localization",
			xray::resources::lua_config_class,
			boost::bind(&string_table_ids_storage::on_string_table_ids_loaded, this, _1), 
			g_allocator); 
	}
}

void string_table_ids_storage::on_string_table_ids_loaded(xray::resources::queries_result& data)
{
	R_ASSERT(data.is_successful());	

	if(!data.is_successful())
		return;

	if(data.size() > 0)
	{
		// loading string_table_ids
		for(u16 i=0; i<data.size(); ++i)
		{
			CURE_ASSERT(data[i].is_successful(), return);	

			xray::fs_new::virtual_path_string file_path;
			xray::fs_new::file_name_with_no_extension_from_path(&file_path, data[i].get_requested_path());
			pcstr loc_name = file_path.c_str();

			localization_languages_type::iterator loc_it = m_languages->find(loc_name);
			if(loc_it==m_languages->end())
				continue;

			configs::lua_config_ptr config = static_cast_checked<configs::lua_config*>(data[i].get_unmanaged_resource().c_ptr());
			configs::lua_config::const_iterator	b = config->get_root()["string_table_ids"].begin();
			for(; b!=config->get_root()["string_table_ids"].end(); ++b)
			{
				string_table_ids_type::iterator it_str_id = m_string_table_ids->find(b.key());
				if(it_str_id==m_string_table_ids->end())
				{
					string_table_id* new_id = NEW(string_table_id)();
					it_str_id = m_string_table_ids->insert(std::pair<pstr, string_table_id*>(strings::duplicate(g_allocator, b.key()), new_id)).first;
				}
				it_str_id->second->new_text(*loc_it, pcstr(*b));
			}
		}
	}
}

bool string_table_ids_storage::check_id_on_empty_text(pcstr id)
{
	string_table_ids_type::iterator it_str_id = m_string_table_ids->find(id);
	if(it_str_id!=m_string_table_ids->end())
		return it_str_id->second->check_on_empty_text();

	return true;
}

void string_table_ids_storage::add_new_id(pcstr new_id, pcstr new_text)
{
	string_table_ids_type::iterator it_str_id = m_string_table_ids->find(new_id);
	if(it_str_id!=m_string_table_ids->end())
		return;

	
	string_table_id* new_str_id = NEW(string_table_id)();
	m_string_table_ids->insert(std::pair<pstr, string_table_id*>(strings::duplicate(g_allocator, new_id), new_str_id));
	
	localization_languages_type::iterator it_lang = m_languages->begin();
	for(; it_lang!=m_languages->end(); ++it_lang)
	{
		if(strings::compare(*it_lang, m_cur_language)==0)
			new_str_id->new_text(*it_lang, new_text);
		else
			new_str_id->new_text(*it_lang, "");
	}
}

void string_table_ids_storage::change_ids_text(pcstr id, pcstr new_text)
{
	string_table_ids_type::iterator it_str_id = m_string_table_ids->find(id);
	if(it_str_id==m_string_table_ids->end())
		return add_new_id(id, new_text);

	it_str_id->second->change_text(new_text);
}

pcstr string_table_ids_storage::get_text_by_id(pcstr str_id)
{
	if(str_id==NULL)
		return NULL;

	string_table_ids_type::iterator it_str_id = m_string_table_ids->find(str_id);
	if(it_str_id==m_string_table_ids->end())
		return NULL;
	
	return it_str_id->second->text();
}

void string_table_ids_storage::set_cur_language_name(pcstr new_lang)
{
	if(m_cur_language)
		FREE(m_cur_language);

	m_cur_language = strings::duplicate(g_allocator, new_lang);
}

pcstr string_table_ids_storage::cur_language_name()
{
	return m_cur_language;
}

void string_table_ids_storage::new_language(pcstr lang_name)
{
	localization_languages_type::iterator it_lang = m_languages->insert(strings::duplicate(g_allocator, lang_name)).first;
	string_table_ids_type::iterator it_str_id = m_string_table_ids->begin();
	for(; it_str_id!=m_string_table_ids->end(); ++it_str_id)
		it_str_id->second->new_text(*it_lang, "");

	set_cur_language_name(lang_name);
}

void string_table_ids_storage::remove_language(pcstr lang_name)
{
	localization_languages_type::iterator it_lang = m_languages->find(lang_name);
	string_table_ids_type::iterator it_str_id = m_string_table_ids->begin();
	for(; it_str_id!=m_string_table_ids->end(); ++it_str_id)
		it_str_id->second->remove_text(*it_lang);

	FREE(*it_lang);
	m_languages->erase(it_lang);
	set_cur_language_name(*(m_languages->begin()));

	fs_new::virtual_path_string file_path;
	file_path.append("resources/dialogs/texts/");
	file_path.append(lang_name);
	file_path.append(".localization");

	resources::query_erase_file("", file_path, NULL, g_allocator);
}
