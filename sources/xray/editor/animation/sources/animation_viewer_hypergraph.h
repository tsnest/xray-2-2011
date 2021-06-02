////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION_VIEWER_HYPERGRAPH_H_INCLUDED
#define ANIMATION_VIEWER_HYPERGRAPH_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using xray::editor_base::command_engine;

namespace xray {
namespace animation_editor {

	ref class xray::editor::controls::hypergraph::connection_point;
	ref class editor::controls::hypergraph::node;
	ref class editor::controls::hypergraph::link;
	enum class editor::controls::hypergraph::view_mode;

	ref class viewer_editor;
	ref class viewer_document;
	ref class animation_viewer_move_nodes;
	ref class animation_viewer_graph_link;
	ref class animation_viewer_graph_link_weighted;
	ref class animation_viewer_graph_node_base;
	ref class animation_node_interval;

	public ref class animation_viewer_hypergraph: public xray::editor::controls::hypergraph::hypergraph_control
	{
		typedef xray::editor::controls::hypergraph::hypergraph_control super;
		typedef xray::editor::controls::hypergraph::connection_point connection_point;
		typedef xray::editor::controls::hypergraph::node node;
		typedef xray::editor::controls::hypergraph::link link;
		typedef Generic::List<xray::editor::controls::hypergraph::node^ > nodes;
		typedef Generic::List<xray::editor::controls::hypergraph::link^ > links;
		typedef xray::editor::wpf_controls::property_container wpf_property_container;

	public:
		static	animation_viewer_graph_node_base^	find_node_by_id		(nodes^ nds, u32 id);
		static	animation_viewer_graph_link^		find_link_by_ids	(links^ lnks, u32 id1, u32 id2);
		static	animation_viewer_graph_link^		create_link			(animation_viewer_graph_node_base^ src_node, animation_viewer_graph_node_base^ dst_node);

	public:
						animation_viewer_hypergraph	(viewer_document^ owner);
						~animation_viewer_hypergraph();
		virtual	void	destroy_connection			(link^ lnk) override;
		void			destroy_connection			(links^ lnks);
		void			add_connection				(links^ lnks);
		void			add_group					(pcstr config_string, Point offset);
		void			remove_group				(nodes^ nds, links^ lnks);
		virtual	void	on_key_down					(Object^ , System::Windows::Forms::KeyEventArgs^ e) override;
		virtual	void	hypergraph_area_MouseDown	(Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual	void	hypergraph_area_MouseUp		(Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void	on_hypergraph_scroll		(Object^ , System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void	hypergraph_Paint			(System::Object^ , System::Windows::Forms::PaintEventArgs^ e) override;
				bool	can_make_connection			(animation_viewer_graph_node_base^ src, animation_viewer_graph_node_base^ dst);
	//	virtual void	do_layout					() override;

				links^	copy_links					();
				void	undo						();
				void	redo						();
				void	save						(xray::configs::lua_config_value cfg);
				void	save						(xray::configs::lua_config_value cfg, nodes^ nds, links^ lnks);
				void	clear_command_stack			();
				void	set_saved_flag				();
				void	on_property_changed			(wpf_property_container^ pc, System::String^ l, Object^ new_val, Object^ old_val);
				void	set_animation_node_new_intervals_list(animation_viewer_graph_node_base^ n, List<animation_node_interval^>^ lst, u32 cycle_index);
				void	cut							(System::Object^, System::EventArgs^);
				void	copy						(System::Object^, System::EventArgs^);
				void	paste						(System::Object^, System::EventArgs^);
				void	del							(System::Object^, System::EventArgs^);
				void	save_clip					(System::Object^, System::EventArgs^);
	command_engine^		get_command_engine			();

				u32		assign_node_id				(animation_viewer_graph_node_base^ n, u32 new_id);
				void	remove_node_id				(animation_viewer_graph_node_base^ n);
	viewer_document^	get_owner					() {return m_owner;};
	viewer_editor^		get_editor					();

	protected:
		virtual	void	drag_start					() override;
		virtual	void	drag_stop					() override;

	private: 
		command_engine^								m_command_engine;
		animation_viewer_move_nodes^				m_move_nodes_command;
		viewer_document^							m_owner;
		Point										m_start_move_point;

		System::Windows::Forms::ContextMenuStrip^	m_context_menu;
		System::Windows::Forms::ToolStripMenuItem^	cut_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	copy_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	paste_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	delete_menu_item;
		System::Windows::Forms::ToolStripMenuItem^	save_clip_item;
		System::Collections::Generic::List<u32>^	m_nodes_ids;
	}; // class animation_hypergraph
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_VIEWER_HYPERGRAPH_H_INCLUDED