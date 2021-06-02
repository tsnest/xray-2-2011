#pragma once


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {

	class editor_world;

	/// <summary>
	/// Summary for loading_dialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class loading_dialog : public System::Windows::Forms::Form
	{
	public:
		loading_dialog( editor_world& world ):
		 m_world (world)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~loading_dialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  cancel_btn;
	protected: 
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Label^  label_elements_loaded;
	private: System::ComponentModel::IContainer^  components;

	private:
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
			this->cancel_btn = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->label_elements_loaded = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// cancel_btn
			// 
			this->cancel_btn->Location = System::Drawing::Point(337, 57);
			this->cancel_btn->Name = L"cancel_btn";
			this->cancel_btn->Size = System::Drawing::Size(75, 23);
			this->cancel_btn->TabIndex = 0;
			this->cancel_btn->Text = L"Cancel";
			this->cancel_btn->UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(57, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Loading ...";
			this->label1->Visible = false;
			// 
			// timer1
			// 
			this->timer1->Enabled = true;
			this->timer1->Tick += gcnew System::EventHandler(this, &loading_dialog::timer1_Tick);
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(15, 30);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(397, 16);
			this->progressBar1->TabIndex = 2;
			// 
			// label_elements_loaded
			// 
			this->label_elements_loaded->AutoSize = true;
			this->label_elements_loaded->Location = System::Drawing::Point(15, 62);
			this->label_elements_loaded->Name = L"label_elements_loaded";
			this->label_elements_loaded->Size = System::Drawing::Size(0, 13);
			this->label_elements_loaded->TabIndex = 3;
			// 
			// loading_dialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(424, 92);
			this->ControlBox = false;
			this->Controls->Add(this->label_elements_loaded);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->cancel_btn);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"loading_dialog";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Loading ...";
			this->TopMost = true;
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: 
		editor_world&		m_world;

	private: 
		System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e);
	};

} // namespace editor
} // namespace xray