////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef HYPERGRAPH_NODE_H_INCLUDED
#define HYPERGRAPH_NODE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#include "hypergraph_defines.h"
#include "hypergraph_node_style.h"

namespace xray {
namespace editor {
namespace controls {
namespace hypergraph {

	ref class node;
	ref class hypergraph_control;
	ref class connection_point;
	ref class link;
	ref class node_style;
	/// <summary>
	/// Summary for node
	/// </summary>
	public ref class node : public System::Windows::Forms::UserControl
	{
	public:
		node(void)
		{
			InitializeComponent	();
			in_constructor		();
			//
			//TODO: Add the constructor code here
			//
		}
	//- node interface
	protected:
		void					in_constructor					();

		hypergraph_control^		m_parent;
		System::Drawing::Brush^ select_brush					(connection_point^ p, Point mouse_pos);
		void					set_focused_connection_point_impl(connection_point^ p);
		connection_point^		m_focused_connection_point;

		node_style^				m_style;
		Drawing::Size			m_size;
		Point					m_position;
		void					set_position_impl				(Point p);
		virtual	void			recalc							();

	public:
		virtual void	on_added				(hypergraph_control^ parent);
		virtual void	on_removed				(hypergraph_control^ parent);
		virtual void	assign_style			(node_style^ style);

		void			set_view_mode			(view_mode m);
		node_style^		get_style				()								{return m_style;};
		hypergraph_control^	get_parent			()								{return m_parent;};
		connection_point^	add_connection_point(System::Type^ t, String^ name);
		connection_point^	pick_connection_point(Point pt);
		connection_point^	get_connection_point(String^ name);
		virtual String^	caption					() {return "node";}
		virtual void	on_focus				(bool b_enter);
		virtual void	on_paint				(System::Windows::Forms::PaintEventArgs^ e);
		virtual void	draw_frame				(System::Windows::Forms::PaintEventArgs^ e);
		virtual void	draw_caption			(System::Windows::Forms::PaintEventArgs^ e);
		virtual void	draw_connection_points	(System::Windows::Forms::PaintEventArgs^ e);
		virtual void	on_connection_made		(link^ link) {};
		virtual void	on_connection_destroyed	(link^ link) {};
		virtual void	on_destroyed			(){};
		virtual void	select_parents			();

		property ArrayList^	connection_points{
			ArrayList^	get() {return m_connection_points;}
		};

		property connection_point^			focused_connection_point{
			connection_point^		get()						{return m_focused_connection_point;}
			void					set(connection_point^ p)	{set_focused_connection_point_impl(p);}
		}
		property hypergraph_control^		parent_hypergraph{
			hypergraph_control^		get()						{return m_parent;}
			void					set(hypergraph_control^ value)	{m_parent = value;}				
		}

		property bool				movable;
		
		System::Drawing::Point	get_point_location	(connection_point^ pt);
		System::Drawing::Point	get_point_location	(connection_type t);

		void					set_scale(float scl);
		float					get_scale()							{return m_style->scale;};

		property Point			position{
			Point				get()								{return m_position;}
			void				set(Point p)						{set_position_impl(p);}
		}

		property Drawing::Size	size{
			Drawing::Size		get()								{return m_size;}
			void				set(Drawing::Size value);
		}
		
		event position_changed^ m_on_position_changed;
	protected:
		System::Collections::ArrayList^		m_connection_points;

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~node()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// hypergraph_node
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::Window;
			this->Name = L"node";
			this->ResizeRedraw = true;
			this->DoubleBuffered = true;
			this->Size = System::Drawing::Size(120, 35);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &node::hypergraph_node_Paint);
			this->MouseEnter += gcnew System::EventHandler(this, &node::hypergraph_node_MouseEnter);
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &node::hypergraph_node_MouseDown);
			this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &node::hypergraph_node_MouseUp);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &node::hypergraph_node_MouseMove);
			this->ResumeLayout(false);
		}
#pragma endregion
	private: System::Void hypergraph_node_MouseEnter(System::Object^  sender, System::EventArgs^  e);
	private: System::Void hypergraph_node_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	public: System::Void hypergraph_node_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	private: System::Void hypergraph_node_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e);
	private: System::Void hypergraph_node_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e){on_paint(e);}
	};

} //namespace namespace hypergraph
} //namespace controls
} //namespace editor
} //namespace xray

#endif // #ifndef HYPERGRAPH_NODE_H_INCLUDED