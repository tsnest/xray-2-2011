////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_node_base.h"
#include <xray/intrusive_list.h>
#include "dialog_link.h"

#pragma unmanaged
using xray::dialog_editor::dialog_node_base;
using xray::dialog_editor::dialog_link;

dialog_node_base::dialog_node_base()
:m_is_root(true),
m_id(u32(-1))
{

}

dialog_node_base::~dialog_node_base()
{
	//for(dialog_link* lnk = m_links_list.pop_front(); lnk!=NULL; lnk = m_links_list.pop_front())
	//	DELETE(lnk);
}

dialog_link* dialog_node_base::new_link(dialog_node_base* obj, bool change_is_root)
{
	if(change_is_root)
		obj->set_is_root(false);

	dialog_link* lnk = NEW(dialog_link)(obj);
	m_links_list.push_back(lnk);
	return m_links_list.back();
}

void dialog_node_base::add_link(dialog_link* lnk)
{
	m_links_list.push_back(lnk);
}

void dialog_node_base::remove_link(dialog_link* lnk)
{
	m_links_list.erase(lnk);
}
