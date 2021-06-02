////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_phrase.h"
#include "string_table_ids_storage.h"

#pragma unmanaged
using xray::dialog_editor::dialog_phrase;
using xray::dialog_editor::string_tables::string_table_ids_storage;

u32 dialog_phrase::m_created_counter = 0;

dialog_phrase::dialog_phrase(pstr str_id, bool is_editor_phrase)
:m_str_id(str_id),
m_is_editor_phrase(is_editor_phrase)
{
	++m_created_counter;
}

dialog_phrase::~dialog_phrase()
{
	if(m_is_editor_phrase)
		FREE(m_str_id);

	--m_created_counter;
}

void dialog_phrase::set_string_table_id(pcstr new_str_id)
{
	FREE(m_str_id);
	m_str_id = strings::duplicate(g_allocator, new_str_id);
}

void dialog_phrase::set_text(pcstr new_text)
{
	XRAY_UNREFERENCED_PARAMETER(new_text);
}

pcstr dialog_phrase::text()
{
	return string_tables::get_string_tables()->get_text_by_id(m_str_id);
}
