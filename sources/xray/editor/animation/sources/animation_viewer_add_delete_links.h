////////////////////////////////////////////////////////////////////////////
//	Created		: 14.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_VIEWER_ADD_DELETE_LINKS_H_INCLUDED
#define ANIMATION_VIEWER_ADD_DELETE_LINKS_H_INCLUDED

namespace xray {
namespace animation_editor {
	ref class xray::editor::controls::hypergraph::link;
	ref class xray::editor::controls::hypergraph::connection_point;
	ref class animation_viewer_hypergraph;
	ref class animation_viewer_graph_link;
	ref class animation_viewer_graph_link_weighted;

	public ref class animation_viewer_add_links : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::link^ > links;
		typedef System::Collections::Generic::List<u32 > nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
	public:
						animation_viewer_add_links	(animation_viewer_hypergraph^ h, links^ lnks);
		virtual			~animation_viewer_add_links	();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		animation_viewer_hypergraph^ 	m_hypergraph;
		links_ids_type^					m_links_ids;
		pstr							m_cfg_string;
	}; // class animation_viewer_add_links

	public ref class animation_viewer_remove_links : public xray::editor_base::command
	{
		typedef System::Collections::Generic::List<xray::editor::controls::hypergraph::link^ > links;
		typedef System::Collections::Generic::List<u32 > nodes_ids_type;
		typedef System::Collections::Generic::Dictionary<u32, nodes_ids_type^> links_ids_type;
	public:
						animation_viewer_remove_links	(animation_viewer_hypergraph^ h, links^ lnks);
		virtual			~animation_viewer_remove_links	();

		virtual bool	commit							() override;
		virtual void	rollback						() override;

	private:
		animation_viewer_hypergraph^ 	m_hypergraph;
		links_ids_type^					m_links_ids;
		pstr							m_cfg_string;
	}; // class animation_viewer_remove_links
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_ADD_DELETE_LINKS_H_INCLUDED