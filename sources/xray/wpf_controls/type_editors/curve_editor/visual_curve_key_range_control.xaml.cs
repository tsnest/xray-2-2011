////////////////////////////////////////////////////////////////////////////
//	Created		: 19.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for visual_curve_key_range_control.xaml
	/// </summary>
	internal partial class visual_curve_key_range_control
	{
		public		visual_curve_key_range_control		( visual_curve_key parent_key )		
		{
			InitializeComponent( );

			m_parent_key	= parent_key;
			SetValue		( Panel.ZIndexProperty, 300 );

			Visibility		= Visibility.Collapsed;
			update_visual	( );

			Background		= m_background_brush = new SolidColorBrush( parent_key.parent_curve.float_curve.color );
		}

		private		Brush					m_background_brush;
		private		visual_curve_key		m_parent_key;
		private		Boolean					m_is_selected;
		
		internal	Double					range_delta			
		{
			get				
			{
				return m_parent_key.key.range_delta;
			}
			set		
			{
				m_parent_key.key.range_delta	= (Single)value;
				visual_position					= parent_key.parent_curve.parent_panel.scale.Y * value;
				m_parent_key.rage_changed		( );
			}
		}
		internal	Boolean					is_selected			
		{
			get	
			{
				return m_is_selected;
			}
			set 
			{
				m_is_selected = value;
				if ( value )
				{
					Background					= Brushes.Yellow;
					parent_key.parent_curve.selected_key_range_controls.Add( this );
				}
				else
				{
					Background					= m_background_brush;
					parent_key.parent_curve.selected_key_range_controls.Remove( this );
				}
			}
		}
		internal	visual_curve_key		parent_key			
		{
			get
			{
				return m_parent_key;
			}
		}
		internal	Double					visual_position		
		{
			set
			{
				SetValue( Canvas.LeftProperty, ( parent_key.Width - Width ) / 2);
				SetValue( Canvas.TopProperty, -value + Height / 2 );
			}
		}

		internal	void					process_selection	( )							
		{
			Visibility = Visibility.Visible;

			var shift_pressed	= Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift);
			var ctrl_pressed	= Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);

			if ( shift_pressed && ctrl_pressed )
				is_selected = true;
			else if ( ctrl_pressed )
				is_selected = false;
			else if ( shift_pressed )
				is_selected = !is_selected;
			else
				is_selected = true;

			return;
		}
		internal	void					move_to				( Double new_delta )		
		{
			range_delta = new_delta;
		}
		internal	void					update_visual		( )							
		{
			visual_position					= parent_key.parent_curve.parent_panel.scale.Y * m_parent_key.key.range_delta;
		}
	}
}