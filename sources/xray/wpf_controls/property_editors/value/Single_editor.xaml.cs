////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Data;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	partial class single_editor
	{

		#region | Initialize |


		public	single_editor	( )		
		{
			InitializeComponent( );
			
			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;
				m_property = (property)DataContext;

				var attribute = (value_range_and_format_attribute)m_property.descriptors[0].Attributes[typeof(value_range_and_format_attribute)];
				if (attribute != null)
				{
					m_is_update_on_release = attribute.update_on_edit_complete;

					m_step_size = (Single)attribute.step_size;
					m_precision = attribute.precision;
					if (m_step_size == 0)
						m_step_size = 0.1f;

					if( attribute.min_value_func != null && attribute.max_value_func != null )
					{
						m_min_value_func = attribute.min_value_func;
						m_max_value_func = attribute.max_value_func;
						progress_bar.Visibility = Visibility.Visible;
					}
					else
					{
						if( !Double.IsNaN( attribute.min_value ) && !Double.IsNaN( attribute.max_value ) )
							progress_bar.Visibility = Visibility.Visible;

						if( !Double.IsNaN( attribute.min_value ) )
							m_min_value = (Single)attribute.min_value;

						if( !Double.IsNaN( attribute.max_value ) )
							m_max_value = (Single)attribute.max_value;
					}
				}
				else
				{
					progress_bar.Visibility = Visibility.Collapsed;
				}

				m_text_box_binding = new Binding("value")
               	{
               		Converter				= new to_single_converter( this ),
               		UpdateSourceTrigger		= UpdateSourceTrigger.Explicit,
					Mode					= BindingMode.TwoWay,
               		NotifyOnValidationError = true
               	};
			
				m_text_box_binding.ValidationRules.Add( new validation_rule( this ) );
				text_box.SetBinding					( TextBox.TextProperty, m_text_box_binding );

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

				if( m_property.is_read_only )
				{
					text_box.IsReadOnly		= true;
					progress_bar.IsEnabled	= false;
					progress_bar.SetValue(Panel.ZIndexProperty, 250);
				}
				if( m_property.is_multiple_values )
				{
					text_box.Text = "<many>";
				}
			};

			m_editor_tool_tip.SetValue( ToolTipService.PlacementTargetProperty, text_box );
		}


		#endregion

		#region |   Fields   |


		Func<Double>	m_min_value_func;
		Func<Double>	m_max_value_func;
		Single			m_min_value =  Single.MinValue;
		Single			m_max_value =  Single.MaxValue;
		Single			m_step_size = 0.1f;
		Int32			m_precision = 4;
		Boolean			m_is_update_on_release;
		Boolean			m_is_validation_fail;

		Boolean			m_drag_started;
		Boolean			m_drag_initialized;
		Boolean			m_mouse_pressed;
		Point			m_initial_mouse;
		Single			m_initial_value;

		Binding			m_text_box_binding;
		Boolean			m_is_leaving;

		#endregion

		#region | Properties |


		Single			min_value
		{
			get
			{
				return ( m_min_value_func != null ) ? (Single)m_min_value_func( ) : m_min_value;
			}
		}
		Single			max_value
		{
			get
			{
				return ( m_max_value_func != null ) ? (Single)m_max_value_func( ) : m_max_value;
			}
		}


		#endregion

		#region |  Methods   |
		

		private				void	text_box_preview_mouse_left_button_down	( Object sender, MouseButtonEventArgs e )			
		{
			if( !m_property.is_read_only && Keyboard.PrimaryDevice.IsKeyDown( Key.LeftCtrl ) )
			{
				m_mouse_pressed		= true;
				e.Handled			= true;
			}
		}
		private				void	text_box_preview_mouse_move				( Object sender, MouseEventArgs e )					
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

				if ( !m_drag_initialized )
				{
					m_initial_value			= ( m_property.value is Single ) ? (Single)m_property.value : (Single)(Double)m_property.value;
					m_initial_mouse			= utils.get_screen_mouse_position( );
					m_drag_initialized		= true;
					return;
				}

				var pt				= utils.get_screen_mouse_position( );
				var mouse_delta		= pt.X - m_initial_mouse.X;

				if ( !m_drag_started && Math.Abs(mouse_delta) >= 5 )
				{
					m_drag_started			= true;
					text_box.Cursor			= Cursors.None;
					m_initial_mouse			= utils.get_screen_mouse_position( );
					mouse_delta				= 0;
					text_box.CaptureMouse	( );
				}
				if( m_drag_started )
				{
					if( mouse_delta != 0 )
					{

					}
					var value			= m_initial_value + ( mouse_delta ) * m_step_size;
					if( value == 0 )
					{
					}
					m_initial_value		= (Single)value;
					text_box.Text		= String.Format( "{0:F" + m_precision + "}", ( ( value < min_value ) ? min_value : ( ( value > max_value ) ? max_value : value ) ) );
					utils.set_screen_mouse_position ( m_initial_mouse );

					if( !m_is_update_on_release )
						text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
				}
			}
		}
		private				void	text_box_preview_mouse_left_button_up	( Object sender, MouseButtonEventArgs e )			
		{
			m_mouse_pressed		= false;

			if (m_drag_started)
			{
				m_drag_initialized	= false;
				m_drag_started		= false;
				
				text_box.ReleaseMouseCapture();
				text_box.Cursor				= null;
				if( m_is_update_on_release )
					m_property.value	= Single.Parse( text_box.Text );
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
		private				void	text_box_preview_mouse_double_click		( Object sender, MouseButtonEventArgs e )			
		{
			text_box.Focus();
			text_box.SelectAll();
		}
		private				void	text_box_error							( Object sender, ValidationErrorEventArgs e )		
		{
			if (e.Action == ValidationErrorEventAction.Added)
			{
				ToolTipService.SetIsEnabled		( text_box, true );
				m_editor_tool_tip.Content		= e.Error.ErrorContent;
				m_editor_tool_tip.Visibility	= Visibility.Visible;
				m_is_validation_fail			= true;
			}
			else
			{
				m_editor_tool_tip.IsOpen		= false;
				m_editor_tool_tip.Visibility	= Visibility.Collapsed;
				m_is_validation_fail			= false;
			}
		}
		private				void	text_box_lost_focus						( Object sender, RoutedEventArgs e )				
		{
			if( text_box.Text == "<many>" )
				return;

			if( m_is_leaving )
				return;

			if( m_is_validation_fail )
			{
				var value = ( m_property.value is Single ) ? (Single)m_property.value : (Single)(Double)m_property.value;
				text_box.Text		= String.Format( "{0:F" + m_precision + "}", ( ( value < min_value ) ? min_value : ( ( value > max_value ) ? max_value : value ) ) );
			}
			text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
		}
		private				void	text_box_preview_key_down				( Object sender, KeyEventArgs e )					
		{
			if( e.Key == Key.Escape )
			{
				if( m_property.is_multiple_values )
				{
					text_box.Text = "<many>";
					return;
				}
				var value = ( m_property.value is Single ) ? (Single)m_property.value : (Single)(Double)m_property.value;
				text_box.Text		= String.Format( "{0:F" + m_precision + "}", ( ( value < min_value ) ? min_value : ( ( value > max_value ) ? max_value : value ) ) );
				text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
				return;
			}

			if( e.Key != Key.Return )
				return;

			if( text_box.Text == "<many>" )
				return;

			text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
			if( m_is_validation_fail )
				return;

			m_is_leaving = true;
			text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
			m_is_leaving = false;
            e.Handled = true;
		}
		private				void	handle_input							( Object sender, InputCommandEventArgs e )			
		{
			e.Handled = true;
		}

		public override		void	update									( )													
		{
			text_box.GetBindingExpression( TextBox.TextProperty ).UpdateTarget( );
			m_property.invalidate_value();
		}


		#endregion

		#region | InnerTypes |

		
		class validation_rule : ValidationRule
		{
			public validation_rule(single_editor editor)
			{
				m_editor = editor;
			}
			private readonly single_editor m_editor;
			public override ValidationResult Validate(object value, System.Globalization.CultureInfo culture_info)
			{
				Single val;

				var number_string = (String)value;
				if( Thread.CurrentThread.CurrentCulture.NumberFormat.NumberDecimalSeparator == "," )
					number_string = number_string.Replace( '.', ',' );
				else
					number_string = number_string.Replace( ',', '.' );

				if( !Single.TryParse( number_string , out val ) )
					return new ValidationResult( false, "Can not parse to Single" );

				Double max;
				Double min;

				if ( m_editor.m_min_value_func != null && m_editor.m_max_value_func != null )
				{
					max = m_editor.m_max_value_func( );
					min = m_editor.m_min_value_func( );
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

				return new ValidationResult( false, String.Format( "value mast be between {0:N3} and {1:N3}, fix it.", min, max ) );
			}
		}

		class to_single_converter: IValueConverter
		{
			public to_single_converter( single_editor editor )
			{
				m_editor = editor;
			}
			private readonly single_editor m_editor;

			public		Object		Convert		( Object value, Type target_type, Object parameter, System.Globalization.CultureInfo culture )	
			{
				if( value == null )
					return "<many>";

				var val = System.Convert.ToDouble( value );
				return String.Format( "{0:F" + m_editor.m_precision + "}", val );
			}
			public		Object		ConvertBack	( Object value, Type target_type, Object parameter, System.Globalization.CultureInfo culture )	
			{
				return Single.Parse( Thread.CurrentThread.CurrentCulture.NumberFormat.NumberDecimalSeparator == ","
				                	? ( (String)value ).Replace( '.', ',' )
				                	: ( (String)value ).Replace( ',', '.' ) );
			}
		}


		#endregion

	}
}
