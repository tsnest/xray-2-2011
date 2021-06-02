////////////////////////////////////////////////////////////////////////////
//	Created		: 02.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Input;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for text_box_editor_extension.xaml
	/// </summary>
	public partial class text_box_editor_extension
	{
		public				text_box_editor_extension		( )			
		{
			InitializeComponent( );

			DataContextChanged += delegate {

				if( DataContext == null )
					return;

				m_property	= (property)DataContext;

				var attr	= (text_box_extension_attribute)m_property.descriptor.Attributes[typeof(text_box_extension_attribute)];
				m_setter	= attr.m_setter;
				m_getter	= attr.m_getter;

				m_value_editor					= item_editor.value_editor_selector.select_editor_without_extendion( m_property );
				m_value_editor_border.Child		= m_value_editor;

				m_text_box.Text					= m_getter( ).ToString( );
			};
		}

		private				Action<Single>		m_setter;
		private				Func<Single>		m_getter;
		private				value_editor_base	m_value_editor;
		
		private				void				text_box_key_down		( Object sender, KeyEventArgs e )			
		{
			if( e.Key == Key.Enter )
			{
				Single val;
				if( Single.TryParse( m_text_box.Text, out val ) )
				{
					m_setter( val );
				}
			}
		}
		private				void				on_lost_focus			( Object sender, RoutedEventArgs e )		
		{
			Single val;
			if( Single.TryParse( m_text_box.Text, out val ) )
			{
				m_setter( val );
			}
		}

		public override		void				update					( )											
		{
			m_text_box.Text			= m_getter( ).ToString( );
			m_value_editor.update	( );
		}
	}
}
