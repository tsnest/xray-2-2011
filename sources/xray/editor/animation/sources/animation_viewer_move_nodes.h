////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_MOVE_NODES_H_INCLUDED
#define ANIMATION_VIEWER_MOVE_NODES_H_INCLUDED

namespace xray {
namespace animation_editor {
	ref class animation_viewer_hypergraph;
	ref class animation_viewer_graph_node_base;

	public ref class animation_viewer_move_nodes : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<u32 > nodes_ids;
		typedef System::Collections::Generic::List<animation_viewer_graph_node_base^ > nodes;
		typedef System::Collections::Generic::List<System::Drawing::Point> points_list;
	public:
						animation_viewer_move_nodes		(animation_viewer_hypergraph^ h, nodes^ n, System::Drawing::Point offset);
		virtual			~animation_viewer_move_nodes	()	{};

		virtual bool	commit							() override;
		virtual void	rollback						() override;

	private:
		animation_viewer_hypergraph^	m_hypergraph;
		nodes_ids^						m_nodes_ids;
		points_list						m_rollback_positions;
		points_list						m_cur_positions;
	}; // class command_move_nodes
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_MOVE_NODES_H_INCLUDED