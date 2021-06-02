////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_GRAPH_TAB_H_INCLUDED
#define SCENE_GRAPH_TAB_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {

	ref class level_editor;

	ref class link_storage;
	ref class scene_graph_node;
	ref class object_base;

	/// <summary>
	/// Summary for scene_graph_tab
	/// </summary>
	public ref class scene_graph_tab : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		scene_graph_tab(level_editor^ le):m_level_editor(le)
		{
			InitializeComponent();
			in_constructor();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~scene_graph_tab()
		{
			in_destructor	();
			if (components)
			{
				delete components;
			}
		}
	private: 
		xray::editor::controls::hypergraph::hypergraph_control^  m_hypergraph;
		level_editor^		m_level_editor;
		void 				in_constructor	();
		void				in_destructor	();

	public: 
		void		initialize	();
		void		reset		();

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->m_hypergraph = (gcnew xray::editor::controls::hypergraph::hypergraph_control());
			this->SuspendLayout();
			// 
			// m_hypergraph
			// 
			this->m_hypergraph->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->m_hypergraph->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->m_hypergraph->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->m_hypergraph->connection_editor = nullptr;
			this->m_hypergraph->current_view_mode = xray::editor::controls::hypergraph::view_mode::full;
			this->m_hypergraph->focused_link = nullptr;
			this->m_hypergraph->focused_node = nullptr;
			this->m_hypergraph->Location = System::Drawing::Point(2, 27);
			this->m_hypergraph->move_mode = xray::editor::controls::hypergraph::nodes_movability::movable;
			this->m_hypergraph->Name = L"m_hypergraph";
			this->m_hypergraph->node_size_brief = System::Drawing::Size(80, 40);
			this->m_hypergraph->Size = System::Drawing::Size(329, 183);
			this->m_hypergraph->TabIndex = 0;
			this->m_hypergraph->visibility_mode = xray::editor::controls::hypergraph::nodes_visibility::all;
			this->m_hypergraph->on_selection_changed += gcnew xray::editor::controls::hypergraph::selection_changed(this, &scene_graph_tab::m_hypergraph_on_selection_changed);
			// 
			// scene_graph_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(365, 222);
			this->Controls->Add(this->m_hypergraph);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"scene_graph_tab";
			this->TopMost = true;
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void m_hypergraph_on_selection_changed();
	};

ref class scene_links_manager : public xray::editor::controls::hypergraph::links_manager
{
public:
	typedef controls::hypergraph::node					node;
	typedef controls::hypergraph::link					link;
	typedef controls::hypergraph::connection_point		connection_point;
	typedef Generic::List<link^>						links;
	typedef Generic::List<node^>						nodes;
private:
	links^												m_links;
	controls::hypergraph::hypergraph_control^			m_hypergraph;
	link_storage^										m_link_storage;
	scene_graph_node^					find_node		(object_base^ o);

public:
										scene_links_manager(controls::hypergraph::hypergraph_control^ h, link_storage^ s);
	virtual void						on_node_added	(node^ node);
	virtual void						on_node_removed	(node^ node);
	virtual void						on_node_destroyed(node^ node);
	virtual void						create_link		(controls::hypergraph::connection_point^ pt_src, controls::hypergraph::connection_point^ pt_dst);
	virtual void						destroy_link	(controls::hypergraph::link^ link);
	virtual links^						visible_links	();
	virtual void						clear			();
};


}; //namespace editor
}; //namespace xray

#endif // #ifndef SCENE_GRAPH_TAB_H_INCLUDED
