////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace xray.editor.wpf_controls.curve_editor.tools
{
	internal class move_range_tool: tool_base
	{
		public move_range_tool( curve_editor_panel panel ): base( panel )
		{
		}

		private					Vector		m_offset_vector;
		private					Point		m_mouse_position;

		public override			Boolean			mouse_down		( MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Middle && Keyboard.PrimaryDevice.Modifiers == ModifierKeys.None && 
				( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.keys || 
				m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.key_range_controls ) )
			{
				m_offset_vector			= new Vector( 0, 0 );
				m_mouse_position		= Mouse.GetPosition( m_parent_panel.curves_panel );
				m_parent_panel.curves_panel.CaptureMouse	( );
				m_parent_panel.curves_panel.Cursor			= Cursors.SizeAll;
				m_is_in_action			= true;

				if( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.keys )
				{
					foreach( var curve_key in m_parent_panel.selected_keys.ToArray( ) )
					{
						curve_key.is_selected					= false;
						if( curve_key.parent_curve.is_range )
						{
							curve_key.range_control.is_selected		= true;
							curve_key.range_control.Visibility		= Visibility.Visible;
						}
					}
					m_parent_panel.selected_entity			= curve_editor_panel.selected_entity_type.key_range_controls;
					m_parent_panel.initialize_selected_items_position_viewer( );
				}

				return true;

			}
			return false;
		}
		public override			Boolean			mouse_move		( MouseEventArgs e )			
		{
			if( !m_is_in_action )
				return false;

			var new_mouse_position	= Mouse.GetPosition( m_parent_panel.curves_panel );
			var mouse_offset		= (Point)( new_mouse_position - m_mouse_position );
			m_mouse_position		= new_mouse_position;
			mouse_offset.Y			= -mouse_offset.Y;

			var logical_mouse_offset	= new Vector( mouse_offset.X / m_parent_panel.scale.X, mouse_offset.Y / m_parent_panel.scale.Y );
			m_offset_vector				+= logical_mouse_offset;
			var current_offset			= m_offset_vector;

			if( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.key_range_controls )
			{
				if( m_parent_panel.selected_points_top_left.Y + current_offset.Y < 0 )
				    current_offset.Y += -( m_parent_panel.selected_points_top_left.Y + current_offset.Y );

				var index = 0;
				foreach( var item in m_parent_panel.items.OfType<visual_curve>( ) )
				{
					var flag = false;
					foreach ( var rage_control in item.selected_key_range_controls )
					{
						rage_control.move_to( ( m_parent_panel.points_positions[index++] + current_offset ).Y );
						flag = true;
					}
					if( flag && m_parent_panel.is_frequently_changed )
						m_parent_panel.on_float_curve_changed( item.float_curve );
				}
			}
			
			m_parent_panel.update_selected_items_position_viewer( );

			return true;
		}
		public override 		Boolean		mouse_up		( MouseButtonEventArgs e )		
		{
			if( !m_is_in_action )
				return false;

			m_is_in_action = false;

			foreach ( var curve in m_parent_panel.items.OfType<visual_curve>( ).Where( curve => curve.selected_key_range_controls.Count > 0 ) )
				m_parent_panel.on_float_curve_changed( curve.float_curve );

			m_parent_panel.curves_panel.Cursor					= null;
			m_parent_panel.curves_panel.ReleaseMouseCapture		( );

			return true;
		}
	}
}
