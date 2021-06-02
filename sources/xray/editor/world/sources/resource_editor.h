////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_EDITOR_H_INCLUDED
#define RESOURCE_EDITOR_H_INCLUDED

using namespace System::IO;
using namespace System::Windows;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace System::Collections::Generic;

using WeifenLuo::WinFormsUI::Docking::IDockContent;
using xray::editor::wpf_controls::property_grid::value_changed_event_args;
typedef xray::editor::wpf_controls::property_container wpf_property_container;
typedef xray::editor::wpf_controls::property_descriptor property_descriptor;

#include "resource_editor_resource.h"

namespace xray {

namespace editor_base { class property_holder; }

namespace editor {

		class				editor_world;
		interface class		tree_view_source;
		
		typedef				resource_editor_resource			resource;
		typedef				controls::document_base				document_base;

		public ref class resource_editor abstract : public System::Windows::Forms::Form, 
													public editor_base::tool_window	
		{
		#pragma region | InnerTypes |

		public:
			enum class entity
			{
				resource_name,
				resource_object
			};


		#pragma endregion

		#pragma region | Initialize |

		public:

			resource_editor( editor_world& world, bool is_run_as_selector )
				:m_editor_world	( world ),
				m_is_selector	( is_run_as_selector ),
				m_is_apply_resource_on_property_value_changed ( false )
			{
				InitializeComponent	();
				in_constructor		();
			}


		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~resource_editor()
			{
				if (components)
				{
					delete components;
				}
				in_destructor	( );
			}

		private:
			void 		in_constructor			( );
			void 		in_destructor			( );


		private:	System::Windows::Forms::Panel^						m_controls_panel;
		private:	System::Windows::Forms::Button^						m_cancel_button;
		protected:	System::Windows::Forms::Button^						m_ok_button;
		private:	System::Windows::Forms::ToolStripMenuItem^			revertToolStripMenuItem;
		protected:	System::Windows::Forms::ToolStripMenuItem^			revert_toolStripMenuItem;
		protected:	System::Windows::Forms::ToolStripMenuItem^			applyToolStripMenuItem;
		protected:	System::Windows::Forms::ToolStripMenuItem^			expandAllToolStripMenuItem;
		protected:	System::Windows::Forms::ToolStripMenuItem^			collapseToolStripMenuItem;
			/// <summary>
			/// Required designer variable.
			/// </summary>
			System::ComponentModel::Container ^components;

		#pragma region Windows Form Designer generated code

			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
		protected:
			void InitializeComponent(void)
			{
				System::Windows::Forms::GroupBox^  h_line;
				this->m_controls_panel = (gcnew System::Windows::Forms::Panel());
				this->m_cancel_button = (gcnew System::Windows::Forms::Button());
				this->m_ok_button = (gcnew System::Windows::Forms::Button());
				this->revert_toolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->applyToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->expandAllToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				this->collapseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
				h_line = (gcnew System::Windows::Forms::GroupBox());
				this->m_controls_panel->SuspendLayout();
				this->SuspendLayout();
				// 
				// h_line
				// 
				h_line->Dock = System::Windows::Forms::DockStyle::Bottom;
				h_line->Location = System::Drawing::Point(0, 482);
				h_line->Name = L"h_line";
				h_line->Size = System::Drawing::Size(653, 3);
				h_line->TabIndex = 1;
				h_line->TabStop = false;
				// 
				// m_controls_panel
				// 
				this->m_controls_panel->Controls->Add(this->m_cancel_button);
				this->m_controls_panel->Controls->Add(this->m_ok_button);
				this->m_controls_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->m_controls_panel->Location = System::Drawing::Point(0, 485);
				this->m_controls_panel->Name = L"m_controls_panel";
				this->m_controls_panel->Size = System::Drawing::Size(653, 54);
				this->m_controls_panel->TabIndex = 0;
				// 
				// m_cancel_button
				// 
				this->m_cancel_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
				this->m_cancel_button->Location = System::Drawing::Point(566, 16);
				this->m_cancel_button->Name = L"m_cancel_button";
				this->m_cancel_button->Size = System::Drawing::Size(75, 23);
				this->m_cancel_button->TabIndex = 0;
				this->m_cancel_button->Text = L"Cancel";
				this->m_cancel_button->UseVisualStyleBackColor = true;
				this->m_cancel_button->Click += gcnew System::EventHandler(this, &resource_editor::m_cancel_button_Click);
				// 
				// m_ok_button
				// 
				this->m_ok_button->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
				this->m_ok_button->Location = System::Drawing::Point(485, 16);
				this->m_ok_button->Name = L"m_ok_button";
				this->m_ok_button->Size = System::Drawing::Size(75, 23);
				this->m_ok_button->TabIndex = 0;
				this->m_ok_button->Text = L"Apply";
				this->m_ok_button->UseVisualStyleBackColor = true;
				this->m_ok_button->Click += gcnew System::EventHandler(this, &resource_editor::m_ok_button_Click);
				// 
				// revert_toolStripMenuItem
				// 
				this->revert_toolStripMenuItem->Name = L"revert_toolStripMenuItem";
				this->revert_toolStripMenuItem->Size = System::Drawing::Size(152, 22);
				this->revert_toolStripMenuItem->Text = L"&Revert";
				// 
				// applyToolStripMenuItem
				// 
				this->applyToolStripMenuItem->Name = L"applyToolStripMenuItem";
				this->applyToolStripMenuItem->Size = System::Drawing::Size(152, 22);
				this->applyToolStripMenuItem->Text = L"&Apply";
				// 
				// expandAllToolStripMenuItem
				// 
				this->expandAllToolStripMenuItem->Name = L"expandAllToolStripMenuItem";
				this->expandAllToolStripMenuItem->Size = System::Drawing::Size(152, 22);
				this->expandAllToolStripMenuItem->Text = L"&Expand All";
				// 
				// collapseToolStripMenuItem
				// 
				this->collapseToolStripMenuItem->Name = L"collapseToolStripMenuItem";
				this->collapseToolStripMenuItem->Size = System::Drawing::Size(152, 22);
				this->collapseToolStripMenuItem->Text = L"&Collapse All";
				// 
				// resource_editor
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(653, 539);
				this->Controls->Add(h_line);
				this->Controls->Add(this->m_controls_panel);
				this->Name = L"resource_editor";
				this->ShowInTaskbar = false;
				this->Text = L"Resource Editor";
				this->Load += gcnew System::EventHandler(this, &resource_editor::resource_editor_Load);
				this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &resource_editor::resource_editor_FormClosing);
				this->m_controls_panel->ResumeLayout(false);
				this->ResumeLayout(false);

			}

