////////////////////////////////////////////////////////////////////////////
//	Created		: 16.03.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef DIALOG_H_INCLUDED
#define DIALOG_H_INCLUDED

#include "dialog_node_base.h"
#pragma managed( push, off )

namespace xray {
namespace dialog_editor {
	class dialog_phrase;

	class dialog : public resources::unmanaged_resource
	{
		typedef vector<dialog_node_base*> phrases_list;
	public:
									dialog		(configs::lua_config_value const& cfg);
		virtual						~dialog		();
		virtual	void				set_name	(pcstr new_name);
		virtual pcstr				name		();
				void				add_node	(dialog_node_base* n);
				void				remove_node	(dialog_node_base* n);
				dialog_node_base*	get_node	(u32 id);
		const	phrases_list*		get_phrases	()	{return &m_phrases;};
				void				save		(configs::lua_config_value cfg);
				void				clear		();

	public:
		u32				m_last_id;

	private:
		phrases_list	m_phrases;
		pstr			m_name;
	}; // class dialog
} // namespace dialog_editor
} // namespace xray
#pragma managed( pop )
#endif // #ifndef DIALOG_H_INCLUDED
