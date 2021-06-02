////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_HYPERGRAPH_H_INCLUDED
#define DIALOG_HYPERGRAPH_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
namespace editor_base { ref class command_engine; }
namespace dialog_editor {

	ref class xray::editor::controls::hypergraph::connection_point;
	ref class editor::controls::hypergraph::node;
	ref class editor::controls::hypergraph::link;
	enum class editor::controls::hypergraph::view_mode;

	ref class dialog_document;
	ref class command_move_nodes;
	ref class dialog_graph_link;
	ref class dialog_link_action;
	ref class dialog_link_precondition;
	ref class dialog_graph_node_base;

	public ref class dialog_hypergraph : public xray::editor::controls::hypergraph::hypergraph_control
	{
	private:
		typedef xray::editor::controls::hypergraph::hypergraph_control super;
		typedef xray::editor::controls::hypergraph::connection_point connection_point;
		typedef xray::editor::controls::hypergraph::node node;
		typedef xray::editor::controls::hypergraph::link link;
		typedef Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
		typedef Generic::List<xray::editor::controls::hypergraph::link^ > links;
	public:
		static	dialog_graph_node_base^	find_node_by_id		(nodes^ nds, u32 id);
		static	dialog_graph_link^		find_link_by_ids	(links^ lnks, u32 id1, u32 id2);

	public:
						dialog_hypergraph			(dialog_document^ owner);
						~dialog_hypergraph			();
		links^			copy_links					();
		virtual	void	destroy_connection			(link^ lnk) override;
		void			destroy_connection			(links^ lnks);
		virtual	void	make_connection				(connection_point^ psrc, connection_point^ pdst) override;
		void			add_connection				(links^ lnks);
		void			add_group					(pcstr config_string, Point offset);
		void			remove_group				(nodes^ nds, links^ lnks);
		virtual	void	on_key_down					(Object^ , System::Windows::Forms::KeyEventArgs^ e) override;
		virtual	void	on_key_up					(Object^ , System::Windows::Forms::KeyEventArgs^ e) override;
		virtual	void	hypergraph_area_MouseDown	(Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void	hypergraph_area_MouseMove	(Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual	void	hypergraph_area_MouseUp		(Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void	on_hypergraph_scroll		(Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void	hypergraph_Paint			(System::Object^ , System::Windows::Forms::PaintEventArgs^ e) override;
		virtual void	do_layout					() override;
				void	undo						();
				void	redo						();
				void	save						();
				void	clear_command_stack			();
				void	on_node_property_changed	(node^ n, String^ label, Object^ v, Object^ ov);
				void	on_node_text_changed		(node^ n, String^ new_str, String^ new_text, String^ old_str, String^ old_text);
		dialog_document^ get_owner					()				{return m_owner;};
				void	set_preconditions_and_actions(dialog_graph_link^ lnk, dialog_link_precondition^ prec_list, Generic::List<dialog_link_action^ >^ acts);

				void	cut							(System::Object^ , System::EventArgs^ );
				void	copy						(System::Object^ , System::EventArgs^ );
				void	paste						(System::Object^ , System::EventArgs^ );
				void	del							(System::Object^ , System::EventArgs^ );

	protected:
		virtual	void	drag_start					() override;
		virtual	void	drag_stop					() override;

				void	links_move_start			(links^ lnks, connection_point^ cp);
				void	links_move_update			();
				void	links_move_stop				();

	private: 
		xray::editor_base::command_engine^				m_command_engine;
		command_move_nodes^							m_move_nodes_command;
		dialog_document^							m_owner;
		Point										m_start_move_point;
		bool										m_links_move_mode;
		links^										m_moving_links;
		connection_point^							m_moving_links_start_point;

		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
		System::Windows::Forms::ToolStripMenuItem^	cut_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	copy_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	paste_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	delete_menu_item;
	};  // class dialog_hypergraph 
} // namespace dialog_editor 
} // namespace xray

#endif // #ifndef DIALOG_HYPERGRAPH_H_INCLUDED