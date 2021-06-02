////////////////////////////////////////////////////////////////////////////
//	Created		: 08.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_FILES_VIEW_PANEL_H_INCLUDED
#define SOUND_FILES_VIEW_PANEL_H_INCLUDED

#include "sound_file_selection_event_args.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

using namespace xray::editor::controls;

namespace xray
{
	namespace editor
	{
		ref struct	raw_file_property_struct;
		ref class	raw_file_selection_event_args;
		ref class	graph_view;

		/// <summary>
		/// Summary for raw_files_editor
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class sound_files_view_panel :  public WeifenLuo::WinFormsUI::Docking::DockContent
		{

		#pragma region | Initialize |

		public:
			sound_files_view_panel()
			{
				InitializeComponent();
				//
				//TODO: Add the constructor code here
				//
				in_constructor();
				HideOnClose = true;
			}

		protected:
			/// <summary>
			/// Clean up any resources being used.
			/// </summary>
			~sound_files_view_panel()
			{
				if (components)
				{
					delete components;
				}
			}
		private:
			void in_constructor();

			/// <summary>
			/// Required designer variable.
			/// </summary>

		#pragma region Windows Form Designer generated code
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->components = (gcnew System::ComponentModel::Container());
				System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(sound_files_view_panel::typeid));
				this->m_tree_view_image_list = (gcnew System::Windows::Forms::ImageList(this->components));
				this->m_sound_files_tree_view = (gcnew xray::editor::controls::tree_view());
				this->m_tree_view_panel = (gcnew System::Windows::Forms::Panel());
				this->splitter1 = (gcnew System::Windows::Forms::Splitter());
				this->m_rms_graph_panel = (gcnew System::Windows::Forms::Panel());
				this->m_loading_label = (gcnew System::Windows::Forms::Label());
				this->m_rms_label = (gcnew System::Windows::Forms::Label());
				this->m_raw_properties_panel = (gcnew System::Windows::Forms::Panel());
				this->m_property_grid = (gcnew xray::editor::controls::property_grid());
				this->raw_options_label = (gcnew System::Windows::Forms::Label());
				this->m_selected_file_label = (gcnew System::Windows::Forms::Label());
				this->m_tree_view_panel->SuspendLayout();
				this->m_rms_graph_panel->SuspendLayout();
				this->m_raw_properties_panel->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_tree_view_image_list
				// 
				this->m_tree_view_image_list->ImageStream = (cli::safe_cast<System::Windows::Forms::ImageListStreamer^  >(resources->GetObject(L"m_tree_view_image_list.ImageStream")));
				this->m_tree_view_image_list->TransparentColor = System::Drawing::Color::Transparent;
				this->m_tree_view_image_list->Images->SetKeyName(0, L"folder_closed.bmp");
				this->m_tree_view_image_list->Images->SetKeyName(1, L"folder_opened.bmp");
				this->m_tree_view_image_list->Images->SetKeyName(2, L"sound_file.bmp");
				// 
				// m_sound_files_tree_view
				// 
				this->m_sound_files_tree_view->auto_expand_on_filter = false;
				this->m_sound_files_tree_view->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_sound_files_tree_view->filter_visible = false;
				this->m_sound_files_tree_view->HideSelection = false;
				this->m_sound_files_tree_view->ImageIndex = 0;
				this->m_sound_files_tree_view->ImageList = this->m_tree_view_image_list;
				this->m_sound_files_tree_view->Indent = 22;
				this->m_sound_files_tree_view->is_multiselect = false;
				this->m_sound_files_tree_view->is_selectable_groups = true;
				this->m_sound_files_tree_view->ItemHeight = 20;
				this->m_sound_files_tree_view->Location = System::Drawing::Point(0, 0);
				this->m_sound_files_tree_view->Name = L"m_sound_files_tree_view";
				this->m_sound_files_tree_view->nodes_context_menu = nullptr;
				this->m_sound_files_tree_view->PathSeparator = L"/";
				this->m_sound_files_tree_view->SelectedImageIndex = 0;
				this->m_sound_files_tree_view->Size = System::Drawing::Size(289, 157);
				this->m_sound_files_tree_view->source = nullptr;
				this->m_sound_files_tree_view->TabIndex = 0;
				this->m_sound_files_tree_view->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &sound_files_view_panel::raw_files_tree_view_after_select);
				this->m_sound_files_tree_view->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &sound_files_view_panel::raw_files_tree_view_mouse_move);
				this->m_sound_files_tree_view->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &sound_files_view_panel::raw_files_tree_view_mouse_down);
				// 
				// m_tree_view_panel
				// 
				this->m_tree_view_panel->Controls->Add(this->m_sound_files_tree_view);
				this->m_tree_view_panel->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_tree_view_panel->Location = System::Drawing::Point(0, 0);
				this->m_tree_view_panel->Name = L"m_tree_view_panel";
				this->m_tree_view_panel->Size = System::Drawing::Size(289, 157);
				this->m_tree_view_panel->TabIndex = 0;
				// 
				// splitter1
				// 
				this->splitter1->Location = System::Drawing::Point(0, 0);
				this->splitter1->Name = L"splitter1";
				this->splitter1->Size = System::Drawing::Size(3, 3);
				this->splitter1->TabIndex = 0;
				this->splitter1->TabStop = false;
				// 
				// m_rms_graph_panel
				// 
				this->m_rms_graph_panel->BackColor = System::Drawing::SystemColors::Control;
				this->m_rms_graph_panel->Controls->Add(this->m_loading_label);
				this->m_rms_graph_panel->Controls->Add(this->m_rms_label);
				this->m_rms_graph_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->m_rms_graph_panel->Location = System::Drawing::Point(0, 181);
				this->m_rms_graph_panel->Name = L"m_rms_graph_panel";
				this->m_rms_graph_panel->Size = System::Drawing::Size(289, 143);
				this->m_rms_graph_panel->TabIndex = 2;
				// 
				// m_loading_label
				// 
				this->m_loading_label->Anchor = System::Windows::Forms::AnchorStyles::Top;
				this->m_loading_label->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
				this->m_loading_label->Font = (gcnew System::Drawing::Font(L"Nina", 30, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->m_loading_label->ForeColor = System::Drawing::SystemColors::AppWorkspace;
				this->m_loading_label->Location = System::Drawing::Point(45, 49);
				this->m_loading_label->Name = L"m_loading_label";
				this->m_loading_label->Size = System::Drawing::Size(191, 48);
				this->m_loading_label->TabIndex = 1;
				this->m_loading_label->Text = L"LOADING";
				this->m_loading_label->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				this->m_loading_label->Visible = false;
				// 
				// m_rms_label
				// 
				this->m_rms_label->BackColor = System::Drawing::SystemColors::Control;
				this->m_rms_label->Dock = System::Windows::Forms::DockStyle::Top;
				this->m_rms_label->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->m_rms_label->Location = System::Drawing::Point(0, 0);
				this->m_rms_label->Name = L"m_rms_label";
				this->m_rms_label->Size = System::Drawing::Size(289, 22);
				this->m_rms_label->TabIndex = 0;
				this->m_rms_label->Text = L"  • RMS graph";
				// 
				// m_raw_properties_panel
				// 
				this->m_raw_properties_panel->Controls->Add(this->m_property_grid);
				this->m_raw_properties_panel->Controls->Add(this->raw_options_label);
				this->m_raw_properties_panel->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->m_raw_properties_panel->Location = System::Drawing::Point(0, 324);
				this->m_raw_properties_panel->Name = L"m_raw_properties_panel";
				this->m_raw_properties_panel->Size = System::Drawing::Size(289, 208);
				this->m_raw_properties_panel->TabIndex = 3;
				// 
				// m_property_grid
				// 
				this->m_property_grid->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_property_grid->Location = System::Drawing::Point(0, 22);
				this->m_property_grid->PropertySort = System::Windows::Forms::PropertySort::NoSort;
				this->m_property_grid->Name = L"m_property_grid";
				this->m_property_grid->Size = System::Drawing::Size(289, 186);
				this->m_property_grid->TabIndex = 1;
				// 
				// raw_options_label
				// 
				this->raw_options_label->Dock = System::Windows::Forms::DockStyle::Top;
				this->raw_options_label->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->raw_options_label->Location = System::Drawing::Point(0, 0);
				this->raw_options_label->Name = L"raw_options_label";
				this->raw_options_label->Size = System::Drawing::Size(289, 22);
				this->raw_options_label->TabIndex = 0;
				this->raw_options_label->Text = L"  • Raw file options";
				// 
				// m_selected_file_label
				// 
				this->m_selected_file_label->BackColor = System::Drawing::SystemColors::ControlLightLight;
				this->m_selected_file_label->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				this->m_selected_file_label->Dock = System::Windows::Forms::DockStyle::Bottom;
				this->m_selected_file_label->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, 
					System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(204)));
				this->m_selected_file_label->Location = System::Drawing::Point(0, 157);
				this->m_selected_file_label->Name = L"m_selected_file_label";
				this->m_selected_file_label->Padding = System::Windows::Forms::Padding(3, 0, 0, 0);
				this->m_selected_file_label->Size = System::Drawing::Size(289, 24);
				this->m_selected_file_label->TabIndex = 1;
				this->m_selected_file_label->Text = L"no selected file";
				this->m_selected_file_label->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
				// 
				// sound_files_view_panel
				// 
				this->AutoScroll = true;
				this->ClientSize = System::Drawing::Size(289, 532);
				this->Controls->Add(this->m_tree_view_panel);
				this->Controls->Add(this->m_selected_file_label);
				this->Controls->Add(this->m_rms_graph_panel);
				this->Controls->Add(this->m_raw_properties_panel);
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->Name = L"sound_files_view_panel";
				this->Text = L"Sound Files";
				this->m_tree_view_panel->ResumeLayout(false);
				this->m_rms_graph_panel->ResumeLayout(false);
				this->m_raw_properties_panel->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion

		#pragma endregion

		#pragma region |   Events   |

		public:
			event EventHandler<sound_file_selection_event_args^>^ raw_file_selected;
			event EventHandler<sound_file_selection_event_args^>^ raw_file_deselected;
			
		#pragma endregion

		#pragma region |   Fields   |

		private:
			scalable_scroll_bar^					m_rms_scroll;
			graph_view^								m_rms_graph;
			Splitter^								m_splitter;
			TreeNode^								m_drag_node;
			System::Drawing::Rectangle				m_drag_box_from_mouse_down;

			System::Windows::Forms::ImageList^		m_tree_view_image_list;
			System::Windows::Forms::Splitter^		splitter1;
			System::Windows::Forms::Panel^			m_rms_graph_panel;
			System::Windows::Forms::Panel^			m_raw_properties_panel;
			System::Windows::Forms::Label^			m_rms_label;
			System::Windows::Forms::Label^			m_selected_file_label;
			System::Windows::Forms::Label^			raw_options_label;
			System::Windows::Forms::Label^			m_loading_label;
			xray::editor::controls::tree_view^		m_sound_files_tree_view;
			System::Windows::Forms::Panel^			m_tree_view_panel;
			xray::editor::controls::property_grid^	m_property_grid;


			System::ComponentModel::IContainer^		components;

		#pragma endregion

		#pragma region | Properties |

		public:
			property raw_file_property_struct^	selected_struct
			{
				raw_file_property_struct^	get	();
				void						set	(raw_file_property_struct^ value);
			}

		#pragma endregion

		#pragma region |   Methods  |

		private:
			void	raw_files_tree_view_after_select		(System::Object^  sender, System::Windows::Forms::TreeViewEventArgs^  e);
			void	raw_files_tree_view_mouse_down			(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
			void	raw_files_tree_view_mouse_move			(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
			void	load_complete							(sound_file_struct^ loader);
			void	raw_options_load_complete				(raw_file_property_struct^ loaded_struct);
			void	rms_scroll_changed						(System::Object^  sender, controls::scroller_event_args^  e);
			void	rms_scroll_draw_background				(System::Object^  sender, PaintEventArgs^  e);

		public: 
			void show_properties(Object^ object);
		
		#pragma endregion

		};
	}// namespace editor
}// namespace xray

#endif // #ifndef SOUND_FILES_VIEW_PANEL_H_INCLUDED