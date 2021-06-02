////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;

namespace xray.editor.wpf_controls.type_editors
{
	public class number_editor: Control
	{
		#region | Initialize |


		static	number_editor	( )	
		{
			DefaultStyleKeyProperty.OverrideMetadata( typeof(number_editor), new FrameworkPropertyMetadata( typeof(number_editor) ) );
		}


		#endregion

		#region |   Fields   |

		
		private			Func<float>	m_min_value_func;
		private			Func<float>	m_max_value_func;
		
		private			TextBox			m_text_box;
		private			ToolTip			m_editor_tool_tip;
		private			Binding			m_text_box_binding;

		private			Boolean			m_is_validation_fail;
		private			Boolean			m_drag_started;
		private			Boolean			m_drag_initialized;
		private			Boolean			m_mouse_pressed;
		private			Point			m_initial_mouse;
		private			Single			m_initial_value;


		#endregion

		#region | Properties |


		public static	DependencyProperty	valueProperty			= DependencyProperty.Register( "value", typeof( Single ), typeof( number_editor ), new PropertyMetadata( new Single( ), value_changed_callback ) );
		public			Single				value					
		{
			get 
			{
				return (Single)GetValue( valueProperty ); 
			}
			set 
			{
				SetValue( valueProperty, value );
			}
		}

		public static	DependencyProperty	min_valueProperty		= DependencyProperty.Register( "min_value", typeof( Single ), typeof( number_editor ), new PropertyMetadata( Single.MinValue ) );
		public			Single				min_value				
		{
			get 
			{
				return (Single)GetValue( min_valueProperty ); 
			}
			set 
			{
				SetValue( min_valueProperty, value );
			}
		}

		public static	DependencyProperty	max_valueProperty		= DependencyProperty.Register( "max_value", typeof( Single ), typeof( number_editor ), new PropertyMetadata( Single.MaxValue ) );
		public			Single				max_value				
		{
			get 
			{
				return (Single)GetValue( max_valueProperty ); 
			}
			set 
			{
				SetValue( max_valueProperty, value );
			}
		}

		public static	DependencyProperty	precisionProperty		= DependencyProperty.Register( "precision", typeof( Int32 ), typeof( number_editor ), new PropertyMetadata( 3 ) );
		public			Int32				precision				
		{
			get 
			{
				return (Int32)GetValue( precisionProperty ); 
			}
			set 
			{
				SetValue( precisionProperty, value );
			}
		}

		public static	DependencyProperty	step_sizeProperty		= DependencyProperty.Register( "step_size", typeof( Single ), typeof( number_editor ), new PropertyMetadata( 0.1f ) );
		public			Single				step_size				
		{
			get 
			{
				return (Single)GetValue( step_sizeProperty );
			}
			set 
			{
				SetValue( step_sizeProperty, value );
			}
		}

		private			Single				min_value_internal		
		{
			get 
			{
				return ( m_min_value_func != null ) ? m_min_value_func( ) : min_value;
			}
		}
		private			Single				max_value_internal		
		{
			get 
			{
				return ( m_max_value_func != null ) ? m_max_value_func( ) : max_value;
			}
		}

		public			Func<float>		min_value_func			
		{
			get 
			{
				return m_min_value_func; 
			}
			set 
			{
				m_min_value_func = value; 
			}
		}
		public			Func<float>		max_value_func			
		{
			get 
			{
				return m_max_value_func; 
			}
			set 
			{
				m_max_value_func = value; 
			}
		}

		
		#endregion

		#region |   Events   |


		public event		Action	value_changed;


		#endregion

		#region |  Methods   |
		

		private static		void	value_changed_callback					( DependencyObject d, DependencyPropertyChangedEventArgs e )	
		{
			var editor = (number_editor)d;

			if( editor.value_changed != null )
				editor.value_changed( );
		}

		private				void	text_box_preview_mouse_left_button_down	( Object sender, MouseButtonEventArgs e )			
		{
			if( Keyboard.PrimaryDevice.IsKeyDown( Key.LeftCtrl ) )
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
					
					m_text_box.ReleaseMouseCapture();
					m_text_box.Cursor = null;

					return;
				}

				if ( !m_drag_initialized )
				{
					m_initial_value			= value;
					m_initial_mouse			= utils.get_screen_mouse_position( );
					m_drag_initialized		= true;
					return;
				}

				var pt				= utils.get_screen_mouse_position( );
				var mouse_delta		= pt.X - m_initial_mouse.X;

