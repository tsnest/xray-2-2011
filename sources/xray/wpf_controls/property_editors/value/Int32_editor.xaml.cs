////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	partial class int32_editor
	{
		public int32_editor( )
		{
			InitializeComponent();

			EditorToolTip.SetValue(ToolTipService.PlacementTargetProperty, text_box);
			
			DataContextChanged += (o, e) =>
			{
				if( DataContext == null )
					return;
				m_property = (property)e.NewValue;
				
				text_box.GetBindingExpression(TextBox.TextProperty).ParentBinding.ValidationRules.Add(new validation_rule(this));
				
				var attribute = (value_range_and_format_attribute)m_property.descriptors[0].Attributes[typeof(value_range_and_format_attribute)];
				if (attribute != null)
				{
					m_is_update_on_release = attribute.update_on_edit_complete;

					m_min_value_func = attribute.min_value_func;
					m_max_value_func = attribute.max_value_func;

					m_min_value = (Int32)attribute.min_value;
					m_max_value = (Int32)attribute.max_value;
					m_step_size = (Int32)attribute.step_size;
					if (m_step_size == 0)
						m_step_size = 1;
				}
				else
				{
					progress_bar.Visibility = Visibility.Collapsed;
				}

				if (m_min_value_func != null && m_max_value_func != null)
				{
					progress_bar.Maximum = m_max_value_func();
					progress_bar.Minimum = m_min_value_func();
				}
				else
				{
					progress_bar.Maximum = m_max_value;
					progress_bar.Minimum = m_min_value;
				}

				if( ( (property)DataContext ).is_read_only )
				{
					text_box.IsReadOnly		= true;
					progress_bar.IsEnabled	= false;
					progress_bar.SetValue	( Panel.ZIndexProperty, 250 );
				}
			};
		}

		Func<Double>	m_min_value_func;
		Func<Double>	m_max_value_func;
		Int32			m_min_value = Int32.MinValue;
		Int32			m_max_value = Int32.MaxValue;
		Int32			m_step_size = 1;

		Boolean			m_is_update_on_release;
		Boolean			m_is_validation_fail;
		Boolean			m_drag_started;
		Boolean			m_drag_initialized;
		Boolean			m_mouse_pressed;
		Point			m_initial_mouse;
		Int32			m_initial_value;

		private				void	text_box_preview_mouse_left_button_down		( Object sender, MouseButtonEventArgs e )		
		{
			if( !m_property.is_read_only && Keyboard.PrimaryDevice.IsKeyDown( Key.LeftCtrl ) )
			{
				m_mouse_pressed		= true;
				e.Handled			= true;
			}
		}
		private				void	text_box_preview_mouse_move					( Object sender, MouseEventArgs e )				
		{
			if ( m_mouse_pressed )
			{
				if ( m_drag_started && e.LeftButton == MouseButtonState.Released )
				{
					m_drag_started		= false;
					m_drag_initialized	= false;
					m_mouse_pressed		= false;
					text_box.ReleaseMouseCapture();
					text_box.Cursor = null;
					return;
				}

				e.Handled = true;

				if (!m_drag_initialized)
				{
                    m_initial_value			= (Int32)Convert.ChangeType( m_property.value, typeof(Int32) );
					m_drag_initialized		= true;
					m_initial_mouse			= utils.get_screen_mouse_position( );
					return;
				}
				
				var pt				= utils.get_screen_mouse_position( );
				var mouse_delta		= pt.X - m_initial_mouse.X;

				if( !m_drag_started && Math.Abs(mouse_delta) >= 5 )
				{
					text_box.MoveFocus		( new TraversalRequest( FocusNavigationDirection.Previous ) );
					m_drag_started			= true;
					text_box.Cursor			= Cursors.None;
					m_initial_mouse			= utils.get_screen_mouse_position( );
					mouse_delta				= 0;
					text_box.CaptureMouse	( );
				}

				if( m_drag_started )
				{
					var value = m_initial_value + ( mouse_delta ) * m_step_size;
                    if ( value < 0 && m_property.type == typeof(UInt32) )
                        value = 0;
					text_box.Text = ((value < m_min_value)?m_min_value:((value>m_max_value)?m_max_value:value)).ToString();
					m_initial_value		= (Int32)value;
					utils.set_screen_mouse_position ( m_initial_mouse );

					if( !m_is_update_on_release )
						text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
				}
			}
		}
		private				void	text_box_preview_mouse_left_button_up		( Object sender, MouseButtonEventArgs e )		
		{
			m_mouse_pressed		= false;

			if (m_drag_started)
			{
				m_drag_initialized	= false;
				m_drag_started		= false;
				
				text_box.ReleaseMouseCapture();
				text_box.Cursor				= null;
				if( m_is_update_on_release )
					text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
			}
			else
			{
				if( !text_box.IsFocused )
				{
					text_box.Focus();
					text_box.SelectAll();
				}
			}
		}
		private				void	text_box_preview_mouse_double_click			( Object sender, MouseButtonEventArgs e )		
		{
			text_box.Focus();
			text_box.SelectAll();
		}
		private				void	text_box_error								( Object sender, ValidationErrorEventArgs e )	
		{
			if (e.Action == ValidationErrorEventAction.Added)
			{
				ToolTipService.SetIsEnabled		( text_box, true );
				EditorToolTip.Content			= e.Error.ErrorContent;
				EditorToolTip.Visibility		= Visibility.Visible;
				m_is_validation_fail			= true;
			}
			else
			{
				EditorToolTip.IsOpen			= false;
				EditorToolTip.Visibility		= Visibility.Collapsed;
				m_is_validation_fail			= false;
			}
		}
		private				void	text_box_preview_key_down					( Object sender, KeyEventArgs e	)				
		{
			if( e.Key == Key.Escape )
			{
				var value = (Int32)m_property.value;
				text_box.Text = ((value < m_min_value)?m_min_value:((value>m_max_value)?m_max_value:value)).ToString();
				text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
				return;
			}

			if( e.Key != Key.Return )
				return;

			text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );

			if( m_is_validation_fail )
				return;

			text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
		}
		private				void	text_box_lost_focus							( Object sender, RoutedEventArgs e )			
		{
			if( m_is_validation_fail )
			{
				var value = (Int32)m_property.value;
				text_box.Text = ((value < m_min_value)?m_min_value:((value>m_max_value)?m_max_value:value)).ToString();
			}
			text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
		}
		private				void	handle_input								( Object sender, InputCommandEventArgs e )		
		{
			e.Handled = true;
		}

		public override		void	update										( )												
		{
			text_box.GetBindingExpression( TextBox.TextProperty ).UpdateTarget( );
		}


		#region | InnerTypes |


		class validation_rule : ValidationRule
		{
			public validation_rule(int32_editor editor)
			{
				m_editor = editor;
			}

			private int32_editor m_editor;

			public override ValidationResult Validate(object value, System.Globalization.CultureInfo culture_info)
			{
				Int32 val;

				if(!Int32.TryParse((String)value, out val))
					return new ValidationResult(false, "Can not parse to Int32");

				Int32 max;
				Int32 min;

				if ( m_editor.m_min_value_func != null && m_editor.m_max_value_func != null )
				{
					max = (Int32)m_editor.m_max_value_func( );
					min = (Int32)m_editor.m_min_value_func( );
					m_editor.progress_bar.Maximum = max;
					m_editor.progress_bar.Minimum = min;

					
				}
				else
				{
					min = m_editor.m_min_value;
					max = m_editor.m_max_value;
				}

				if ( val >= min && val <= max )
					return new ValidationResult( true, "" );

				return new ValidationResult( false, String.Format( "value mast be between {0} and {1}, fix it.", min, max ) );
			}
		}

		#endregion

	}	
}
