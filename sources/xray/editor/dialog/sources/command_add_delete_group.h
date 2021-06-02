////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef COMMAND_ADD_DELETE_GROUP_H_INCLUDED
#define COMMAND_ADD_DELETE_GROUP_H_INCLUDED

namespace xray {
namespace dialog_editor {
	ref class dialog_graph_node_base;
	ref class dialog_hypergraph;
	ref class xray::editor::controls::hypergraph::node;
	ref class xray::editor::controls::hypergraph::link;

	public ref class command_add_group : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
		typedef System::Collections::Generic::Dictionary<u32, u32> ids_type;
		typedef System::Collections::Generic::List<u32> nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, System::String^> string_ids_type;
	public:
						command_add_group	(dialog_hypergraph^ h, pcstr config_string, System::Drawing::Point offset, xray::editor::controls::hypergraph::node^ n);
		virtual			~command_add_group	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;
				void	init_node			(dialog_graph_node_base^ n, System::Drawing::Point p);

	private:
		dialog_hypergraph^ 			m_hypergraph;
		pstr						m_cfg_string;
		u32							m_node_to_connect_id;
		ids_type^					m_nodes_ids;
		links_ids_type^				m_links_ids;
		string_ids_type^			m_string_ids;
		System::Drawing::Point		m_offset;
		bool						m_first_run;
	}; // class command_add_group

	public ref class command_remove_group : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::link^ > links;
		typedef System::Collections::Generic::List<u32 > nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
	public:
						command_remove_group	(dialog_hypergraph^ h, nodes^ nds, links^ lnks);
		virtual			~command_remove_group	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		dialog_hypergraph^ 	m_hypergraph;
		nodes_ids_type^		m_nodes_ids;
		links_ids_type^		m_links_ids;
		pstr				m_cfg_string;
		bool				m_first_run;
	}; // class command_remove_group
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef COMMAND_ADD_DELETE_GROUP_H_INCLUDED