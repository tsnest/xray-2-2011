//-------------------------------------------------------------------------------------------
//	Created		: 23.12.2009
//	Author		: Alexander Plichko
//	Copyright (C) GSC Game World - 2009
//-------------------------------------------------------------------------------------------
#ifndef PARTICLE_EDITOR_H_INCLUDED
#define PARTICLE_EDITOR_H_INCLUDED

#include "editor_world.h"
#include "particle_time_based_layout_panel.h"
#include "particle_data_sources_panel.h"


using namespace System;
using namespace System::IO;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI::Docking;

using namespace xray::editor::controls;

namespace xray {
	namespace editor {

		class editor_world;
		ref class particle_graph_document;
		ref class particle_graph_node_style_mgr;
		ref class particle_sources_panel;

		template<class t_type>
		interface class i_cloneable
		{
			t_type^ clone();
		};

		/// <summary>
		/// Summary for particle_editor
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class particle_editor : public System::Windows::Forms::Form
		{

		#pragma region | InnerTypes |

// 		public:
// 			ref struct node_type
// 			{
// 				node_type(String^ set_type_name):type_name(set_type_name){}
// 
// 				String^ type_name;
// 			};

		#pragma endregion

		#pragma region | Initialize |

		public:
			particle_editor(editor_world& w):
			  m_editor_world(w)
			{
				particle_resources_path				= "resources";
				absolute_particle_resources_path	= Path::GetFullPath(gcnew String(m_editor_world.engine().get_resource_path())+"/");

				InitializeComponent();
				in_constructor();
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~particle_editor();
		

		private:
			void								in_constructor();
			System::ComponentModel::Container^	components;
		

		#pragma region Windows Form Designer generated code

		protected:
				 /// <summary>
				 /// Required method for Designer support - do not modify
				 /// the contents of this method with the code editor.
				 /// </summary>
				 void InitializeComponent()
				 {
					 this->SuspendLayout();
					 // 
					 // particle_editor
					 // 
					 this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					 this->ClientSize = System::Drawing::Size(382, 396);
					 this->Name = L"particle_editor";
					 this->Text = L"particle_editor";
					 this->Activated += gcnew System::EventHandler(this, &particle_editor::particle_editor_Activated);
					 this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &particle_editor::particle_editor_FormClosing);
					
					 this->ResumeLayout(false);

				 }

		#pragma endregion

		#pragma endregion

		#pragma region |   Fields  |

		private:
			static String^									s_default_file_name = "new_particle";
			static String^									s_default_folder_name = "new_folder";
			static String^									s_file_extension	= ".particle";
			editor_world&									m_editor_world;

			document_editor_base^							m_multidocument_base;
			particle_graph_node_style_mgr^					m_node_style_mgr;
			particle_sources_panel^							m_nodes_panel;
			particle_time_based_layout_panel^				m_time_layout_panel;
			particle_data_sources_panel^					m_data_sources_panel;
			
		public:
			static String^									particle_resources_path;
			static String^									absolute_particle_resources_path;

		#pragma endregion

			property document_editor_base^					multidocument_base{
				document_editor_base^						get(){return m_multidocument_base;}
			}
			property particle_graph_node_style_mgr^			node_style_mgr{
				particle_graph_node_style_mgr^				get(){return m_node_style_mgr;}
			}
			property particle_time_based_layout_panel^		time_based_layout_panel{
				particle_time_based_layout_panel^			get(){return m_time_layout_panel;}
			}
			property particle_sources_panel^				node_sources_panel{
				particle_sources_panel^						get(){return m_nodes_panel;}
			}
			property particle_data_sources_panel^			data_sources_panel{
				particle_data_sources_panel^				get(){return m_data_sources_panel;}
			}


		#pragma region |  Methods  |

		private:
			void			dock_panels						();
			void			particle_editor_FormClosing		(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
			void			exitToolStripMenuItem_Click		(System::Object^  sender, System::EventArgs^  e) ;

			void			on_file_view_double_click		(Object^ sender, TreeNodeMouseClickEventArgs^ e);
			void			on_view_label_edit				(Object^ sender, NodeLabelEditEventArgs^ e);
			void 			on_new_document					(Object^ sender, EventArgs^ e);
			void 			on_new_folder					(Object^ sender, EventArgs^ e);
			void 			on_view_new_document			(Object^ sender, EventArgs^ e);
			void 			on_view_remove_document			(Object^ sender, EventArgs^ e);
			void 			on_view_new_folder				(Object^ sender, EventArgs^ e);
			void			on_node_style_mgr_loaded		(Object^ sender, EventArgs^ e);
			void			tree_loaded						(Object^ sender, EventArgs^ e);
			document_base^	on_create_document				();
			void			particle_editor_Activated		(System::Object^  sender, System::EventArgs^  e);

		public:
			void							manual_close					();
			
		#pragma endregion

}; //class particle_editor
	} // namespace xray 
} // namespace editor 
#endif // #ifndef PARTICLE_EDITOR_H_INCLUDED