				if ( !m_drag_started && Math.Abs(mouse_delta) >= 5 )
				{
					m_drag_started			= true;
					m_text_box.Cursor			= Cursors.None;
					m_initial_mouse			= utils.get_screen_mouse_position( );
					mouse_delta				= 0;
					m_text_box.CaptureMouse	( );
				}
				if( m_drag_started )
				{
					var new_value		= (Single)( m_initial_value + ( mouse_delta ) * step_size );

					Single max;
					Single min;

					if ( m_min_value_func != null && m_max_value_func != null )
					{
						max = m_max_value_func( );
						min = m_min_value_func( );
						max_value = max;
						min_value = min;
					}
					else
					{
						min = min_value;
						max = max_value;
					}

					if( new_value < min )		new_value = min;
					if( new_value > max )		new_value = max;

					value				= new_value;
					m_initial_value		= value;
					//m_text_box.Text		= String.Format( "{0:F" + m_precision + "}", ( ( value < min_value_internal ) ? min_value_internal : ( ( value > max_value_internal ) ? max_value_internal : value ) ) );
					utils.set_screen_mouse_position ( m_initial_mouse );
				}
			}
		}
		private				void	text_box_preview_mouse_left_button_up	( Object sender, MouseButtonEventArgs e )			
		{
			m_mouse_pressed		= false;

			if ( m_drag_started )
			{
				m_drag_initialized	= false;
				m_drag_started		= false;
				
				m_text_box.ReleaseMouseCapture( );
				m_text_box.Cursor				= null;
			}
			else
			{
				if( !m_text_box.IsFocused )
				{
					m_text_box.Focus( );
					m_text_box.SelectAll( );
				}
			}
		}
		private				void	text_box_preview_mouse_double_click		( Object sender, MouseButtonEventArgs e )			
		{
			m_text_box.Focus		( );
			m_text_box.SelectAll	( );
		}
		private				void	text_box_error							( Object sender, ValidationErrorEventArgs e )		
		{
			if ( e.Action == ValidationErrorEventAction.Added )
			{
				ToolTipService.SetIsEnabled		( m_text_box, true );
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
			if( m_is_validation_fail )
				m_text_box.Text		= String.Format( "{0:F" + precision + "}", ( ( value < min_value_internal ) ? min_value_internal : ( ( value > max_value_internal ) ? max_value_internal : value ) ) );

			m_text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
		}
		private				void	text_box_preview_key_down				( Object sender, KeyEventArgs e )					
		{
			if( e.Key == Key.Escape )
			{
				m_text_box.Text		= String.Format( "{0:F" + precision + "}", ( ( value < min_value_internal ) ? min_value_internal : ( ( value > max_value_internal ) ? max_value_internal : value ) ) );
				m_text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
				return;
			}

			if( e.Key != Key.Return )
				return;

			m_text_box.GetBindingExpression( TextBox.TextProperty ).UpdateSource( );
			if( m_is_validation_fail )
				return;

			m_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Left ) );
			e.Handled = true;
		}
		private				void	handle_input							( Object sender, InputCommandEventArgs e )			
		{
			e.Handled = true;
		}

		public override		void	OnApplyTemplate							( )													
		{
			m_text_box		= (TextBox)GetTemplateChild( "PART_text_box" );
			Debug.Assert	( m_text_box != null );

			m_text_box.PreviewMouseLeftButtonDown	+= text_box_preview_mouse_left_button_down;
			m_text_box.PreviewMouseMove				+= text_box_preview_mouse_move;
			m_text_box.PreviewMouseLeftButtonUp		+= text_box_preview_mouse_left_button_up;
			m_text_box.PreviewMouseDoubleClick		+= text_box_preview_mouse_double_click;
			m_text_box.LostFocus					+= text_box_lost_focus;
			m_text_box.PreviewKeyDown				+= text_box_preview_key_down;
			Validation.AddErrorHandler				( m_text_box, text_box_error );
			input.AddPreviewInputCommandHandler		( m_text_box, handle_input );

			m_text_box_binding						= new Binding( "value" ){
				RelativeSource			= RelativeSource.TemplatedParent,
				Converter				= new to_single_converter( this ),
				UpdateSourceTrigger		= UpdateSourceTrigger.LostFocus,
				Mode					= BindingMode.TwoWay,
				NotifyOnValidationError = true
			};
			m_text_box_binding.ValidationRules.Add	( new validation_rule( this ) );
			m_text_box.SetBinding					( TextBox.TextProperty, m_text_box_binding );

			m_editor_tool_tip = (ToolTip)GetTemplateChild( "PART_editor_tool_tip" );
			Debug.Assert	( m_editor_tool_tip != null );
			m_editor_tool_tip.SetValue( ToolTipService.PlacementTargetProperty, m_text_box );

			base.OnApplyTemplate( );
		}


		#endregion

		#region | InnerTypes |

		
		private class validation_rule : ValidationRule
		{
			public validation_rule(number_editor editor)
			{
				m_editor = editor;
			}
			private readonly number_editor m_editor;
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

				Single max;
				Single min;

				if ( m_editor.m_min_value_func != null && m_editor.m_max_value_func != null )
				{
					max = m_editor.m_max_value_func( );
					min = m_editor.m_min_value_func( );
					m_editor.max_value = max;
					m_editor.min_value = min;
				}
				else
				{
					min = m_editor.min_value;
					max = m_editor.max_value;
				}

				if ( val >= min && val <= max )
					return new ValidationResult( true, "" );

				return new ValidationResult( false, String.Format( "value mast be between {0:N3} and {1:N3}, fix it.", min, max ) );
			}
		}

		private class to_single_converter: IValueConverter
		{
			public to_single_converter( number_editor editor )
			{
				m_editor = editor;
			}
			private readonly number_editor m_editor;

			public		Object		Convert		( Object value, Type target_type, Object parameter, System.Globalization.CultureInfo culture )	
			{
				if( value == null )
					return "<many>";

				var val = System.Convert.ToDouble( value );
				return String.Format( "{0:F" + m_editor.precision + "}", val );
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
