////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOD_VIEW_PANEL_H_INCLUDED
#define LOD_VIEW_PANEL_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace model_editor {
	ref class model_editor;
	/// <summary>
	/// Summary for lod_view_panel
	/// </summary>
	ref class lod_view_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
		typedef xray::editor::wpf_controls::hypergraph::link_event_args link_event_args;
		typedef xray::editor::wpf_controls::hypergraph::node_event_args node_event_args;
		typedef xray::editor::wpf_controls::hypergraph::selection_event_args selection_event_args;
	public:
		lod_view_panel( )
		{
			InitializeComponent();
			in_constructor();			
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~lod_view_panel()
		{
			if (components)
			{
				delete components;
			}
		}
	public: xray::editor::wpf_controls::hypergraph::hypergraph_host^  hypergraph_host1;
	protected: 

	protected: 

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
			this->hypergraph_host1 = (gcnew xray::editor::wpf_controls::hypergraph::hypergraph_host());
			this->SuspendLayout();
			// 
			// hypergraph_host1
			// 
			this->hypergraph_host1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->hypergraph_host1->Location = System::Drawing::Point(0, 0);
			this->hypergraph_host1->Name = L"hypergraph_host1";
			this->hypergraph_host1->Size = System::Drawing::Size(562, 439);
			this->hypergraph_host1->TabIndex = 0;
			this->hypergraph_host1->Text = L"hypergraph_host1";
			this->hypergraph_host1->Child = nullptr;
			// 
			// lod_view_panel
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->ClientSize = System::Drawing::Size(562, 439);
			this->Controls->Add(this->hypergraph_host1);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"lod_view_panel";
			this->Text = L"LOD view";
			this->ResumeLayout(false);

		}
#pragma endregion
	private:
	void				in_constructor			( );
	};
} // namespace model_editor
} // namespace xray

#endif // #ifndef LOD_VIEW_PANEL_H_INCLUDED