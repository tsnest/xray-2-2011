////////////////////////////////////////////////////////////////////////////
//	Created		: 24.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef COMMAND_MOVE_NODES_H_INCLUDED
#define COMMAND_MOVE_NODES_H_INCLUDED

namespace xray {
namespace dialog_editor {
	ref class dialog_hypergraph;
	ref class dialog_graph_node_base;

	public ref class command_move_nodes : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<u32 > nodes_ids;
		typedef System::Collections::Generic::List<dialog_graph_node_base^ > nodes;
		typedef System::Collections::Generic::List<System::Drawing::Point> points_list;
	public:
						command_move_nodes		(dialog_hypergraph^ h, nodes^ n, System::Drawing::Point offset);
		virtual			~command_move_nodes		()	{};

		virtual bool	commit					() override;
		virtual void	rollback				() override;

	private:
		dialog_hypergraph^	m_hypergraph;
		nodes_ids^			m_nodes_ids;
		points_list			m_rollback_positions;
		points_list			m_cur_positions;
	}; // class command_move_nodes
} // namespace dialog_editor
} // namespace xray
#endif // #ifndef COMMAND_MOVE_NODES_H_INCLUDED