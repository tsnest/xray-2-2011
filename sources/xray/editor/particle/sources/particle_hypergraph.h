////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_HYPERGRAPH_H_INCLUDED
#define PARTICLE_HYPERGRAPH_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using xray::editor_base::command_engine;

namespace xray {
	namespace editor_base { ref class command_engine; }
	namespace particle_editor {
		ref class particle_graph_document;
		ref class xray::editor::controls::hypergraph::connection_point;
		ref class xray::editor::controls::hypergraph::node;
		ref class xray::editor::controls::hypergraph::link;
		ref class particle_graph_node;

		enum class xray::editor::controls::hypergraph::view_mode;

		public ref class particle_hypergraph : public xray::editor::controls::hypergraph::hypergraph_control
		{
		private:
			typedef xray::editor::controls::hypergraph::hypergraph_control super;
			typedef xray::editor::controls::hypergraph::connection_point connection_point;
			typedef xray::editor::controls::hypergraph::node node;
			typedef xray::editor::controls::hypergraph::link link;
			typedef Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
			typedef Generic::List<xray::editor::controls::hypergraph::link^ > links;

		public:
			particle_hypergraph			(particle_graph_document^ owner);
			~particle_hypergraph			();
			virtual	void	append_node					(node^ node) override;				
			void			move_node					(node^ nod, node^ new_parent);
			links^			copy_links					();
			void			change_parent_node			(node^ nod, node^ new_parent_node);
			virtual	void	make_connection				(connection_point^ psrc, connection_point^ pdst) override;
			void			remove_group				(nodes^ nds);
			virtual	void	on_key_down					(Object^ , System::Windows::Forms::KeyEventArgs^ e) override;
			virtual	void	on_key_up					(Object^ , System::Windows::Forms::KeyEventArgs^ e) override;
			virtual void	hypergraph_Paint			(System::Object^ , System::Windows::Forms::PaintEventArgs^ e) override;
			void			undo						( );
			void			redo						( );
			void			save						( );
			bool			is_saved					( );
			void			clear_command_stack			( );
			void			show_available_nodes		( particle_graph_node^ nod);
			void			hide_available_nodes		( particle_graph_node^ nod);
			particle_graph_node^ get_node_by_id			(String^ node_id);
			Void			delete_node					(String^ node_id);
	
		private: 
			
			particle_graph_document^					m_owner;
			particle_graph_node^						m_root_node;
			xray::editor_base::command_engine^			m_command_engine;

		public:
			property xray::editor_base::command_engine^ comm_engine{
				xray::editor_base::command_engine^				get() {return m_command_engine;}
			}
			property particle_graph_document^			owner{
				particle_graph_document^	get() {return m_owner;}
			}	
			property particle_graph_node^				root_node{
				particle_graph_node^		get() {return m_root_node;}
			}
		};  // class dialog_hypergraph 
	} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_HYPERGRAPH_H_INCLUDED