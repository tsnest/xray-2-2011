////////////////////////////////////////////////////////////////////////////
//	Created		: 01.02.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_CHANGE_NODE_STRING_TABLE_H_INCLUDED
#define COMMAND_CHANGE_NODE_STRING_TABLE_H_INCLUDED

namespace xray {
namespace dialog_editor {
	ref class dialog_hypergraph;
	ref class dialog_editor_impl;
	ref class dialog_graph_node_phrase;

	public ref class command_change_node_string_table : public xray::editor_base::command
	{
	public:
						command_change_node_string_table	(dialog_hypergraph^ h, dialog_graph_node_phrase^ n, System::String^ new_str, System::String^ new_text, System::String^ old_str, System::String^ old_text);
		virtual			~command_change_node_string_table	()			{};

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
		dialog_hypergraph^	m_hypergraph;
		dialog_editor_impl^	m_editor;
		u32				 	m_node_id;
		System::String^ 	m_old_str;
		System::String^ 	m_new_str;
		System::String^ 	m_old_text;
		System::String^ 	m_new_text;
		System::String^ 	m_backup_str_text;
		bool				m_first_run;
	}; // class command_change_node_string_table
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef COMMAND_CHANGE_NODE_STRING_TABLE_H_INCLUDED