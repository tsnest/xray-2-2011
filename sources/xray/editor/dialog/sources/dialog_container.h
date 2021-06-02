////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_CONTAINER_H_INCLUDED
#define DIALOG_CONTAINER_H_INCLUDED

#include "dialogs_manager.h"
#include "dialog_node_base.h"
#pragma managed( push, off )

namespace xray {
namespace dialog_editor {
	class dialog_container : public dialog_node_base
	{
	public:
									dialog_container	();
		virtual						~dialog_container	();
		virtual	void				set_text			(pcstr new_text);
		virtual pcstr				text				();
		void						add_node			(dialog_node_base* n);
		void						remove_node			(dialog_node_base* n);
		void						remove_node			(u32 id);
		dialog_node_base*			get_node			(u32 id);
		void						save				(configs::lua_config_value cfg);
		void						request_dialog		();
		const dialog_resources_ptr*	get_resource		() {return m_resource;};

	private:
		void				on_dialog_loaded	(dialog_resources_ptr p);

	private:
		dialog_resources_ptr*					m_resource;
		pstr									m_text;
	}; // class dialog_container
} // namespace dialog_editor
} // namespace xray
#pragma managed( pop )
#endif // #ifndef DIALOG_CONTAINER_H_INCLUDED
