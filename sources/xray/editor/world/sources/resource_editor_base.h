////////////////////////////////////////////////////////////////////////////
//	Created		: 22.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_EDITOR_BASE_H_INCLUDED
#define RESOURCE_EDITOR_BASE_H_INCLUDED

#include "editor_world.h"
#include "resource_options.h"
#include "resource_editor_options_wrapper.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using xray::editor::wpf_controls::property_grid_value_changed_event_args;

namespace xray
{
	namespace editor
	{
		interface class resource_document_factory;
		/// <summary>
		/// Summary for resource_editor_base
		/// </summary>
		public ref class resource_editor_base : public controls::document_editor_base
		{

		#pragma region |   Events  |

		public:
			event	EventHandler^		resource_list_loaded;
			event	EventHandler^		resource_editor_closing;

		#pragma endregion

		#pragma region | Initialize |
		public:
			resource_editor_base(editor_world& world, resource_document_factory^ document_factory, controls::tree_view_source^ resources_source):
				document_editor_base("resource_editor"),
				m_editor_world(world),
				m_resources_source(resources_source),
				m_document_factory(document_factory)
			{
				InitializeComponent();
				//
				//TODO: Add the constructor code here
				//
				in_constructor();
			}
			resource_editor_base(editor_world& world, resource_document_factory^ document_factory, controls::tree_view_source^ resources_source, Boolean show_main_menu):
				document_editor_base("resource_editor", show_main_menu),
				m_editor_world(world),
				m_resources_source(resources_source),
				m_document_factory(document_factory)
			{
				InitializeComponent();
				//
				//TODO: Add the constructor code here
				//
				in_constructor();
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>

		private:
			void in_constructor();
			/// <summary>
			/// Required designer variable.
			/// </summary>

		#pragma region Windows Form Designer generated code
			
		#pragma endregion

		#pragma endregion

		#pragma region | Fields |

		private:
			editor_world&							m_editor_world;
			controls::tree_view_source^				m_resources_source;

			ImageList^								m_images;
			Dictionary<String^, resource_editor_options_wrapper^>^	m_opened_resources;
			Dictionary<String^, resource_editor_options_wrapper^>^	m_changed_resources;
			List<resource_editor_options_wrapper^>^					m_selected_resources;
		private: System::Windows::Forms::ContextMenuStrip^			m_revertContextMenuStrip;



		private: System::Windows::Forms::ToolStripMenuItem^  revertToolStripMenuItem;
		private: System::Windows::Forms::ToolStripMenuItem^  revert_toolStripMenuItem;
		private: System::Windows::Forms::ToolStripMenuItem^  applyToolStripMenuItem;

		public:
			resource_document_factory^				m_document_factory;

		#pragma endregion

		#pragma region | Properties |

		public:
			property Boolean			need_load_panels_from_reestr;

			property String^ view_panel_caption
			{
				String^		get()				{return view_panel->Text;}
				void		set(String^ value)	{view_panel->Text = value;}
			}
			property String^ properties_panel_caption
			{
				String^		get()				{return properties_panel->Text;}
				void		set(String^ value)	{properties_panel->Text = value;}
			}

			property editor_world&		world
			{
				editor_world&				get() {return m_editor_world;}
			}
			property Collections::Generic::Dictionary<String^, resource_editor_options_wrapper^>^ changed_resources
			{
				Collections::Generic::Dictionary<String^, resource_editor_options_wrapper^>^		get() {return m_changed_resources;}
			}
			property Collections::Generic::List<resource_editor_options_wrapper^>^ selected_resources
			{
				Collections::Generic::List<resource_editor_options_wrapper^>^		get() {return m_selected_resources;}
			}

		#pragma endregion

		#pragma region |   Methods  |

		private:
			void						on_resource_selected			(Object^ sender, EventArgs^ e);
			void						on_property_value_changed		(Object^ sender, property_grid_value_changed_event_args^ e);
			void						on_item_list_loaded				(Object^ sender, EventArgs^ e);
			void						revertToolStripMenuItem_Click	(Object^ sender, EventArgs^ e);
			void						applyToolStripMenuItem_Click	(Object^ sender, EventArgs^ e);

		protected:
			controls::document_base^	on_document_creating			();

		public:
			virtual Boolean				load_panels						(Form^ parent, Boolean create_base_panel_objects) override;
					void				set_options_and_holder			(const resources::unmanaged_resource_ptr& options_ptr, xray::editor_base::property_holder* holder);
					Boolean				is_resource_loaded				(String^ resource_name);
					resource_editor_options_wrapper^	get_loaded_resource				(String^ resource_name);
					void				free_resources					();


		#pragma endregion

		protected:
			void InitializeComponent() new {
				this->components = (gcnew System::ComponentModel::Container());
				this->m_revertContextMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
				this->revert_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->applyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->m_revertContextMenuStrip->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_revertContextMenuStrip
				// 
				this->m_revertContextMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->applyToolStripMenuItem, 
					this->revert_toolStripMenuItem});
				this->m_revertContextMenuStrip->Name = L"contextMenuStrip";
				this->m_revertContextMenuStrip->Size = System::Drawing::Size(153, 70);
				// 
				// revert_toolStripMenuItem
				// 
				this->revert_toolStripMenuItem->Name = L"revert_toolStripMenuItem";
				this->revert_toolStripMenuItem->Size = System::Drawing::Size(152, 22);
				this->revert_toolStripMenuItem->Text = L"&Revert";
				this->revert_toolStripMenuItem->Click += gcnew System::EventHandler(this, &resource_editor_base::revertToolStripMenuItem_Click);
				// 
				// applyToolStripMenuItem
				// 
				this->applyToolStripMenuItem->Name = L"applyToolStripMenuItem";
				this->applyToolStripMenuItem->Size = System::Drawing::Size(152, 22);
				this->applyToolStripMenuItem->Text = L"&Apply";
				this->applyToolStripMenuItem->Click += gcnew System::EventHandler(this, &resource_editor_base::applyToolStripMenuItem_Click);
				// 
				// resource_editor_base
				// 
				this->Name = L"resource_editor_base";
				this->m_revertContextMenuStrip->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		private: System::ComponentModel::IContainer^  components;

		}; // class resource_editor_base
	} // namespace editor
} // namespace xray

#endif // #ifndef RESOURCE_EDITOR_BASE_H_INCLUDED