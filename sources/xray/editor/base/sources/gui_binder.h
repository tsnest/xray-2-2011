////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GUI_BINDER_H_INCLUDED
#define GUI_BINDER_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;

using Windows::Forms::ToolStripMenuItem;
using Windows::Forms::ToolStripButton;
using Windows::Forms::MenuStrip;
using Windows::Forms::ImageList;
using Windows::Forms::PaintEventArgs;
using System::Drawing::Image;

namespace xray
{	
	namespace editor_base
	{

		ref class input_engine;
		ref class action;

		public ref class gui_binder
		{
		public:
			gui_binder( input_engine^ input_engine );

		public:
			void			add_action_menu_item	( MenuStrip^ menuStrip, String^ action_name, String^ strip_key, u32 prio );
			void			remove_action_menu_item	( MenuStrip^ menuStrip, String^ action_name, String^ strip_key );
			void			add_menu_item			( MenuStrip^ strip, String^ strip_key, String^ text, u32 prio );

			void			bind					( ToolStripMenuItem^ menu_item, String^ action_name );
			void			bind					( ToolStripMenuItem^ menu_item, action^ action );
			void			bind					( ToolStripButton^ button, String^ action_name );
			void			bind					( ToolStripButton^ button, action^ action );
			void			unbind					( ToolStripMenuItem^ menu_item );
			void			unbind					( ToolStripButton^ button );

			void			update_menu_item		( ToolStripMenuItem^ item );
			void			update_button			( ToolStripButton^ button );
			void			update_button			( ToolStripButton^ button, String^ action_name );
			void			update_button			( ToolStripButton^ button, action^ action );
			void			update_items			( );

			void			gui_menu_drop_down		( Object^ sender, EventArgs^  e );
			Image^			get_image				( String^ key );
			void			set_image_list			( String^ list_id, ImageList^ l );
			void			register_image			( String^ lmage_list_key, String^ image_key, s32 image_index );

		private:
			void			gui_control_click		( Object^  sender, EventArgs^  e );
			void			gui_menu_item_paint		( Object^  sender, PaintEventArgs^ e );
			Image^			get_image_internal		( String^ key );

		private:
			input_engine^							m_input_engine;
			Collections::Hashtable					m_menu_items;
			Collections::Hashtable					m_buttons;

			Dictionary<String^, ImageList^>			m_images;
			Dictionary<String^, String^>			m_image_key_aliases;

		}; // class gui_binder

	} // namespace editor_base	
} // namespace xray

#endif // #ifndef GUI_BINDER_H_INCLUDED