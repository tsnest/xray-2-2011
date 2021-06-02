////////////////////////////////////////////////////////////////////////////
//	Created		: 22.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_SELECTOR_H_INCLUDED
#define RESOURCE_SELECTOR_H_INCLUDED

#include "texture_editor_source.h"
#include "editor_world.h"
#include "resource_editor_base.h"

using namespace System;
using namespace System::IO;
using namespace System::Windows;
using namespace System::Windows::Forms;
using namespace System::Drawing;

//using xray::editor::wpf_controls::property_grid_value_changed_event_args;
namespace xray {
	namespace editor {

		/// <summary>
		/// Summary for resource_selector
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class resource_selector : public System::Windows::Forms::Form
		{

		#pragma region | Inner Types |

		public:
			enum class resource_type
			{
				Texture
			};

			enum class entity
			{
				resource_name,
				resource_object
			};

		#pragma endregion

		#pragma region | Initialize |

		public:
			resource_selector(editor_world& world, resource_type resource_type):
				m_resource_type(resource_type)
			{
				InitializeComponent	();
				in_constructor		(world, nullptr);
			}

			resource_selector(editor_world& world, resource_type resource_type, Object^ filter):
				m_resource_type(resource_type)
			{
				InitializeComponent	();
				in_constructor		(world, filter);
			}
		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~resource_selector()
			{
				if(!m_is_disposed)
				{
					m_is_disposed = true;
					m_multidocument->free_resources		();
				}
			}
			!resource_selector()
			{
				this->~resource_selector();
			}

		private:
			void in_constructor(editor_world& world, Object^ filter);
		private: System::Windows::Forms::Panel^  panel1;
		private: System::Windows::Forms::Button^  m_cancel_button;
		private: System::Windows::Forms::Button^  m_ok_button;
		private: System::Windows::Forms::GroupBox^  groupBox1;

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
				this->panel1 = (gcnew System::Windows::Forms::Panel());
				this->m_ok_button = (gcnew System::Windows::Forms::Button());
				this->m_cancel_button = (gcnew System::Windows::Forms::Button());
				this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
				this->panel1->SuspendLayout();
				this->SuspendLayout();
				// 
				// panel1
				// 
				this->panel1->Controls->Add(this->m_ok_button);
				this->panel1->Controls->Add(this->m_cancel_button);
				this->panel1->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->panel1->Location = System::Drawing::Point(0, 532);
				this->panel1->Name = L"panel1";
				this->panel1->Size = System::Drawing::Size(670, 46);
				this->panel1->TabIndex = 0;
				// 
				// m_ok_button
				// 
				this->m_ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
				this->m_ok_button->Location = System::Drawing::Point(502, 11);
				this->m_ok_button->Name = L"m_ok_button";
				this->m_ok_button->Size = System::Drawing::Size(75, 23);
				this->m_ok_button->TabIndex = 0;
				this->m_ok_button->Text = L"OK";
				this->m_ok_button->UseVisualStyleBackColor = true;
				this->m_ok_button->Click += gcnew System::EventHandler(this, &resource_selector::m_ok_button_Click);
				// 
				// m_cancel_button
				// 
				this->m_cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
				this->m_cancel_button->DialogResult = System::Windows::Forms::DialogResult::Cancel;
				this->m_cancel_button->Location = System::Drawing::Point(583, 11);
				this->m_cancel_button->Name = L"m_cancel_button";
				this->m_cancel_button->Size = System::Drawing::Size(75, 23);
				this->m_cancel_button->TabIndex = 1;
				this->m_cancel_button->Text = L"Cancel";
				this->m_cancel_button->UseVisualStyleBackColor = true;
				// 
				// groupBox1
				// 
				this->groupBox1->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->groupBox1->Location = System::Drawing::Point(0, 525);
				this->groupBox1->Margin = System::Windows::Forms::Padding(10, 3, 10, 3);
				this->groupBox1->Name = L"groupBox1";
				this->groupBox1->Padding = System::Windows::Forms::Padding(10, 3, 10, 3);
				this->groupBox1->Size = System::Drawing::Size(670, 7);
				this->groupBox1->TabIndex = 1;
				this->groupBox1->TabStop = false;
				// 
				// resource_selector
				// 
				this->AcceptButton = this->m_ok_button;
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->CancelButton = this->m_cancel_button;
				this->ClientSize = System::Drawing::Size(670, 578);
				this->Controls->Add(this->groupBox1);
				this->Controls->Add(this->panel1);
				this->Name = L"resource_selector";
				this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
				this->Text = L"resource_selector";
				this->Load += gcnew System::EventHandler(this, &resource_selector::resource_selector_Load);
				this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &resource_selector::resource_selector_FormClosing);
				this->panel1->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion
		#pragma endregion

		#pragma region |   Fields   |

		private:
			Boolean									m_is_disposed;
			String^									m_track_resource_name;
			array<String^>^							m_track_resource_names;
			Boolean									m_is_resource_list_loaded;
			Boolean									m_self_value_change;
			Boolean									m_resources_loaded;
			Boolean									m_commit_on_loaded;
			resource_type 							m_resource_type;
			resource_editor_base^					m_multidocument;

		#pragma endregion

		#pragma region | Properties |

		public:
			property String^			name
			{
				String^				get()				{return m_multidocument->Name;}
				void				set(String^ value)	{m_multidocument->Name = value;}
			}
			property String^			selected_name
			{
				String^				get();
				void				set(String^ value);
			}

			property resources::unmanaged_resource_ptr		selected_resource
			{
				resources::unmanaged_resource_ptr		get();
			}
// 			property String^			selected_resources
// 			{
// 				array<String^>^		get()				{return m_multidocument->view_panel->tree_view->SelectedNode->FullPath;}
// 				void				set(array<String^>^ value);
// 			}
			property entity				selecting_entity;			

		#pragma endregion

		#pragma region |   Methods  |
		
			private:
				void	resources_list_loaded			(Object^ sender, EventArgs^ e);
				void	resources_loaded				(Object^ sender, EventArgs^ e);
				void	resource_selector_Load			(Object^ sender, EventArgs^ e);
				void	m_ok_button_Click				(Object^ sender, EventArgs^ e);
				void	document_created				(Object^ sender, controls::document_event_args^ e);
				void	resource_selector_FormClosing	(Object^ sender, FormClosingEventArgs^ e);
				void	tree_view_node_double_click		(Object^ sender, TreeNodeMouseClickEventArgs^ e);
				void	tree_view_selection_changed		(Object^ sender, TreeViewEventArgs^ e);

		#pragma endregion
		}; // class resource_selector

		struct resource_selector_ptr
		{
			resource_selector_ptr(editor_world& w, resource_selector::resource_type t){
					m_selector = gcnew resource_selector(w, t);
			}
			resource_selector_ptr(editor_world& w, resource_selector::resource_type t, gcroot<Object^> filter){
					m_selector = gcnew resource_selector(w, t, filter);
			}

			gcroot<resource_selector^> m_selector;

			~resource_selector_ptr()
			{
				delete m_selector;
			}

			resource_selector^ operator ->()
			{
				return m_selector;
			}
		};
	} // namespace editor
} // namespace xray
#endif // #ifndef RESOURCE_SELECTOR_H_INCLUDED