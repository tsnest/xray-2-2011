////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using xray.editor.wpf_controls.curve_editor.effects;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for item_view.xaml
	/// </summary>
	public partial class panel_curve_item
	{
		public				panel_curve_item		( panel_curve_group group )	
		{
			InitializeComponent( );
			m_parent_group = group;
		}

		private static		DependencyProperty		headerProperty				= DependencyProperty.Register( "header", typeof(String), typeof(panel_curve_item), new PropertyMetadata("unnamed") );
		private				Boolean					m_is_selected;
		private				float_curve				m_curve;
		private readonly	panel_curve_group		m_parent_group;

		internal			visual_curve			visual_curve;

		internal			curve_editor_panel		curves_panel				
		{
			get
			{
				return m_parent_group.parent_panel;
			}
		}
		internal			String					header						
		{
			get
			{
				return (String)GetValue( headerProperty );
			}
			set
			{
				SetValue( headerProperty, value );
			}
		}
		internal			String					curve_key					
		{
			get;
			set;
		}
		internal			float_curve				curve						
		{
			get 
			{
				return m_curve; 
			}
			set 
			{
				Debug.Assert( m_curve == null );
				m_curve							= value; 
				visual_curve					= curves_panel.add_curve( curve_key, m_curve );
				m_curve_color_rect.Background	= new SolidColorBrush( m_curve.color );

				visual_curve.double_click			+= curve_double_click;
				visual_curve.is_selected_changed	+= curve_selection_changed;
			}
		}

		public				Boolean					is_selected					
		{
			get 
			{
				return m_is_selected;
			}
			set 
			{
				m_is_selected				= value;
				visual_curve.Visibility		= ( m_is_selected ) ? Visibility.Visible : Visibility.Collapsed;
				Background					= ( m_is_selected ) ? new SolidColorBrush( Colors.Gray ) : new SolidColorBrush( Colors.Transparent );

				//foreach( panel_curve_effect effect in m_effects.Items )
				//    effect.is_visible = value;
			}
		}

		private				void					mouse_down					( Object sender, MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Left )
			{
				if( Keyboard.PrimaryDevice.IsKeyDown( Key.LeftCtrl ) )
					is_selected = !is_selected;
				else
				{
					curves_panel.deselect_all_groups( );
					is_selected = true;
				}
				e.Handled = true;
			}
		}
		private				void					mouse_move					( Object sender, MouseEventArgs e )				
		{
			if( e.LeftButton == MouseButtonState.Pressed )
			{
				is_selected = true;
			}
		}
		private				void					add_effect_click			( Object sender, RoutedEventArgs e )			
		{
			if( m_parent_group.m_effects.Items.Count > 0 )
			{
				MessageBox.Show( "You can not add effect to curve when group already has it.", "effect", MessageBoxButton.OK, MessageBoxImage.Information );
				return;
			}

			var noise							= new float_curve_noise_effect( );
			var effect							= new visual_noise_effect( visual_curve, noise );
			var effect_item						= new panel_curve_effect( this, noise ){ name = "noise" };
			effect_item.add_effect				( effect );
			m_effects.Items.Add					( effect_item ); 
			effect.open_effect_settings			( Window.GetWindow( this ) );
			e.Handled							= true;
		}
		private				void					curve_double_click			( )												
		{
			if( !curves_panel.double_click_key_adding_enabled )
			{
				curves_panel.deselect_all_groups( );
				is_selected = true;
			}
		}
		private				void					curve_selection_changed		( )												
		{
			m_text_box.SetValue( ForegroundProperty, visual_curve.is_selected ? new SolidColorBrush( Colors.Yellow ) : DependencyProperty.UnsetValue );
		}

		internal			void					remove_effect				( panel_curve_effect effect )					
		{
			m_effects.Items.Remove			( effect );

			if( visual_curve.effects.Count == 0 )
				visual_curve.path.StrokeDashArray	= null;

		}
	}
}
