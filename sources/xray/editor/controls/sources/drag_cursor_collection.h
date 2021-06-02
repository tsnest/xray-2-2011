////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DRAG_CURSOR_COLLECTION_H_INCLUDED
#define DRAG_CURSOR_COLLECTION_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;

namespace xray
{
	namespace editor
	{
		namespace controls
		{
			public ref class drag_cursor_collection
			{
			#pragma region | Initialize |
			
			public:
				drag_cursor_collection();
				drag_cursor_collection(Control^ control);
				drag_cursor_collection(Drawing::Image^ image);

			#pragma endregion

			#pragma region |   Fields   |
				
			private:
				Cursor^					m_copy_cursor;
				Cursor^					m_move_cursor;
				Cursor^					m_no_drop_cursor;

			#pragma endregion

			#pragma region | Properties |
				
			public:
				property Cursor^		copy_cursor{
					Cursor^			get(){return m_copy_cursor;}
				}
				property Cursor^		move_cursor{
					Cursor^			get(){return m_move_cursor;}
				}
				property Cursor^		no_drop_cursor{
					Cursor^			get(){return m_no_drop_cursor;}
				}

			#pragma endregion

			#pragma region | Properties |

			public:
				void		make_cursors		(Control^ control);
				void		make_cursors		(Drawing::Image^ image);
				void		make_cursors		(Control^ control, Boolean show_std_cursor);
				void		make_cursors		(Drawing::Image^ image, Boolean show_std_cursor);
				void		set_cursor_for		(DragDropEffects effect);

			#pragma endregion

			}; // class drag_cursor_collection
		}//namespace controls
	}//namespace editor
}//namespace xray
#endif // #ifndef DRAG_CURSOR_COLLECTION_H_INCLUDED