////////////////////////////////////////////////////////////////////////////
//	Created		: 13.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef MODE_BUTTON_H_INCLUDED
#define MODE_BUTTON_H_INCLUDED

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

			public ref class mode_button: Button
			{
				typedef Button	super;
			public:
				mode_button		( )			
				{
					ImageIndex = 0;
				}

			private:
				Int32				m_mode;
				List<String^>^		m_modes;
				array<MenuItem^>^	m_context_menu_items;
				System::Windows::Forms::ContextMenu^		m_context_menu;

			public:
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

			private:
				void			create_context_menu		( );
				void			context_item_click		( Object^ sender, EventArgs^ e );

			protected:
				virtual void 	OnMouseUp				( MouseEventArgs^ e )	override;
				virtual void	OnMouseClick			( MouseEventArgs^ e )	override;
				virtual void	OnClick					( EventArgs^ e )		override;

			}; // class mode_button

		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef MODE_BUTTON_H_INCLUDED