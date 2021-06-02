////////////////////////////////////////////////////////////////////////////
//	Created		: 11.02.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEXTURE_DOCUMENT_H_INCLUDED
#define TEXTURE_DOCUMENT_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;

using xray::editor::wpf_controls::property_grid::value_changed_event_args;
using System::Windows::RoutedEventArgs;

typedef xray::editor::wpf_controls::property_container wpf_property_container;
typedef xray::resources::unmanaged_resource_ptr unmanaged_resource_ptr;

namespace xray
{
	namespace editor
	{
		ref class	texture_editor;
		struct		texture_tga_argb_buffers;
		ref class	texture_editor_resource;
			
//		typedef xray::editor::wpf_controls::property_editors::property prop;

		interface class i_resource_document
		{
			property List<System::String^>^ loading_items;
		};

		/// <summary>
		/// Summary for texture_document
		/// </summary>
		public ref class texture_document : xray::editor::controls::document_base, i_resource_document
		{

		#pragma region |   Events   |

		public:
			event System::EventHandler^		resource_loaded;

		#pragma endregion

		#pragma region | Initialize |

		public:
			texture_document(controls::document_editor_base^ editor, texture_editor^ res_editor)
				:document_base		( editor ),
				m_texture_editor	( res_editor )
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
			~texture_document()
			{
				if (components)
				{
					delete components;
				}
			}

		private:
			void	in_constructor();
		private: System::Windows::Forms::PictureBox^	m_picture_box;
		private: System::Windows::Forms::Panel^			m_content_panel;

		private: System::Windows::Forms::Panel^			m_property_panel;
		private: System::Windows::Forms::CheckBox^		m_r_checkbox;
		private: System::Windows::Forms::CheckBox^		m_a_checkbox;

		private: System::Windows::Forms::CheckBox^		m_b_checkbox;
		private: System::Windows::Forms::CheckBox^		m_g_checkbox;
		private: System::Windows::Forms::CheckBox^  	m_ab_checkbox;


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
				this->m_picture_box = (gcnew System::Windows::Forms::PictureBox());
				this->m_content_panel = (gcnew System::Windows::Forms::Panel());
				this->m_property_panel = (gcnew System::Windows::Forms::Panel());
				this->m_ab_checkbox = (gcnew System::Windows::Forms::CheckBox());
				this->m_a_checkbox = (gcnew System::Windows::Forms::CheckBox());
				this->m_b_checkbox = (gcnew System::Windows::Forms::CheckBox());
				this->m_g_checkbox = (gcnew System::Windows::Forms::CheckBox());
				this->m_r_checkbox = (gcnew System::Windows::Forms::CheckBox());
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_picture_box))->BeginInit();
				this->m_content_panel->SuspendLayout();
				this->m_property_panel->SuspendLayout();
				this->SuspendLayout();
				// 
				// m_picture_box
				// 
				this->m_picture_box->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
				this->m_picture_box->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_picture_box->Location = System::Drawing::Point(0, 0);
				this->m_picture_box->Name = L"m_picture_box";
				this->m_picture_box->Size = System::Drawing::Size(498, 333);
				this->m_picture_box->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
				this->m_picture_box->TabIndex = 0;
				this->m_picture_box->TabStop = false;
				// 
				// m_content_panel
				// 
				this->m_content_panel->Controls->Add(this->m_picture_box);
				this->m_content_panel->Dock = System::Windows::Forms::DockStyle::Fill;
				this->m_content_panel->Location = System::Drawing::Point(0, 30);
				this->m_content_panel->Name = L"m_content_panel";
				this->m_content_panel->Size = System::Drawing::Size(498, 333);
				this->m_content_panel->TabIndex = 1;
				// 
				// m_property_panel
				// 
				this->m_property_panel->Controls->Add(this->m_ab_checkbox);
				this->m_property_panel->Controls->Add(this->m_a_checkbox);
				this->m_property_panel->Controls->Add(this->m_b_checkbox);
				this->m_property_panel->Controls->Add(this->m_g_checkbox);
				this->m_property_panel->Controls->Add(this->m_r_checkbox);
				this->m_property_panel->Dock = System::Windows::Forms::DockStyle::Top;
				this->m_property_panel->Location = System::Drawing::Point(0, 0);
				this->m_property_panel->Name = L"m_property_panel";
				this->m_property_panel->Size = System::Drawing::Size(498, 30);
				this->m_property_panel->TabIndex = 2;
				// 
				// m_ab_checkbox
				// 
				this->m_ab_checkbox->Appearance = System::Windows::Forms::Appearance::Button;
				this->m_ab_checkbox->Location = System::Drawing::Point(144, 3);
				this->m_ab_checkbox->Name = L"m_ab_checkbox";
				this->m_ab_checkbox->Size = System::Drawing::Size(30, 24);
				this->m_ab_checkbox->TabIndex = 1;
				this->m_ab_checkbox->Text = L"Ab";
				this->m_ab_checkbox->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				this->m_ab_checkbox->UseVisualStyleBackColor = true;
				this->m_ab_checkbox->CheckStateChanged += gcnew System::EventHandler(this, &texture_document::rgba_check_boxes_checked_changed);
				// 
				// m_a_checkbox
				// 
				this->m_a_checkbox->Appearance = System::Windows::Forms::Appearance::Button;
				this->m_a_checkbox->Location = System::Drawing::Point(111, 3);
				this->m_a_checkbox->Name = L"m_a_checkbox";
				this->m_a_checkbox->Size = System::Drawing::Size(27, 24);
				this->m_a_checkbox->TabIndex = 1;
				this->m_a_checkbox->Text = L"A";
				this->m_a_checkbox->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				this->m_a_checkbox->UseVisualStyleBackColor = true;
				this->m_a_checkbox->CheckStateChanged += gcnew System::EventHandler(this, &texture_document::rgba_check_boxes_checked_changed);
				// 
				// m_b_checkbox
				// 
				this->m_b_checkbox->Appearance = System::Windows::Forms::Appearance::Button;
				this->m_b_checkbox->Checked = true;
				this->m_b_checkbox->CheckState = System::Windows::Forms::CheckState::Checked;
				this->m_b_checkbox->Location = System::Drawing::Point(78, 3);
				this->m_b_checkbox->Name = L"m_b_checkbox";
				this->m_b_checkbox->Size = System::Drawing::Size(27, 24);
				this->m_b_checkbox->TabIndex = 1;
				this->m_b_checkbox->Text = L"B";
				this->m_b_checkbox->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				this->m_b_checkbox->UseVisualStyleBackColor = true;
				this->m_b_checkbox->CheckStateChanged += gcnew System::EventHandler(this, &texture_document::rgba_check_boxes_checked_changed);
				// 
				// m_g_checkbox
				// 
				this->m_g_checkbox->Appearance = System::Windows::Forms::Appearance::Button;
				this->m_g_checkbox->Checked = true;
				this->m_g_checkbox->CheckState = System::Windows::Forms::CheckState::Checked;
				this->m_g_checkbox->Location = System::Drawing::Point(45, 3);
				this->m_g_checkbox->Name = L"m_g_checkbox";
				this->m_g_checkbox->Size = System::Drawing::Size(27, 24);
				this->m_g_checkbox->TabIndex = 1;
				this->m_g_checkbox->Text = L"G";
				this->m_g_checkbox->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				this->m_g_checkbox->UseVisualStyleBackColor = true;
				this->m_g_checkbox->CheckStateChanged += gcnew System::EventHandler(this, &texture_document::rgba_check_boxes_checked_changed);
				// 
				// m_r_checkbox
				// 
				this->m_r_checkbox->Appearance = System::Windows::Forms::Appearance::Button;
				this->m_r_checkbox->Checked = true;
				this->m_r_checkbox->CheckState = System::Windows::Forms::CheckState::Checked;
				this->m_r_checkbox->Location = System::Drawing::Point(12, 3);
				this->m_r_checkbox->Name = L"m_r_checkbox";
				this->m_r_checkbox->Size = System::Drawing::Size(27, 24);
				this->m_r_checkbox->TabIndex = 1;
				this->m_r_checkbox->Text = L"R";
				this->m_r_checkbox->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				this->m_r_checkbox->UseVisualStyleBackColor = true;
				this->m_r_checkbox->CheckStateChanged += gcnew System::EventHandler(this, &texture_document::rgba_check_boxes_checked_changed);
				// 
				// texture_document
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(498, 363);
				this->Controls->Add(this->m_content_panel);
				this->Controls->Add(this->m_property_panel);
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(204)));
				this->Name = L"texture_document";
				this->Text = L"texture_document";
				(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->m_picture_box))->EndInit();
				this->m_content_panel->ResumeLayout(false);
				this->m_property_panel->ResumeLayout(false);
				this->ResumeLayout(false);

			}
		#pragma endregion

		#pragma endregion

		#pragma region |   Fields   |

		private:
			float							m_scale;
			bool							m_document_exists;

			texture_editor^					m_texture_editor;

			//preview fields
			Bitmap^							m_last_image;
			BitmapData^						m_last_image_data;
			texture_tga_argb_buffers*		m_last_buf_struct;

			Dictionary<int, Bitmap^>^		m_images;
			u32								m_tga_request_id;
			bool							m_is_tga_synk_called;
			u32								m_argb_last_request_id;

			Bitmap^							m_picture_image;
			bool							m_image_has_alpha;

			//convarsion options fields
//			List<System::String^>^					m_requesting_options;
//			List<texture_editor_resource^>^			m_options_to_show;
//			List<System::String^>^					m_loading_options;
			
		#pragma endregion

		#pragma region | Properties |


		public:
					property bool					is_use_by_selector;
			virtual property List<System::String^>^	loading_items;


		#pragma endregion

		#pragma region |  Methods   |


		private:
			void				tga_loaded								( resources::queries_result& data, Object^ resource_path );
			void				image_loaded							( resources::queries_result& data );
			void				rgba_check_boxes_checked_changed		( Object^ sender,System::EventArgs^ e);
			void				view_selected_options					( );

		public:
			virtual void						load					( ) override;
			virtual void						save					( ) override;
			virtual controls::document_base^	create_document			( );

		#pragma endregion
};//class texture_document

}//namespace editor
}//namespace xray
#endif // #ifndef TEXTURE_DOCUMENT_H_INCLUDED