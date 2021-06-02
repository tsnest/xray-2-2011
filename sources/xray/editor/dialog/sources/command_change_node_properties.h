////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef COMMAND_CHANGE_NODE_PROPERTIES_H_INCLUDED
#define COMMAND_CHANGE_NODE_PROPERTIES_H_INCLUDED

#include <list>

namespace xray {
namespace dialog_editor {
	ref class dialog_hypergraph;
	ref class dialog_graph_node_base;

	public ref class command_change_node_properties : public xray::editor_base::command
	{
	public:
						command_change_node_properties		(dialog_hypergraph^ h, dialog_graph_node_base^ n, System::String^ property_name, System::Object^ new_v, System::Object^ old_v);
		virtual			~command_change_node_properties		()			{};

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
		dialog_hypergraph^ 	m_hypergraph;
		u32 				m_node_id;
		System::Object^ 	m_old_value;
		System::Object^ 	m_new_value;
		System::String^ 	m_changed_property_name;
	}; // class command_change_node_properties
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef COMMAND_CHANGE_NODE_PROPERTIES_H_INCLUDED