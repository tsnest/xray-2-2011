////////////////////////////////////////////////////////////////////////////
//	Created		: 11.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_GROUPS_ADD_REMOVE_GROUP_COMMAND_H_INCLUDED
#define ANIMATION_GROUPS_ADD_REMOVE_GROUP_COMMAND_H_INCLUDED

using namespace xray::editor::wpf_controls::hypergraph;
using namespace System;
using namespace System::Windows;
using namespace System::Collections::Generic;

namespace xray {
namespace animation_editor {

	ref class animation_groups_document;
	ref class animation_node_interval;
	ref class animation_node_interval_instance;

	public ref class animation_groups_add_group_command: public xray::editor_base::command
	{
	public:
		animation_groups_add_group_command	(animation_groups_document^ d);
		~animation_groups_add_group_command	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		animation_groups_document^	m_document;
		Guid						m_node_id;
	}; // class animation_groups_add_group_command

	public ref class animation_groups_add_link_command: public xray::editor_base::command
	{
	public:
		animation_groups_add_link_command	(animation_groups_document^ d, link_event_args^ e);
		~animation_groups_add_link_command	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		animation_groups_document^	m_document;
		String^						m_src_node_id;
		String^						m_dst_node_id;
	}; // class animation_groups_add_link_command

	public ref class animation_groups_delete_selected_command: public xray::editor_base::command
	{
		typedef List<String^>							nodes_ids_collection;
		typedef List<KeyValuePair<String^, String^>>	links_ids_collection;
	public:
		animation_groups_delete_selected_command	(animation_groups_document^ d, selection_event_args^ e);
		~animation_groups_delete_selected_command	();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_groups_document^	m_document;
		nodes_ids_collection^		m_nodes_ids;
		links_ids_collection^		m_links_ids;
		pstr						m_cfg_string;
	}; // class animation_groups_remove_group_command

	public ref class animation_groups_change_model_command: public xray::editor_base::command
	{
	public:
		animation_groups_change_model_command		(animation_groups_document^ d, String^ new_val, String^ old_val);
		~animation_groups_change_model_command		();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_groups_document^	m_document;
		String^						m_new_val;
		String^						m_old_val;
	}; // class animation_groups_remove_group_command

	public ref class animation_groups_nodes_move_command: public xray::editor_base::command
	{
	public:
		animation_groups_nodes_move_command			(animation_groups_document^ d, node_move_event_args^ e);
		~animation_groups_nodes_move_command		();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_groups_document^	m_document;
		List<node_move_args^>^		m_args;
	}; // class animation_groups_nodes_move_command

	public ref class animation_groups_add_intervals_command: public xray::editor_base::command
	{
		typedef List<animation_node_interval_instance^> intervals_list;
	public:
		animation_groups_add_intervals_command		(animation_groups_document^ d, node^ n, List<animation_node_interval^>^ lst);
		~animation_groups_add_intervals_command		();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_groups_document^	m_document;
		String^						m_node_id;
		intervals_list^				m_intervals;
	}; // class animation_groups_add_intervals_command

	public ref class animation_groups_remove_intervals_command: public xray::editor_base::command
	{
		typedef List<animation_node_interval_instance^> intervals_list;
	public:
		animation_groups_remove_intervals_command	(animation_groups_document^ d, node^ n, System::Collections::IList^ lst);
		~animation_groups_remove_intervals_command	();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_groups_document^	m_document;
		String^						m_node_id;
		intervals_list^				m_intervals;
		bool						m_first_run;
	}; // class animation_groups_remove_intervals_command

	public ref class animation_groups_change_property_command: public xray::editor_base::command
	{
	public:
		animation_groups_change_property_command	(animation_groups_document^ d, String^ node_id, String^ property_name, Object^ new_val, Object^ old_val);
		~animation_groups_change_property_command	();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_groups_document^	m_document;
		String^						m_node_id;
		String^						m_property_name;
		Object^						m_old_val;
		Object^						m_new_val;
	}; // class animation_groups_change_property_command

	public ref class animation_groups_change_interval_property_command: public xray::editor_base::command
	{
	public:
		animation_groups_change_interval_property_command	(animation_groups_document^ d, String^ node_id, u32 index, String^ property_name, Object^ new_val, Object^ old_val);
		~animation_groups_change_interval_property_command	();

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
		animation_groups_document^	m_document;
		String^						m_node_id;
		u32							m_interval_index;
		String^						m_property_name;
		Object^						m_old_val;
		Object^						m_new_val;
	}; // class animation_groups_change_interval_property_command

	public ref class animation_groups_change_link_property_command: public xray::editor_base::command
	{
	public:
		animation_groups_change_link_property_command	(animation_groups_document^ d, link^ lnk, Double new_val, Double old_val);
		~animation_groups_change_link_property_command	();

		virtual bool	commit							() override;
		virtual void	rollback						() override;

	private:
		animation_groups_document^	m_document;
		String^						m_src_node_id;
		String^						m_dst_node_id;
		Double						m_old_val;
		Double						m_new_val;
	}; // class animation_groups_change_property_command

	public ref class animation_groups_change_node_value_property_command: public xray::editor_base::command
	{
	public:
		animation_groups_change_node_value_property_command	(animation_groups_document^ d, node^ n, Double new_val, Double old_val);
		~animation_groups_change_node_value_property_command();

		virtual bool	commit								() override;
		virtual void	rollback							() override;

	private:
		animation_groups_document^	m_document;
		String^						m_node_id;
		Double						m_old_val;
		Double						m_new_val;
	}; // class animation_groups_change_property_command
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_GROUPS_ADD_REMOVE_GROUP_COMMAND_H_INCLUDED