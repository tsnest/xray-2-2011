////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_LINKS_MANAGER_H_INCLUDED
#define PARTICLE_LINKS_MANAGER_H_INCLUDED

using namespace System::Collections;

namespace xray {
	namespace particle_editor {
		ref class particle_links_manager : public xray::editor::controls::hypergraph::links_manager
		{
			typedef xray::editor::controls::hypergraph::node				node;
			typedef xray::editor::controls::hypergraph::link				link;
			typedef xray::editor::controls::hypergraph::connection_point	connection_point;
			typedef Generic::List<link^>					links;
			typedef Generic::List<node^>					nodes;

		private:
			links^											m_links;
			xray::editor::controls::hypergraph::hypergraph_control^		m_hypergraph;

		public:
			particle_links_manager	(xray::editor::controls::hypergraph::hypergraph_control^ h);
			virtual void								on_node_added			(node^ node);
			virtual void								on_node_removed			(node^ node);
			virtual void								on_node_destroyed		(node^ node);
			virtual void								create_link				(xray::editor::controls::hypergraph::connection_point^ pt_src, xray::editor::controls::hypergraph::connection_point^ pt_dst);
			void										create_link				(node^ src_node, node^ dst_node);
			virtual void								destroy_link			(xray::editor::controls::hypergraph::link^ link);
					void								destroy_link			(node^ src_node, node^ dst_node);
			virtual links^								visible_links			();
			virtual void								clear					();
		}; // ref class particle_links_manager
	} // namespace particle_editor
} // namespace xray

#endif //PARTICLE_LINKS_MANAGER_H_INCLUDED