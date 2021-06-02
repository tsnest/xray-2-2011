////////////////////////////////////////////////////////////////////////////
//	Created		: 20.01.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DOCUMENT_BASE_H_INCLUDED
#define DOCUMENT_BASE_H_INCLUDED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray
{
	namespace editor
	{
		namespace controls
		{

			ref class document_editor_base;

			public ref class document_base : public WeifenLuo::WinFormsUI::Docking::DockContent
			{
			public:
				document_base(document_editor_base^ ed):m_editor(ed)
				{
					InitializeComponent();

					m_is_saved = true;
				}

				virtual	void		save					()				{};
				virtual	void		make_no_saved			()				{is_saved		= false;}
				virtual	void		load					()				{};
				virtual	void		load_partialy			(String^ part_path)	{};
				virtual	void		undo					()				{};
				virtual	void		redo					()				{};
				virtual	void		copy					(bool del)		{};
				virtual	void		del						()				{};
				virtual	void		paste					()				{};
				virtual	void		select_all				()				{};
				virtual	String^		get_document_name		(String^ full_name)				{return full_name;}
				virtual	Boolean		check_saved				();

				property System::String^ source_path;
				property System::String^ extension;

			protected:
				~document_base()
				{
					if(components)
						delete components;
				}

			private:
						document_base(){}
						
			protected:
						virtual System::Void	on_form_closing		(System::Object^ sender, System::Windows::Forms::FormClosingEventArgs^ e);
						virtual System::Void	on_form_activated	(System::Object^ sender, System::EventArgs^ e);
			private:

		#pragma region Windows Form Designer generated code
				void InitializeComponent(void)
				{
					this->SuspendLayout();
					// 
					// document_base
					// 
					this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
					this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
					this->ClientSize = System::Drawing::Size(292, 273);
					this->DockAreas = WeifenLuo::WinFormsUI::Docking::DockAreas::Document;
					this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
						static_cast<System::Byte>(204)));
					this->Name = L"document_base";
					this->Enter += gcnew System::EventHandler(this, &document_base::on_form_activated);
					this->VisibleChanged += gcnew System::EventHandler(this, &document_base::on_form_activated);
					this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &document_base::on_form_closing);
					this->ResumeLayout(false);

				}
		#pragma endregion

			public:
				property bool				is_saved
				{
					bool		get()				{return m_is_saved;}
					void		set(bool value)
					{
						m_is_saved = value;
						if(!m_is_saved && !Text->EndsWith("*"))
							Text += "*";
						else if(m_is_saved  && Text->EndsWith("*"))
							Text = Text->Substring(0, Text->Length-1);
					}
				}

			protected:
				document_editor_base^				m_editor;

			private:
				bool								m_is_saved;
				System::ComponentModel::Container^	components;

			public:
				bool								m_need_suppress_loading;

			}; // class document_base
		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef DOCUMENT_BASE_H_INCLUDED