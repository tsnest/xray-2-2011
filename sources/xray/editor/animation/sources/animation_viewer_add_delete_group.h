////////////////////////////////////////////////////////////////////////////
//	Created		: 10.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_ADD_DELETE_GROUP_H_INCLUDED
#define ANIMATION_VIEWER_ADD_DELETE_GROUP_H_INCLUDED

namespace xray {
namespace animation_editor {
	ref class animation_viewer_graph_node_base;
	ref class animation_viewer_hypergraph;
	ref class animation_viewer_graph_node_base;
	ref class xray::editor::controls::hypergraph::node;
	ref class xray::editor::controls::hypergraph::link;
	ref class animation_viewer_graph_link_weighted;

	public ref class animation_viewer_add_group : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
		typedef System::Collections::Generic::Dictionary<u32, u32> ids_type;
		typedef System::Collections::Generic::List<u32> nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
	public:
						animation_viewer_add_group	(animation_viewer_hypergraph^ h, pcstr config_string, System::Drawing::Point offset);
		virtual			~animation_viewer_add_group	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		animation_viewer_hypergraph^ m_hypergraph;
		pstr						m_cfg_string;
		ids_type^					m_nodes_ids;
		links_ids_type^				m_links_ids;
		System::Drawing::Point		m_offset;
		bool						m_first_run;
	}; // class command_add_group

	public ref struct link_weight_values
	{
		link_weight_values(u32 src, u32 dst, float o, float n)
			:src_id(src),
			dst_id(dst),
			old_val(o),
			new_val(n)
		{}
		u32 src_id;
		u32 dst_id;
		float old_val;
		float new_val;
	};

	public ref class animation_viewer_remove_group : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::link^ > links;
		typedef System::Collections::Generic::List<u32 > nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
		typedef System::Collections::Generic::List<link_weight_values^> link_values_type;
	public:
						animation_viewer_remove_group	(animation_viewer_hypergraph^ h, nodes^ nds, links^ lnks);
		virtual			~animation_viewer_remove_group	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
				void	init_weight_values	(animation_viewer_graph_link_weighted^ initial_link, links^ deleting_links);

	private:
		animation_viewer_hypergraph^ 	m_hypergraph;
		nodes_ids_type^					m_nodes_ids;
		links_ids_type^					m_links_ids;
		link_values_type^				m_link_values;
		pstr							m_cfg_string;
		bool							m_first_run;
	}; // class command_remove_group
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_ADD_DELETE_GROUP_H_INCLUDED