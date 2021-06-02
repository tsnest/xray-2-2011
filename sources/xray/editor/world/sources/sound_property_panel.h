////////////////////////////////////////////////////////////////////////////
//	Created		: 10.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_PROPERTY_PANEL_H_INCLUDED
#define SOUND_PROPERTY_PANEL_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;
using namespace Flobbster::Windows::Forms;

using namespace xray::editor::controls;

namespace xray
{
	namespace editor
	{

		ref struct raw_file_property_struct;

		/// <summary>
		/// Summary for raw_file_property
		///
		/// WARNING: If you change the name of this class, you will need to change the
		///          'Resource File Name' property for the managed resource compiler tool
		///          associated with all .resx files this class depends on.  Otherwise,
		///          the designers will not be able to interact properly with localized
		///          resources associated with this form.
		/// </summary>
		public ref class sound_property_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
		{

		#pragma region | Initialize |

		public:
			sound_property_panel(void)
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
			~sound_property_panel()
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
			System::ComponentModel::Container ^components;

			#pragma region Windows Form Designer generated code
			
			/// <summary>
			/// Required method for Designer support - do not modify
			/// the contents of this method with the code editor.
			/// </summary>
			void InitializeComponent(void)
			{
				this->SuspendLayout();
				// 
				// raw_file_property
				// 
				this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				this->ClientSize = System::Drawing::Size(284, 264);
				this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
					static_cast<System::Byte>(0)));
				this->MaximizeBox = false;
				this->MinimizeBox = false;
				this->Name = L"raw_file_property";
				this->Text = L"Sound Properties";
				this->ResumeLayout(false);

			}

			#pragma endregion

		#pragma endregion

		#pragma region |   Fileds   |

		private:
			property_grid^	m_property_grid;

		
		#pragma endregion

		#pragma region | Properties |

		public:
			property raw_file_property_struct^	selected_struct
			{
				raw_file_property_struct^	get	();
				void						set	(raw_file_property_struct^ value);
			}

		#pragma endregion

		#pragma region |  Methods   |

		public: 
			void show_properties(Object^ object);

		};

		#pragma endregion

	}//namespace editor
}//namespace xray
#endif // #ifndef RAW_FILE_PROPERTY_PANEL_H_INCLUDED