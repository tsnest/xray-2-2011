////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Input;

namespace xray.editor.wpf_controls.property_editors.value
{
	partial class string_editor
	{
		public				string_editor	( )			
		{
			InitializeComponent();

			DataContextChanged += delegate 
			{
				if( DataContext == null )
					return;

				m_property				= (property)DataContext;
				m_text_box.IsEnabled	= !m_property.is_read_only;
			};
		}

		private				void	text_box_key_down	( Object sender, KeyEventArgs e )			
		{
			if(e.Key == Key.Return)
				m_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
		}
		private				void	handle_input		( Object sender, InputCommandEventArgs e )	
		{
			e.Handled = true;
		}

		public override		void	update				( )											
		{
			m_property.invalidate_value( );
		}
	}
}
