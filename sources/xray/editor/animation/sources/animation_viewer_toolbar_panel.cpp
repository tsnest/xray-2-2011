////////////////////////////////////////////////////////////////////////////
//	Created		: 07.09.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "animation_viewer_toolbar_panel.h"
#include "viewer_editor.h"
#include "images/images24x24.h"

using xray::animation_editor::animation_viewer_toolbar_panel;
using xray::animation_editor::viewer_drag_drop_operation;
using xray::editor::controls::drag_cursor_collection;

void animation_viewer_toolbar_panel::in_constructor(void)
{
	ImageList^ images_list = xray::editor_base::image_loader::load_images("images24x24", "animation_editor.resources", 24, safe_cast<int>(images24x24_count), this->GetType()->Assembly);

	pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
	pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
	pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(pictureBox1))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(pictureBox2))->BeginInit();
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(pictureBox3))->BeginInit();
	SuspendLayout();

	// pictureBox1
	pictureBox1->Image = safe_cast<System::Drawing::Image^>(images_list->Images[animation_node]);
	pictureBox1->Location = System::Drawing::Point(5, 5);
	pictureBox1->Name = L"pictureBox1";
	pictureBox1->Size = System::Drawing::Size(32, 32);
	pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
	pictureBox1->TabIndex = 0;
	pictureBox1->TabStop = false;
	pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &animation_viewer_toolbar_panel::animation_node_mouse_down);

	// pictureBox2
	pictureBox2->Image = safe_cast<System::Drawing::Image^>(images_list->Images[operation_node]);
	pictureBox2->Location = System::Drawing::Point(42, 5);
	pictureBox2->Name = L"pictureBox2";
	pictureBox2->Size = System::Drawing::Size(32, 32);
	pictureBox2->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
	pictureBox2->TabIndex = 1;
	pictureBox2->TabStop = false;
	pictureBox2->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &animation_viewer_toolbar_panel::operator_node_mouse_down);

	// pictureBox3
	pictureBox3->Image = safe_cast<System::Drawing::Image^>(images_list->Images[target_node]);
	pictureBox3->Location = System::Drawing::Point(79, 5);
	pictureBox3->Name = L"pictureBox3";
	pictureBox3->Size = System::Drawing::Size(32, 32);
	pictureBox3->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
	pictureBox3->TabIndex = 2;
	pictureBox3->TabStop = false;
	pictureBox3->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &animation_viewer_toolbar_panel::root_node_mouse_down);

	Controls->Add(pictureBox3);
	Controls->Add(pictureBox2);
	Controls->Add(pictureBox1);
	safe_cast<System::ComponentModel::ISupportInitialize^ >(pictureBox1)->EndInit();
	safe_cast<System::ComponentModel::ISupportInitialize^ >(pictureBox2)->EndInit();
	safe_cast<System::ComponentModel::ISupportInitialize^ >(pictureBox3)->EndInit();
	m_cursors = gcnew xray::editor::controls::drag_cursor_collection();
	HideOnClose = true;
	ResumeLayout(false);
}

System::Void animation_viewer_toolbar_panel::animation_node_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
	{
		m_cursors->make_cursors(pictureBox1->Image);
		DoDragDrop(viewer_drag_drop_operation::animation_node, System::Windows::Forms::DragDropEffects::Copy);
	}
}

System::Void animation_viewer_toolbar_panel::operator_node_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
	{
		m_cursors->make_cursors(pictureBox2->Image);
		DoDragDrop(viewer_drag_drop_operation::operator_node, System::Windows::Forms::DragDropEffects::Copy);
	}
}

System::Void animation_viewer_toolbar_panel::root_node_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
	{
		m_cursors->make_cursors(pictureBox3->Image);
		DoDragDrop(viewer_drag_drop_operation::root_node, System::Windows::Forms::DragDropEffects::Copy);
	}
}

void animation_viewer_toolbar_panel::OnGiveFeedback(GiveFeedbackEventArgs^ e)
{
	e->UseDefaultCursors = false;
	m_cursors->set_cursor_for(e->Effect);
}
