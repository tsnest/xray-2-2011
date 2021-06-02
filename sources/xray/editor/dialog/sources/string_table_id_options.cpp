////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "string_table_id_options.h"
#include "string_table_ids_storage.h"

#pragma unmanaged
using xray::dialog_editor::string_tables::string_table_id_options;
using xray::dialog_editor::string_tables::string_table_ids_storage;

string_table_id_options::string_table_id_options()
{
	m_category = none;
	m_references_count = 0;
	m_batch_file_names = NEW(batch_file_names_type)();
}

string_table_id_options::~string_table_id_options()
{
	batch_file_names_type::iterator it = m_batch_file_names->begin();
	for(; it!=m_batch_file_names->end(); ++it)
	{
		FREE(it->first);
		FREE(it->second);
	}

	DELETE(m_batch_file_names);
}

void string_table_id_options::load(xray::configs::lua_config_value const& cfg)
{
	m_category = (e_string_table_category)(u32)cfg["category"];
	m_references_count = cfg["references_count"];
	xray::configs::lua_config_iterator it = cfg["batch_file_names"].begin();
	for(; it!=cfg["batch_file_names"].end(); ++it)
	{
		pstr loc_name = strings::duplicate(g_allocator, it.key());
		pstr fn = strings::duplicate(g_allocator, *it);
		m_batch_file_names->insert(std::pair<pstr, pstr>(loc_name, fn));
	}
}

void string_table_id_options::save(xray::configs::lua_config_value cfg)
{
	cfg["category"] = m_category;
	cfg["references_count"] = m_references_count;
	batch_file_names_type::iterator it = m_batch_file_names->begin();
	for(; it!=m_batch_file_names->end(); ++it)
	{
		pcstr key = it->first;
		pcstr val = it->second;
		if(val==NULL) 
			val = "";

		cfg["batch_file_names"][key] = val;
	}
}

void string_table_id_options::add_batch_file_name_for_localization(pcstr l_name)
{
	batch_file_names_type::iterator it = m_batch_file_names->find(l_name);
	R_ASSERT(it==m_batch_file_names->end());
	pstr loc_name = strings::duplicate(g_allocator, l_name);
	m_batch_file_names->insert(std::pair<pstr, pstr>(loc_name, strings::duplicate(g_allocator, "")));
}

void string_table_id_options::remove_batch_file_name_for_localization(pcstr l_name)
{
	batch_file_names_type::iterator it = m_batch_file_names->find(l_name);
	R_ASSERT(it!=m_batch_file_names->end());
	FREE(it->second);
	FREE(it->first);
	m_batch_file_names->erase(it);
}

void string_table_id_options::set_batch_file_name(pcstr loc_name, pcstr fn)
{
	batch_file_names_type::iterator it = m_batch_file_names->find(loc_name);
	R_ASSERT(it!=m_batch_file_names->end());
	FREE(it->second);
	it->second = strings::duplicate(g_allocator, fn);
}

pcstr string_table_id_options::batch_file_name(pcstr loc_name)
{
	batch_file_names_type::iterator it = m_batch_file_names->find(loc_name);
	R_ASSERT(it!=m_batch_file_names->end());
	return it->second;
}

void string_table_id_options::clear_batch_file_names()
{
	batch_file_names_type::iterator it = m_batch_file_names->begin();
	for(; it!=m_batch_file_names->end(); ++it)
	{
		FREE(it->second);
		it->second = strings::duplicate(g_allocator, "");
	}
}
