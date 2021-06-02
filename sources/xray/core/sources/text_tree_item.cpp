////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/text_tree_item.h>

using xray::strings::text_tree_item;
text_tree_item::text_tree_item(xray::strings::allocator_type& allocator, pcstr value, bool is_page_breaker)
:m_allocator(allocator),
m_column_value(NULL),
m_is_page_breaker(is_page_breaker),
m_is_visible(true)
{
	if(value)
		m_column_value = strings::duplicate(m_allocator, value);
}

text_tree_item::~text_tree_item()
{
	clear();
}

void text_tree_item::set_name(pcstr s)
{
	if(m_column_value)
		XRAY_FREE_IMPL(m_allocator, m_column_value);

	if(s)
		m_column_value = strings::duplicate(m_allocator, s);
}

void text_tree_item::clear()
{
	text_tree_item_deleter deleter(m_allocator);
	m_sub_items.for_each(deleter);
	m_sub_items.clear();
	m_column_items.for_each(deleter);
	m_column_items.clear();
	if(m_column_value)
		XRAY_FREE_IMPL(m_allocator, m_column_value);
}

text_tree_item* text_tree_item::find(pcstr s)
{
	if(!xray::strings::compare(m_column_value, s))
		return this;

	for(text_tree_item* itm=m_sub_items.front(); itm; itm = m_sub_items.get_next_of_object(itm))
	{
		if(text_tree_item* found = itm->find(s))
			return found;
	}

	return NULL;
}

text_tree_item* text_tree_item::new_child(pcstr s, bool is_page_breaker)
{
	text_tree_item* itm = XRAY_NEW_IMPL(m_allocator, text_tree_item)(m_allocator, s, is_page_breaker);
	m_sub_items.push_back(itm);
	return itm;
}

void text_tree_item::add_column_impl(pcstr s)
{
	text_tree_column_item* itm = XRAY_NEW_IMPL(m_allocator, text_tree_column_item)();
	itm->value = strings::duplicate(m_allocator, s);
	m_column_items.push_back(itm);
}

void text_tree_item::break_page()
{
	new_child(NULL, true);
}