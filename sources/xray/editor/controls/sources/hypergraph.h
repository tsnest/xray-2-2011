////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef HYPERGRAPH_H_INCLUDED
#define HYPERGRAPH_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "hypergraph_defines.h"
#include "scalable_scroll_bar.h"

namespace xray {
namespace editor {
namespace controls {
namespace hypergraph {

ref class connection_point;
ref class node;
ref class link;
interface class connection_editor;
interface class links_manager;
interface class nodes_style_manager;

public delegate void selection_changed ();

	/// <summary>
	/// Summary for hypergraph_control
	/// </summary>
	public ref class hypergraph_control : public System::Windows::Forms::UserControl
	{
	public:
		hypergraph_control(void)
		{
			InitializeComponent();
			in_constructor();
		}
		virtual	void	append_node					(node^ node);
		virtual	void	remove_node					(node^ node);
				void	clear						();

		property nodes_movability			move_mode;
		property nodes_visibility			visibility_mode;
		property System::Drawing::Size		node_size_brief;
		property nodes_link_style			link_style;

		property float						scale
		{
			float				get()						{return m_scale;};
			void				set(float sc)				{change_scale(sc-m_scale);};
		};
	
		event selection_changed^			on_selection_changed;

		property view_mode		current_view_mode{
			view_mode			get()						{return m_view_mode;}
			void				set(view_mode m)			{set_view_mode_impl(m);}
		}
		
		nodes^					all_nodes			() {return m_nodes;}

		property node^			focused_node{
			node^				get()				{return m_focused_node;}
			void				set(node^ n)		{set_focused_node_impl(n);}
		}
		property nodes^			selected_nodes{
			nodes^				get()				{return m_selected_nodes;}
		}
				void			add_selected_node	(nodes^ n);
				void			add_selected_node	(node^ n);
				void			set_selected_node	(nodes^ n);
				void			set_selected_node	(node^ n);
				void			destroy_connections	(links^ link);
		virtual	void			destroy_connection	(link^ lnk);
				void			destroy_nodes		(nodes^ node);
				void			destroy_node		(node^ node);

				bool			is_start_shift_node	(node^ n)				{return n==m_start_shift_node;};

		property link^			focused_link{
			link^				get()				{return m_focused_link;}
			void				set(link^ link)		{if(m_focused_link==link)return; m_focused_link=link; Invalidate(false);}
		}
		property links^			selected_links{
			links^				get()				{return m_selected_links;}
		}
		property Drawing::Rectangle display_rectangle
		{
			Drawing::Rectangle	get(){return m_display_rectangle;}
			void				set(Drawing::Rectangle value);
		}
		property connection_editor^		connection_editor;
		property links_manager^			links_manager;
		property nodes_style_manager^	nodes_style_manager;

		property float			min_scale;
		property float			max_scale;
		property bool			scalable;
		property bool			show_scrollbars;


		virtual	void	do_layout					();

		virtual	bool	can_make_connection			(connection_point^ p1, connection_point^ p2);
		virtual	void	make_connection				(connection_point^ psrc, connection_point^ pdst);
				node^	get_node_under_cursor		();
				links^	get_links_under_cursor		();
	protected:
				nodes^	get_nodes					(Drawing::Rectangle selection_rect);
				link^	get_link_under_cursor		();
				links^	get_links					(Drawing::Rectangle selection_rect);

				void	set_focused_node_impl		(node^ node);
				void	select_src_connection		(node^ node, connection_point^ point);
				void	set_view_mode_impl			(view_mode m);
				void	set_scale_impl				(float new_scale);
		virtual	void	change_scale				(float scale);

		virtual	void	draw_links_full				(System::Windows::Forms::PaintEventArgs^  e);
		virtual	void	draw_links_brief			(System::Windows::Forms::PaintEventArgs^  e);
		virtual	void	draw_rt_connection			(System::Windows::Forms::PaintEventArgs^  e);
		virtual	void	drag_start					();
		virtual	void	drag_stop					();
		virtual	void	drag_update					();

				void	move_start					();
				void	move_stop					();
				void	move_update					();

				void	scale_start					();
				void	scale_stop					();
				void	scale_update				();

				void	recalc_scrolls				();
				void	recalc_outer_field			();
		virtual void	OnControlAdded				(ControlEventArgs^ e) override;
		virtual void	OnResize					(EventArgs^ e) override;
				void	h_scroll_change				(Object^ sender, scroller_event_args^ e);
				void	v_scroll_change				(Object^ sender, scroller_event_args^ e);
		virtual void	OnPaint						(PaintEventArgs^ e) override;
				void	draw_arrow					(Point pt_start, Point pt_end, Pen^ pen, Graphics^ g);

	public:
				
				void	set_node_location			(node^ nod);
				void	focus_on_node				(node^ nod);
				void	center_nodes				();

	protected:

		nodes^								m_nodes;
		Boolean								m_need_recalc;

		Drawing::Pen^						m_link_pen;
		Drawing::Pen^						m_selection_rect_pen;
		view_mode							m_view_mode;

		//-- focused
		node^								m_focused_node;
		link^								m_focused_link;
		//-- focused

		// ---rt connection
		connection_point^					m_src_conn_point;
		node^								m_src_conn_node;
		node^								m_start_shift_node;
		// ---rt connection

		// --selection
		connection_point^					m_selected_conn_point;
		nodes^								m_selected_nodes;
		links^								m_selected_links;
		// --selection
		bool								m_b_rect_selection_mode;
		bool								m_b_drag_mode;
		Point								m_selection_mode_start_point;
		// --scaling and moving graph
		Int32								m_inner_to_outer_distance;
		Drawing::Rectangle					m_display_rectangle;
		Drawing::Rectangle					m_inner_field_rect;
		Drawing::Rectangle					m_outer_field_rect;
		float								m_scale;
		bool								m_b_graph_move_mode;
		bool								m_b_graph_scale_mode;
		xray::editor::controls::scalable_scroll_bar^  m_h_scroll;
private: System::Windows::Forms::Panel^  m_h_scroll_bar_container;
private: System::Windows::Forms::Panel^  m_corner_gizmo;

protected: 
	xray::editor::controls::scalable_scroll_bar^  m_v_scroll;

	protected:
		~hypergraph_control()
		{
			if (components)
				delete components;
		}
		void in_constructor();

	private:
		System::ComponentModel::IContainer^  components;

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->m_v_scroll = (gcnew xray::editor::controls::scalable_scroll_bar());
			this->m_h_scroll = (gcnew xray::editor::controls::scalable_scroll_bar());
			this->m_h_scroll_bar_container = (gcnew System::Windows::Forms::Panel());
			this->m_corner_gizmo = (gcnew System::Windows::Forms::Panel());
			this->m_h_scroll_bar_container->SuspendLayout();
			this->SuspendLayout();
			// 
			// m_v_scroll
			// 
			this->m_v_scroll->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->m_v_scroll->direction = xray::editor::controls::scalable_scroll_bar::Direction::Vertical;
			this->m_v_scroll->Dock = System::Windows::Forms::DockStyle::Right;
			this->m_v_scroll->Location = System::Drawing::Point(400, 0);
			this->m_v_scroll->MaximumSize = System::Drawing::Size(24, 10000);
			this->m_v_scroll->MinimumSize = System::Drawing::Size(24, 2);
			this->m_v_scroll->Name = L"m_v_scroll";
			this->m_v_scroll->scroll_max = 400;
			this->m_v_scroll->scroll_min = 0;
			this->m_v_scroll->scroller_position = 0;
			this->m_v_scroll->scroller_width = 400;
			this->m_v_scroll->Size = System::Drawing::Size(24, 424);
			this->m_v_scroll->TabIndex = 1;
			this->m_v_scroll->scroller_moved += gcnew System::EventHandler<xray::editor::controls::scroller_event_args^ >(this, &hypergraph_control::v_scroll_change);
			this->m_v_scroll->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_up);
			this->m_v_scroll->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_down);
			// 
			// m_h_scroll
			// 
			this->m_h_scroll->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->m_h_scroll->direction = xray::editor::controls::scalable_scroll_bar::Direction::Horizontal;
			this->m_h_scroll->Dock = System::Windows::Forms::DockStyle::Fill;
			this->m_h_scroll->Location = System::Drawing::Point(0, 0);
			this->m_h_scroll->Name = L"m_h_scroll";
			this->m_h_scroll->scroll_max = 400;
			this->m_h_scroll->scroll_min = 0;
			this->m_h_scroll->scroller_position = 0;
			this->m_h_scroll->scroller_width = 400;
			this->m_h_scroll->Size = System::Drawing::Size(376, 24);
			this->m_h_scroll->TabIndex = 0;
			this->m_h_scroll->scroller_moved += gcnew System::EventHandler<xray::editor::controls::scroller_event_args^ >(this, &hypergraph_control::h_scroll_change);
			this->m_h_scroll->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_up);
			this->m_h_scroll->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_down);
			// 
			// m_h_scroll_bar_container
			// 
			this->m_h_scroll_bar_container->Controls->Add(this->m_h_scroll);
			this->m_h_scroll_bar_container->Controls->Add(this->m_corner_gizmo);
			this->m_h_scroll_bar_container->Dock = System::Windows::Forms::DockStyle::Bottom;
			this->m_h_scroll_bar_container->Location = System::Drawing::Point(0, 400);
			this->m_h_scroll_bar_container->MaximumSize = System::Drawing::Size(10000, 24);
			this->m_h_scroll_bar_container->MinimumSize = System::Drawing::Size(0, 24);
			this->m_h_scroll_bar_container->Name = L"m_h_scroll_bar_container";
			this->m_h_scroll_bar_container->Size = System::Drawing::Size(400, 24);
			this->m_h_scroll_bar_container->TabIndex = 2;
			// 
			// m_corner_gizmo
			// 
			this->m_corner_gizmo->Dock = System::Windows::Forms::DockStyle::Right;
			this->m_corner_gizmo->Location = System::Drawing::Point(376, 0);
			this->m_corner_gizmo->MaximumSize = System::Drawing::Size(24, 24);
			this->m_corner_gizmo->MinimumSize = System::Drawing::Size(24, 24);
			this->m_corner_gizmo->Name = L"m_corner_gizmo";
			this->m_corner_gizmo->Size = System::Drawing::Size(24, 24);
			this->m_corner_gizmo->TabIndex = 3;
			// 
			// hypergraph_control
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->Controls->Add(this->m_v_scroll);
			this->Controls->Add(this->m_h_scroll_bar_container);
			this->DoubleBuffered = true;
			this->Name = L"hypergraph_control";
			this->Size = System::Drawing::Size(424, 424);
			this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &hypergraph_control::on_hypergraph_scroll);
			this->MouseLeave += gcnew System::EventHandler(this, &hypergraph_control::hypergraph_area_MouseLeave);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &hypergraph_control::hypergraph_Paint);
			this->Click += gcnew System::EventHandler(this, &hypergraph_control::onclick);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &hypergraph_control::hypergraph_area_MouseMove);
			this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_up);
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &hypergraph_control::hypergraph_area_MouseDown);
			this->Resize += gcnew System::EventHandler(this, &hypergraph_control::hypergraph_Resize);
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &hypergraph_control::hypergraph_area_MouseUp);
			this->MouseEnter += gcnew System::EventHandler(this, &hypergraph_control::hypergraph_area_MouseEnter);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &hypergraph_control::on_key_down);
			this->m_h_scroll_bar_container->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion


	private: System::Void onclick(System::Object^  sender, System::EventArgs^  e);


	private: System::Void hypergraph_area_MouseEnter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void hypergraph_area_MouseLeave(System::Object^  sender, System::EventArgs^  e);
	private: System::Void hypergraph_Resize(System::Object^  sender, System::EventArgs^  e);
	public:
		virtual void hypergraph_area_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		virtual void hypergraph_area_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		virtual void hypergraph_area_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
		virtual void on_key_down(System::Object^ , System::Windows::Forms::KeyEventArgs^ e);
		virtual void on_key_up(System::Object^ , System::Windows::Forms::KeyEventArgs^ e);
		virtual void on_hypergraph_scroll(System::Object^ , System::Windows::Forms::MouseEventArgs^ ) {};
		virtual void hypergraph_Paint(System::Object^ sender, System::Windows::Forms::PaintEventArgs^ e);
};

public interface class connection_editor
{
public:
	virtual void				execute(node^ source, node^ destinastion);
};

public interface class links_manager
{
public:
	virtual void				on_node_added		(node^ node);
	virtual void				on_node_removed		(node^ node);
	virtual void				on_node_destroyed	(node^ node);
	virtual void				create_link			(connection_point^ pt_src, connection_point^ pt_dst);
	virtual void				destroy_link		(link^ link);
	virtual links^				visible_links		();
	virtual void				clear				();
};

ref class links_manager_default : public links_manager
{
	links^						m_links;
	hypergraph_control^			m_hypergraph;
public:
								links_manager_default(hypergraph_control^ h);
	virtual void				on_node_added		(node^ node);
	virtual void				on_node_removed		(node^ node);
	virtual void				on_node_destroyed	(node^ node);
	virtual void				create_link			(connection_point^ pt_src, connection_point^ pt_dst);
	virtual void				destroy_link		(link^ link);
	virtual links^				visible_links		();
	virtual void				clear				();
};

} // namespace hypergraph
} // namespace controls
} // namespace editor 
} // namespace xray

#endif // #ifndef HYPERGRAPH_H_INCLUDED