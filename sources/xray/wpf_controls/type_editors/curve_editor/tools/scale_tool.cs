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
	internal class scale_tool: tool_base
	{
		public scale_tool( curve_editor_panel panel ):base( panel )
		{
			
		}

		private					Vector		m_offset_vector;
		private					Point		m_mouse_position;
		private					Point		m_mouse_start_position;

		public override			Boolean			mouse_down		( MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Middle && Keyboard.PrimaryDevice.Modifiers == ModifierKeys.None &&
				m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.keys )
			{
				m_offset_vector			= new Vector( 0, 0 );
				m_mouse_position		= Mouse.GetPosition( m_parent_panel.curves_panel );
				m_mouse_start_position	= m_mouse_position;
				m_parent_panel.curves_panel.CaptureMouse	( );
				m_parent_panel.curves_panel.Cursor			= Cursors.SizeAll;
				m_is_in_action			= true;

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

			if( Keyboard.PrimaryDevice.IsKeyDown( Key.LeftShift ) || Keyboard.PrimaryDevice.IsKeyDown( Key.RightShift ) )
			{
				var delta = m_mouse_start_position - m_mouse_position;
				if( Math.Abs( delta.X ) > Math.Abs( delta.Y ) )
					current_offset.Y = 0;
				else
					current_offset.X = 0;
			}

			if( m_parent_panel.selected_keys.Count == 0 )
				return true;

			if( m_parent_panel.selected_points_top_left.X + current_offset.X < m_parent_panel.field_left_limit )
				current_offset.X = m_parent_panel.field_left_limit - m_parent_panel.selected_points_top_left.X;

			if( m_parent_panel.selected_points_top_left.Y + current_offset.Y > m_parent_panel.field_top_limit )
				current_offset.Y = m_parent_panel.field_top_limit - m_parent_panel.selected_points_top_left.Y;

			if( m_parent_panel.selected_points_bottom_right.X + current_offset.X > m_parent_panel.field_right_limit )
				current_offset.X = m_parent_panel.field_right_limit - m_parent_panel.selected_points_bottom_right.X;

			if( m_parent_panel.selected_points_bottom_right.Y + current_offset.Y < m_parent_panel.field_bottom_limit )
				current_offset.Y = m_parent_panel.field_bottom_limit - m_parent_panel.selected_points_bottom_right.Y;

			var scale_factor			= ( current_offset / 100 );

			var count = m_parent_panel.points_positions.Count;
			for ( var i = 0; i < count; ++i )
			{
				var logical_start_position	= m_parent_panel.visual_to_logical_point( m_mouse_start_position );
				var point_position			= m_parent_panel.points_positions[i];

				var offset					= point_position - (Vector)logical_start_position;
				offset.X					*= scale_factor.X;
				offset.Y					*= scale_factor.Y;
				
				var new_position			= point_position + (Vector)offset;

				if( m_parent_panel.fix_point_position != null )
					m_parent_panel.fix_point_position( ref new_position );

				m_parent_panel.selected_keys[i].move_to	( new_position );
			}

			if( m_parent_panel.is_frequently_changed )
				foreach( var visual_curve in m_parent_panel.items.OfType<visual_curve>( ).Where( visual_curve => visual_curve.selected_keys.Count != 0 ) )
					m_parent_panel.on_float_curve_changed( visual_curve.float_curve );
		
			m_parent_panel.update_selected_items_position_viewer( );

			return true;
		}
		public override 		Boolean		mouse_up		( MouseButtonEventArgs e )		
		{
			if( !m_is_in_action )
				return false;

			m_is_in_action = false;

			foreach ( var curve in m_parent_panel.items.OfType<visual_curve>( ).Where(curve => curve.selected_keys.Count > 0 ) )
				m_parent_panel.on_float_curve_changed( curve.float_curve );

			m_parent_panel.curves_panel.Cursor					= null;
			m_parent_panel.curves_panel.ReleaseMouseCapture		( );

			return true;
		}
	}
}
