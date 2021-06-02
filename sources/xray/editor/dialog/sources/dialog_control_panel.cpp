////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_control_panel.h"
#include "dialog_editor.h"
#include "images/images70x53.h"

using xray::dialog_editor::dialog_control_panel;
using xray::dialog_editor::drag_drop_create_operation;
using xray::editor::controls::drag_cursor_collection;

void dialog_control_panel::in_constructor(void)
{
	ImageList^ images_list = xray::editor_base::image_loader::load_images("images70x53", "dialog_editor.resources", 70, safe_cast<int>(images70x53_count), this->GetType()->Assembly);

	pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
	pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(pictureBox1))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(pictureBox2))->BeginInit();
	SuspendLayout();

	// pictureBox1
	pictureBox1->Image = safe_cast<System::Drawing::Image^>(images_list->Images[single_node]);
	pictureBox1->Location = System::Drawing::Point(5, 5);
	pictureBox1->Name = L"pictureBox1";
	pictureBox1->Size = System::Drawing::Size(48, 40);
	pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
	pictureBox1->TabIndex = 0;
	pictureBox1->TabStop = false;
	pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &dialog_control_panel::single_node_mouse_down);

	// pictureBox2
	pictureBox2->Image = safe_cast<System::Drawing::Image^>(images_list->Images[two_nodes]);
	pictureBox2->Location = System::Drawing::Point(59, 5);
	pictureBox2->Name = L"pictureBox2";
	pictureBox2->Size = System::Drawing::Size(48, 40);
	pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
	pictureBox2->TabIndex = 1;
	pictureBox2->TabStop = false;
	pictureBox2->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &dialog_control_panel::quest_answ_mouse_down);

	Controls->Add(pictureBox2);
	Controls->Add(pictureBox1);
	safe_cast<System::ComponentModel::ISupportInitialize^ >(pictureBox1)->EndInit();
	safe_cast<System::ComponentModel::ISupportInitialize^ >(pictureBox2)->EndInit();
	m_cursors = gcnew xray::editor::controls::drag_cursor_collection();
	HideOnClose = true;
	ResumeLayout(false);
}

System::Void dialog_control_panel::single_node_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
	{
		m_cursors->make_cursors(pictureBox1->Image);
		DoDragDrop(drag_drop_create_operation::single_node, System::Windows::Forms::DragDropEffects::Copy);
	}
}

System::Void dialog_control_panel::quest_answ_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
	{
		m_cursors->make_cursors(pictureBox2->Image);
		DoDragDrop(drag_drop_create_operation::question_answer, System::Windows::Forms::DragDropEffects::Copy);
	}
}
void dialog_control_panel::OnGiveFeedback(GiveFeedbackEventArgs^ e)
{
	e->UseDefaultCursors = false;
	m_cursors->set_cursor_for(e->Effect);
}
