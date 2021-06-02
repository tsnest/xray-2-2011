////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_NODE_BASE_H_INCLUDED
#define DIALOG_NODE_BASE_H_INCLUDED

#include "dialog_link.h"

#pragma managed( push, off )

#include <xray/intrusive_list.h>

namespace xray {
namespace dialog_editor {

class dialog_link;

typedef intrusive_list<dialog_link, 
	dialog_link*, 
	&dialog_link::m_next_link> links_list;

class dialog_node_base
{
public:

									dialog_node_base	();
	virtual							~dialog_node_base	();
			void					set_id				(u32 new_id)				{m_id = new_id;};
			u32						id					()							{return m_id;};
			void					set_is_root			(bool is)					{m_is_root = is;};
			bool					is_root				()							{return m_is_root;};
			dialog_link*			new_link			(dialog_node_base* obj, bool change_is_root=true);
			void					add_link			(dialog_link* lnk);
			void					remove_link			(dialog_link* lnk);
	virtual	pcstr					text				()=0;
	virtual	void					set_text			(pcstr new_text)=0;
	const	links_list*				get_links			()							{return &m_links_list;};

private:
	u32					m_id;
	bool				m_is_root;
	links_list			m_links_list;
}; // class dialog_node_base

} // namespace dialog_editor 
} // namespace xray 

#pragma managed( pop )

#endif // #ifndef DIALOG_NODE_BASE_H_INCLUDED