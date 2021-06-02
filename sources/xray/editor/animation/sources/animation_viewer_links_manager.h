////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_VIEWER_LINKS_MANAGER_H_INCLUDED
#define ANIMATION_VIEWER_LINKS_MANAGER_H_INCLUDED

using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;

namespace xray {
namespace animation_editor {
	ref class animation_viewer_links_manager : public xray::editor::controls::hypergraph::links_manager
	{
		typedef xray::editor::controls::hypergraph::node				node;
		typedef xray::editor::controls::hypergraph::link				link;
		typedef xray::editor::controls::hypergraph::connection_point	connection_point;
		typedef Generic::List<link^>					links;
		typedef Generic::List<node^>					nodes;

	public:
						animation_viewer_links_manager(xray::editor::controls::hypergraph::hypergraph_control^ h);
						~animation_viewer_links_manager();
		virtual void	on_node_added		(node^ node);
		virtual void	on_node_removed		(node^ node);
		virtual void	on_node_destroyed	(node^ node);
		virtual void	create_link			(xray::editor::controls::hypergraph::connection_point^ pt_src, xray::editor::controls::hypergraph::connection_point^ pt_dst);
		virtual void	destroy_link		(xray::editor::controls::hypergraph::link^ link);
		virtual links^	visible_links		();
		virtual void	clear				();

	private:
		links^													m_links;
		xray::editor::controls::hypergraph::hypergraph_control^	m_hypergraph;
	}; // ref class dialog_links_manager
} // namespace animation_editor
} // namespace xray

#endif // #ifndef ANIMATION_VIEWER_LINKS_MANAGER_H_INCLUDED