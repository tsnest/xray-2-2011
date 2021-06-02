////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "drag_cursor_collection.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace xray {
namespace editor {
namespace controls{

				drag_cursor_collection::drag_cursor_collection	()
{}
				drag_cursor_collection::drag_cursor_collection	(Control^ control)
{
	make_cursors(control, true);
}

				drag_cursor_collection::drag_cursor_collection	(Drawing::Image^ image)
{
	make_cursors(image, true);
}

void			drag_cursor_collection::make_cursors		(Control^ control)
{
	make_cursors(control, true);
}

void			drag_cursor_collection::make_cursors		(Drawing::Image^ image)
{
	make_cursors(image, true);
}

void			drag_cursor_collection::make_cursors		(Control^ control, Boolean show_std_cursor)
{
	Bitmap^ bmp				= gcnew Bitmap(control->Width, control->Height);
	control->DrawToBitmap	(bmp, control->ClientRectangle);
	make_cursors			(bmp, show_std_cursor);
}

static Cursor^	make_cursor(Cursor^ source, Image^ image, Boolean show_std_cursor, Boolean center_std_cursor)
{
	Bitmap^		bmp				= gcnew Bitmap(image);

	if(show_std_cursor)
	{
		Bitmap^		bmp_cursor		= gcnew Bitmap(Cursor::Current->Size.Width, Cursor::Current->Size.Height);
		Graphics^	g_bmp			= Graphics::FromImage(bmp);
		
		if(center_std_cursor)
			source->Draw				(g_bmp, Rectangle(bmp->Width/2-bmp_cursor->Width/2, bmp->Height/2-bmp_cursor->Height/2, bmp_cursor->Width, bmp_cursor->Height));
		else
			source->Draw				(g_bmp, Rectangle(bmp->Width/2, bmp->Height/2, bmp_cursor->Width, bmp_cursor->Height));
	}

	return		gcnew Cursor(bmp->GetHicon());
}

static Cursor^	make_cursor(Cursor^ source, Image^ image, Boolean show_std_cursor)
{
	return make_cursor(source, image, show_std_cursor, false);
}

void			drag_cursor_collection::make_cursors		(Drawing::Image^ image, Boolean show_std_cursor)
{
	m_move_cursor		= make_cursor(Cursors::Arrow, image, show_std_cursor);
	m_copy_cursor		= make_cursor(Cursors::Arrow, image, show_std_cursor);
	m_no_drop_cursor	= make_cursor(Cursors::No, image, show_std_cursor, true);
}

void			drag_cursor_collection::set_cursor_for		(DragDropEffects effect)
{
	switch(effect)
	{
	case DragDropEffects::Copy:
		Cursor::Current	= m_copy_cursor;
		break;
	case DragDropEffects::None:
		Cursor::Current	= m_no_drop_cursor;
		break;
	case DragDropEffects::Move:
		Cursor::Current	= m_copy_cursor;
		break;
	default:
		Cursor::Current	= m_move_cursor;
	}
}

}//namespace controls
}//namespace editor
}//namespace xray