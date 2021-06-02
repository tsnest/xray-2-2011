////////////////////////////////////////////////////////////////////////////
//	Created		: 17.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MODE_STRIP_BUTTON_H_INCLUDED
#define MODE_STRIP_BUTTON_H_INCLUDED

using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::Windows::Forms;

namespace xray
{
	namespace editor
	{
		namespace controls
		{

			public ref class mode_strip_button: ToolStripButton
			{
				typedef ToolStripButton	super;
			public:
				mode_strip_button		( )			
				{
					ImageIndex			= 0;
					can_iterate_modes	= true;
				}

			private:
				Int32				m_mode;
				List<String^>^		m_modes;
				array<MenuItem^>^	m_context_menu_items;
				ImageList^			m_image_list;
				System::Windows::Forms::ContextMenu^		m_context_menu;

			public:
				property	Int32			image_offset;			
				[EditorBrowsableAttribute(EditorBrowsableState::Advanced), BrowsableAttribute(false)]
				[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
				property	Int32			current_mode			
				{
					Int32			get	( );
					void			set	( Int32 value );
				}
				[EditorBrowsableAttribute(EditorBrowsableState::Advanced), BrowsableAttribute(false)]
				[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
				property	List<String^>^	modes					
				{
					List<String^>^	get ( );
					void			set ( List<String^>^ );
				}
				property	String^			current_mode_name		
				{
					String^			get ( );
				}
				property	Boolean			can_iterate_modes;
				[EditorBrowsableAttribute(EditorBrowsableState::Advanced), BrowsableAttribute(false)]
				[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
				property	ImageList^		image_list
				{
					ImageList^		get	( );
					void			set	( ImageList^ value );
				} 

			private:
				void			create_context_menu		( );
				void			context_item_click		( Object^ sender, EventArgs^ e );

			protected:
				virtual void 	OnMouseUp				( MouseEventArgs^ e )	override;
				virtual void	OnClick					( EventArgs^ e )		override;
				virtual void	on_click				( ) {}
				
			}; // class mode_strip_button

		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef MODE_STRIP_BUTTON_H_INCLUDED