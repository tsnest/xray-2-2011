////////////////////////////////////////////////////////////////////////////
//	Created		: 19.04.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef GAME_DIALOG_H_INCLUDED
#define GAME_DIALOG_H_INCLUDED

#include "dialog_node_base.h"
#include "dialog_node_visitors.h"
#pragma managed( push, off )

namespace xray {
namespace dialog_editor {
	class dialog_node_base;

	class game_dialog : public dialog_node_base, public resources::unmanaged_resource
	{
		typedef associative_vector<u32, dialog_node_base*, vector> created_nodes_type;
	public:
								game_dialog			(xray::resources::queries_result& result);
		virtual					~game_dialog		();
		virtual	void			set_text			(pcstr new_text) {XRAY_UNREFERENCED_PARAMETER(new_text);};
		virtual pcstr			text				() {return m_text;};
		dialog_node_base* const&	get_root		(u32 index) const;
				u32				get_roots_count		() const {return m_roots_counter;};

	private:
				u32				calculate_size		(xray::configs::lua_config_value const& cfg);
				void			create_roots		(xray::configs::lua_config_value const& cfg);
				void			create_childs		(dialog_node_base* parent, xray::configs::lua_config_value const& cfg);
		dialog_expression*		create_precondition	(xray::configs::lua_config_value const& cfg);
		dialog_node_base*		new_node			(xray::configs::lua_config_value const& cfg);
				pstr const		new_text_from_buffer(pcstr const string_from_config);

				void			delete_childs		(dialog_node_base* parent);
				void			delete_preconditions(dialog_expression* root);

	private:
		pvoid							m_buffer_raw;
		mutable_buffer					m_buffer;
		dialog_node_base**				m_roots;
		u32								m_roots_counter;
		pcstr							m_text;
		created_nodes_type				m_created_nodes;
	}; // class dialog
} // namespace dialog_editor
} // namespace xray
#pragma managed( pop )
#endif // #ifndef GAME_DIALOG_H_INCLUDED
