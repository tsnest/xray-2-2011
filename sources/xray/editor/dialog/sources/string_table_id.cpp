////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "string_table_id.h"
#include "string_table_ids_storage.h"

#pragma unmanaged
using xray::dialog_editor::string_tables::string_table_id;
using xray::dialog_editor::string_tables::string_table_ids_storage;

string_table_id::string_table_id()
{
	m_localizations = NEW(localizations_type)();
}

string_table_id::~string_table_id()
{
	localizations_type::const_iterator it = m_localizations->begin();
	for(; it!=m_localizations->end(); ++it)
		FREE(it->second);

	DELETE(m_localizations);
}

void string_table_id::new_text(pcstr loc_name, pcstr new_txt)
{
	R_ASSERT(m_localizations->find(loc_name)==m_localizations->end());
	m_localizations->insert(std::pair<pcstr, pstr>(loc_name, strings::duplicate(g_allocator, new_txt)));
}

void string_table_id::change_text(pcstr new_txt)
{
	pcstr cur_lang = string_tables::get_string_tables()->cur_language_name();
	localizations_type::iterator it = m_localizations->find(cur_lang);
	if(it==m_localizations->end())
		return new_text(cur_lang, new_txt);

	FREE(it->second);
	it->second = strings::duplicate(g_allocator, new_txt);
}

void string_table_id::remove_text(pcstr loc_name)
{
	localizations_type::iterator it = m_localizations->find(loc_name);
	if(it==m_localizations->end())
		return;

	FREE(it->second);
	m_localizations->erase(it);
}

pcstr string_table_id::text()
{
	pcstr cur_lang = string_tables::get_string_tables()->cur_language_name();
	localizations_type::iterator it = m_localizations->find(cur_lang);
	R_ASSERT(it!=m_localizations->end());
	return it->second;
}

pcstr string_table_id::text_by_loc_name(pcstr loc_name)
{
	localizations_type::iterator it = m_localizations->find(loc_name);
	R_ASSERT(it!=m_localizations->end());
	return it->second;
}

bool string_table_id::check_on_empty_text()
{
	localizations_type::const_iterator it = m_localizations->begin();
	for(; it!=m_localizations->end(); ++it)
	{
		if(strings::compare(it->second, "")!=0)
			return false;
	}	

	return true;
}
