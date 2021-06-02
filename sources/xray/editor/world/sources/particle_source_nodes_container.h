////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SOURCE_NODES_CONTAINER_H_INCLUDED
#define PARTICLE_SOURCE_NODES_CONTAINER_H_INCLUDED

#include "particle_graph_node.h"


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	/// <summary>
	/// Summary for particle_source_nodes_container
	/// </summary>
	public ref class particle_source_nodes_container : public System::Windows::Forms::UserControl
	{
		typedef System::Collections::Generic::List<particle_graph_node^> particle_nodes;
	public:
		particle_source_nodes_container(String^ text)
		{
			InitializeComponent(text);
			in_constructor();		
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~particle_source_nodes_container()
		{
// 			if (m_particle_source_nodes->Count > 0){
// 				for each(particle_graph_node^ n in m_particle_source_nodes)
// 				{
// 					delete n;
// 				}
// 			}
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container	^components;
		particle_nodes^						m_particle_source_nodes;
		bool								m_enabled;
		ImageList^							m_images;

	public:
		property particle_nodes^			particle_source_nodes{
			particle_nodes^					get(){return m_particle_source_nodes;}
		}
		property bool						enabled {
			bool							get(){return m_enabled;}
			void							set(bool value) {m_enabled = value;}
		}

		

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(String^ text)
		{
			this->SuspendLayout();
			// 
			// particle_source_nodes_container
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->AutoSize = false;		
			this->Name = text;
			this->Text = text;
			this->ForeColor = Drawing::Color::Black;
			this->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			this->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->Dock	= DockStyle::Fill;
			this->Anchor = System::Windows::Forms::AnchorStyles::Left | System::Windows::Forms::AnchorStyles::Right | System::Windows::Forms::AnchorStyles::Top;
			this->ResumeLayout(false);			

		}
#pragma endregion

	private:
					void								in_constructor();
	public:
			virtual void								OnPaint	(PaintEventArgs^ e) override;


	};
}
}
#endif // #ifndef PARTICLE_SOURCE_NODES_CONTAINER_H_INCLUDED