		#pragma endregion


		#pragma endregion

		#pragma region |   Fields   |

		protected:
			controls::document_editor_base^			m_multidocument;
			controls::item_properties_panel_base^	m_properties_panel;
			bool									m_is_selector;
			System::String^							m_resources_sources_path;
			bool									m_need_reset_grid_on_revert;
			System::String^							m_track_resource_name;
			array<System::String^>^					m_track_resource_names;
			bool									m_is_resource_list_loaded;
			bool									m_self_value_change;
			bool									m_resources_loaded;
			bool									m_commit_on_loaded;

			ImageList^								m_images;

			resources_dict^							m_opened_resources;
			resources_list^							m_changed_resources;
			resources_list^							m_selected_resources;
			bool									m_need_sort_tree;
			bool									m_need_refresh_properties;
			bool									m_need_view_selected;
			xray::editor_base::tool_window_holder^	m_holder;

		public:
			void			apply_changes			( resources_list^ selected );
			void			revert_changes			( resources_list^ selected );
			void			show_selected_options	( ) {m_need_view_selected=true;}
		#pragma endregion

		#pragma region | Properties |


		protected:
			virtual property controls::tree_view^	resources_tree_view 
			{
				controls::tree_view^						get() abstract;
			}
			virtual property int						resource_image_index
			{
				int										get() abstract;
			}
			virtual property int						modified_resource_image_index
			{
				int										get() abstract;
			}

		public:
			property controls::document_editor_base^			multidocument
			{
				controls::document_editor_base^				get(){return m_multidocument;}
			}

