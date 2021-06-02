////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;
using System.Windows.Controls;
using System.Windows.Media;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	partial class external_editor
	{
		public		external_editor		( )		
		{
			InitializeComponent( );

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property		= (property)DataContext;

                m_property.PropertyChanged += (o, e) =>
                {
                    if ( e.PropertyName == "value" )
					{
						if( m_property.is_multiple_values )
							externalEditorTextBox.Text = m_property.values.All( value => value == m_property.values[0] ) ? m_property.values[0].ToString( ) : "<many>";
						else
							externalEditorTextBox.Text = m_property.value.ToString();
					}
                };

				m_attribute		= (external_editor_attribute)m_property.descriptors[0].Attributes[typeof(external_editor_attribute)];

				if( ( (ReadOnlyAttribute)m_property.descriptors[0].Attributes[typeof(ReadOnlyAttribute)]).IsReadOnly )
					run_editor_button.Visibility = Visibility.Collapsed;

				if( !m_attribute.m_is_clear_visible )
					m_clear_button.Visibility = Visibility.Collapsed;

				if( m_attribute.m_can_directly_set )
				{
					externalEditorTextBox.IsReadOnly = false;
					externalEditorTextBox.Foreground = Brushes.Black;
				}

				if( m_property.is_multiple_values )
					externalEditorTextBox.Text = "<many>";

				if( ( (property)DataContext ).is_read_only )
				{
					m_clear_button.IsEnabled		= false;
					run_editor_button.IsEnabled		= false;
				}
			};
		}

		private		external_editor_attribute			m_attribute;

		private				void	panel_preview_key_down		( Object sender, KeyEventArgs e )		
		{
			if ((e.Key == Key.Delete || e.Key == Key.Back) && m_attribute.m_is_clear_visible)
				clear_click(((DockPanel)sender).Children[1], new RoutedEventArgs());
		}
		private				void	run_editor_button_click		( Object sender, RoutedEventArgs e )	
		{
			var obj = (property)((Button)sender).DataContext;
			
			if( m_attribute.m_external_editor_type != null )
			{
				var editor = ((i_external_property_editor) Activator.CreateInstance( m_attribute.m_external_editor_type ));
				if( editor is i_additional_data )
					( (i_additional_data)editor ).additional_data = m_attribute.m_additional_data;
				editor.run_editor( obj );
			}
			else
				m_attribute.external_editor_delegate( obj,m_attribute.m_filter );
		
			var text_box = externalEditorTextBox;
			
			if( obj.value == null )
				text_box.Text = "";
			else
				text_box.Text = obj.value.ToString( );
		}
		private				void	clear_click					( Object sender, RoutedEventArgs e )	
		{
			var obj = (property)((Button)sender).DataContext;
			obj.value = "";
			externalEditorTextBox.Text = "";
		}
		private				void	text_box_key_down			( Object sender, KeyEventArgs e )		
		{
			if( e.Key == Key.Enter )
			{
				m_property.value = externalEditorTextBox.Text;
				externalEditorTextBox.MoveFocus( new TraversalRequest( FocusNavigationDirection.Left ) );
			}
		}

		public override		void	update						( )										
		{
			m_property.invalidate_value( );
		}

		private void text_box_lost_focus						( Object sender, RoutedEventArgs e )
		{
			if( externalEditorTextBox.IsReadOnly )
				return;

			if( (String)m_property.value != externalEditorTextBox.Text )
				m_property.value = externalEditorTextBox.Text;
		}
	}
}
