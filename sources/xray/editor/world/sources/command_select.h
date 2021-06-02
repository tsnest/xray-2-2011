////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_SELECT_H_INCLUDED
#define COMMAND_SELECT_H_INCLUDED

#include "project_defines.h"

namespace xray {
namespace editor {

public ref class command_select :	public xray::editor_base::command
{
public:
	command_select	( project^ p );
	
	command_select	( project^ p, project_items_list^ items, enum_selection_method method );
	command_select	( project^ p, project_item_base^ item, enum_selection_method method );
	command_select	( project^ p, item_id_list^ objects, enum_selection_method method );
	command_select	( project^ p, u32 id, enum_selection_method method );

	virtual bool commit		() override;
	virtual void rollback	() override;
			bool work		( enum_selection_method method );

private:
	project^				m_project;
	item_id_list^			m_item_ids;
	enum_selection_method	m_selection_method;
}; // class command_select

} // namespace editor
} // namespace xray

#endif // #ifndef COMMAND_SELECT_H_INCLUDED