			property controls::item_properties_panel_base^		properties_panel
			{
				controls::item_properties_panel_base^		get(){return m_properties_panel;}
			}

			//property System::String^	name
			//{
			//	System::String^		get( )							{ return m_multidocument->Name; }
			//	void				set( System::String^ value )	{ m_multidocument->Name = value; }
			//}

			virtual property System::String^	selected_name
			{
				System::String^ get( )	abstract;
				void			set( System::String^ value ) abstract;
			}

			property resources_dict^	opened_resources
			{
				resources_dict^ get	( )			{ return m_opened_resources; }
			}

			property resources_list^	changed_resources
			{
				resources_list^ get	( )			{ return m_changed_resources; }
			}

			property resources_list^	selected_resources
			{
				resources_list^ get(){return m_selected_resources;}
			}

			property bool				is_selector
			{
				bool			get	( )			{ return m_is_selector; }
			}

			property entity				selecting_entity;


		#pragma endregion

		#pragma region |   Methods  |

		// TODO!
		// bool global_refresh;
		// virtual void refresh();
			
			virtual void on_resource_changed	( ) {}
			bool m_is_apply_resource_on_property_value_changed;
			
		public:
			editor_world&				m_editor_world;
		private:
			void						resource_editor_Load			( Object^ sender,System::EventArgs^ e );
			IDockContent^				layout_for_panel_callback		( System::String^ panel_name );
			document_base^				on_document_creating			( );
			void						on_resource_list_loaded			( Object^ sender,System::EventArgs^ e );
			void						tree_view_selection_changed		( Object^ sender, TreeViewEventArgs^ e );
			void						tree_view_node_double_click		( Object^ sender, TreeNodeMouseClickEventArgs^ e );
			void						revertToolStripMenuItem_Click	( Object^ sender,System::EventArgs^ e );
			void						applyToolStripMenuItem_Click	( Object^ sender,System::EventArgs^ e );
			void						expandAllToolStripMenuItem_Click( Object^ sender,System::EventArgs^ e );
			void						collapseAllToolStripMenuItem_Click( Object^ sender,System::EventArgs^ e );

		protected:
			void						view_selected_options_impl		( );
			void						on_property_value_changed		( Object^ sender, value_changed_event_args^ e );
			RegistryKey^				editor_registry_key				( );
			void						make_wrapper_modified			( resource_editor_resource^ wrapper );
			void						m_ok_button_Click				( Object^ sender,System::EventArgs^ e );
			void						m_cancel_button_Click			( Object^ sender,System::EventArgs^ e );
			void						resources_loaded				( Object^ sender,System::EventArgs^ e );
			virtual bool				load_panels_layout				( );
			virtual IDockContent^		layout_for_panel				( System::String^ panel_name);
			virtual document_base^		create_new_document				( ) abstract;
			void						apply_properties				( resource_editor_resource^ wrapper );
			void						revert_properties				( resource_editor_resource^ wrapper );
			
			void						close_internal					( Forms::DialogResult result );
			virtual	void				on_menu_opening					( System::Object^ sender, System::ComponentModel::CancelEventArgs^ e );
			bool						resolve_changed_resources		( bool cancel_action );
		public:
			void						raise_property_value_changed	( );
			void						resource_editor_FormClosing		( System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^ e );
			bool						is_resource_loaded				( System::String^ resource_name );
			resource^					get_loaded_resource				( System::String^ resource_name );
			virtual void				tick							( );// tool_window member
			virtual void				clear_resources					( );// tool_window member
			virtual bool				close_query						( ); // tool_window member
			virtual	System::String^		name							( ) { return Name; } // tool_window member
			virtual void				load_settings					( RegistryKey^ /*product_key*/ ) {};
			virtual void				save_settings					( RegistryKey^ /*product_key*/ ) {};

			virtual void				Show							( System::String^ context1, System::String^ context2 );
			virtual System::Windows::Forms::Form^	main_form			( ) {return this; }
		#pragma endregion

		}; // class resource_editor
	}//namespace editor
}//namespace xray

#endif // #ifndef RESOURCE_EDITOR_H_INCLUDED