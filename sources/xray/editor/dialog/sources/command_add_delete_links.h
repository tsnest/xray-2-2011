////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef COMMAND_ADD_DELETE_LINKS_H_INCLUDED
#define COMMAND_ADD_DELETE_LINKS_H_INCLUDED

namespace xray {
namespace dialog_editor {
	ref class xray::editor::controls::hypergraph::link;
	ref class xray::editor::controls::hypergraph::connection_point;
	ref class dialog_hypergraph;
	ref class dialog_graph_link;
	ref class dialog_link_action;
	ref class dialog_link_precondition;

	class dialog_action;
	class dialog_expression;

	public ref class command_add_links : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<dialog_graph_link^ > links;
		typedef System::Collections::Generic::List<u32 > nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
	public:
						command_add_links	(dialog_hypergraph^ h, links^ lnks);
		virtual			~command_add_links	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		dialog_hypergraph^ 	m_hypergraph;
		links_ids_type^		m_links_ids;
		pstr				m_cfg_string;
	}; // class command_add_links

	public ref class command_remove_links : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<dialog_graph_link^ > links;
		typedef System::Collections::Generic::List<u32 > nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
	public:
						command_remove_links	(dialog_hypergraph^ h, links^ lnks);
		virtual			~command_remove_links	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		dialog_hypergraph^ 	m_hypergraph;
		links_ids_type^		m_links_ids;
		pstr				m_cfg_string;
	}; // class command_remove_links

	public ref class command_set_actions_and_preconditions : public xray::editor_base::command
	{
	public:
						command_set_actions_and_preconditions	(dialog_hypergraph^ h, dialog_graph_link^ lnk, System::Collections::Generic::List<dialog_link_action^ >^ acts, dialog_link_precondition^ prec_root);
		virtual			~command_set_actions_and_preconditions	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		dialog_hypergraph^ 	m_hypergraph;
		pstr				m_cfg_string;
	}; // class command_set_actions_and_preconditions

	public ref class command_change_links_connection_point : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::link^ > links;
		typedef System::Collections::Generic::KeyValuePair<u32, u32> links_ids_pair;
		typedef System::Collections::Generic::List<links_ids_pair> links_ids_type;
	public:
						command_change_links_connection_point	(dialog_hypergraph^ h, links^ lnks, xray::editor::controls::hypergraph::node^ n, bool dest);
		virtual			~command_change_links_connection_point	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		dialog_hypergraph^ 	m_hypergraph;
		links_ids_type^		m_new_links_ids;
		links_ids_type^		m_old_links_ids;
		u32					m_old_node_id;
		u32					m_new_node_id;
		bool				m_destination;
	}; // class command_change_links_connection_point
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef COMMAND_ADD_DELETE_LINKS_H_INCLUDED