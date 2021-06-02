////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_GRAPH_NODE_STYLE_H_INCLUDED
#define PARTICLE_GRAPH_NODE_STYLE_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;

namespace xray {
	namespace editor {
		public ref class particle_graph_node_style : public controls::hypergraph::node_style
		{
		public:
			void	load	(configs::lua_config_value const& t);
		}; // ref class particle_graph_node_style

		ref class particle_graph_node_style_mgr : public xray::editor::controls::hypergraph::nodes_style_manager
		{
		private:
			typedef xray::editor::controls::hypergraph::node		node;
			typedef xray::editor::controls::hypergraph::node_style	node_style;

		public:
			event EventHandler^ library_loaded;

		public:
			particle_graph_node_style_mgr();
			~particle_graph_node_style_mgr();
			virtual		void	assign_style(node^ n, System::String^ style_name);
			void	load_library();
			void	on_library_loaded(xray::resources::queries_result& data);

		private:
			configs::lua_config_ptr*		m_library_cfg;
			System::Collections::ArrayList^	m_library;
			ImageList^						m_images;
			ImageList^						m_mini_images;
		public:
			property ImageList^				images{
				ImageList^			get(){return m_images;}
			}
			property ImageList^				mini_images{
				ImageList^			get(){return m_mini_images;}
			}
		}; // class particle_graph_node_style_mgr
	} //namespace editor
} //namespace xray

#endif //PARTICLE_GRAPH_NODE_STYLE_H_INCLUDED