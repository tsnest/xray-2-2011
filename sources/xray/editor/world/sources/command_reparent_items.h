////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_REPARENT_ITEMS_H_INCLUDED
#define COMMAND_REPARENT_ITEMS_H_INCLUDED

#include "project_defines.h"

namespace xray {
namespace editor {

public ref class command_reparent_items :	public xray::editor_base::command
{
public:

	command_reparent_items	( project^ p, string_list^ src_names, System::String^ new_parent );

	virtual bool commit		() override;
	virtual void rollback	() override;

private:
	ref struct move_item
	{
		System::String^ src_name;
		System::String^ dst_name;
	};
	typedef System::Collections::Generic::List<move_item^>	dictionary;
	dictionary			m_storage;

	project^			m_project;
}; // class command_delete_selected_objects

} // namespace editor
} // namespace xray

#endif //COMMAND_REPARENT_ITEMS_H_INCLUDED