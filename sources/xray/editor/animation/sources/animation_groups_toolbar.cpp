////////////////////////////////////////////////////////////////////////////
//	Created		: 07.04.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "animation_groups_toolbar.h"
#include "animation_groups_editor.h"
#include "images/images24x24.h"

using xray::animation_editor::animation_groups_toolbar;
using xray::animation_editor::animation_groups_drag_drop_operation;
using xray::editor::controls::drag_cursor_collection;

void animation_groups_toolbar::in_constructor(void)
{
	ImageList^ images_list = xray::editor_base::image_loader::load_images("images24x24", "animation_editor.resources", 24, safe_cast<int>(images24x24_count), this->GetType()->Assembly);

	pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
	(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(pictureBox1))->BeginInit();
	SuspendLayout();

	// pictureBox1
	pictureBox1->Image = safe_cast<System::Drawing::Image^>(images_list->Images[group_node]);
	pictureBox1->Location = System::Drawing::Point(5, 5);
	pictureBox1->Name = L"pictureBox1";
	pictureBox1->Size = System::Drawing::Size(32, 32);
	pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
	pictureBox1->TabIndex = 0;
	pictureBox1->TabStop = false;
	pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &animation_groups_toolbar::animation_group_node_mouse_down);

	Controls->Add(pictureBox1);
	safe_cast<System::ComponentModel::ISupportInitialize^ >(pictureBox1)->EndInit();
	m_cursors = gcnew xray::editor::controls::drag_cursor_collection();
	HideOnClose = true;
	ResumeLayout(false);
}

void animation_groups_toolbar::animation_group_node_mouse_down(System::Object^ , System::Windows::Forms::MouseEventArgs^ e)
{
	if(e->Button==System::Windows::Forms::MouseButtons::Left)
	{
		m_cursors->make_cursors(pictureBox1->Image);
		DoDragDrop(animation_groups_drag_drop_operation::animation_group_node, System::Windows::Forms::DragDropEffects::Copy);
	}
}

void animation_groups_toolbar::OnGiveFeedback(GiveFeedbackEventArgs^ e)
{
	e->UseDefaultCursors = false;
	m_cursors->set_cursor_for(e->Effect);
}
