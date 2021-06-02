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
	using hermite_spline_evaluator = wpf_controls.hermite_spline_evaluator;

	internal class move_tool: tool_base
	{
		public move_tool( curve_editor_panel panel ): base( panel )
		{
			 m_evaluator = new hermite_spline_evaluator( );
		}

		private					Vector						m_offset_vector;
		private					Vector						m_last_offset;
		private					Point						m_mouse_position;
		private					Point						m_mouse_start_position;
		private					hermite_spline_evaluator	m_evaluator;

		private					Boolean		curves_valid	( )								
		{
			var count = ( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.keys )
				? m_parent_panel.selected_keys.Count
				: m_parent_panel.selected_key_tangents.Count;

			for ( var i = 0; i < count; ++i )
			{
				var key = ( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.keys )
				? m_parent_panel.selected_keys[i]
				: m_parent_panel.selected_key_tangents[i].parent_key;

				var monotone_type = key.parent_curve.monotone_type;
				if( monotone_type == float_curve_monotone_type.none )
					continue;

				if( !key.is_first_key )
				{
					var key0 = key.prev_key;
					m_evaluator.create_hermite( key0.position, key0.key.right_tangent, key.key.left_tangent, key.position );

					var last_y	= key0.position_y;
					var max		= key.position_x - key0.position_x;
					var step	= max / 10;
					for( var j = 0.0; j < max; j += step )
					{
						var y = m_evaluator.evaluate_spline( j );
						if( monotone_type == float_curve_monotone_type.down )
						{
							if( y > last_y || y < key.position_y )
								return false;
						}
						else if( y < last_y || y > key.position_y )
							return false;

						last_y = y;
					}
				}
				if( !key.is_last_key )
				{
					var key1 = key.next_key;
					m_evaluator.create_hermite( key.position, key.key.right_tangent, key1.key.left_tangent, key1.position );

					var last_y	= key.position_y;
					var max		= key1.position_x - key.position_x;
					var step	= max / 10;
					for( var j = 0.0; j < max; j += step )
					{
						var y = m_evaluator.evaluate_spline( j );
						if( monotone_type == float_curve_monotone_type.down )
						{
							if( y > last_y || y < key1.position_y )
								return false;
						}
						else if( y < last_y || y > key1.position_y )
							return false;

						last_y = y;
					}
				}
			}

			return true;
		}
		private					void		move_keys		( Vector current_offset )		
		{
			var count = m_parent_panel.points_positions.Count;
			for ( var i = 0; i < count; ++i )
			{
				var new_position			= m_parent_panel.points_positions[i] + current_offset;

				if( m_parent_panel.fix_point_position != null )
					m_parent_panel.fix_point_position( ref new_position );

				m_parent_panel.selected_keys[i].move_to	( new_position );
			}
		}
		private					void		move_tangents	( Vector current_offset )		
		{
			var index = 0;
			foreach( var item in m_parent_panel.items.OfType<visual_curve>( ) )
			{
				var flag = false;
				foreach ( var tangent in item.selected_tangents )
				{
					tangent.move_to( m_parent_panel.points_positions[index++] + current_offset );
					flag = true;
				}
				if( flag && m_parent_panel.is_frequently_changed )
					m_parent_panel.on_float_curve_changed( item.float_curve );
			}
		}

		public override			Boolean		mouse_down		( MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Middle && Keyboard.PrimaryDevice.Modifiers == ModifierKeys.None )
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
		public override			Boolean		mouse_move		( MouseEventArgs e )			
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
			else if( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.tangents )
			{
				move_tangents( current_offset );

				if( curves_valid( ) )
					m_last_offset = current_offset;
				else
				{
					move_tangents( m_last_offset );
					return true;
				}
			}
			else
			{
				if( m_parent_panel.selected_keys.Count == 0 )
					return true;

				if( m_parent_panel.m_snap_to_grid_toggle.IsChecked == true )
				{
					var checked_position			= m_parent_panel.points_positions[m_parent_panel.points_positions.Count - 1];
					var visual_snap_delta			= new Vector( 5, 5 ).LengthSquared;

					var offsetted_position			= checked_position + current_offset;
					var offsetted_scaled_position	= new Point( offsetted_position.X / m_parent_panel.grid_x_scale_multiplier , offsetted_position.Y / m_parent_panel.grid_y_scale_multiplier );
					
					var step_x						= m_parent_panel.curves_panel.divide_factor_x;
					var step_y						= m_parent_panel.curves_panel.divide_factor_y;

					var x_index						= (Int32)( offsetted_scaled_position.X / step_x );
					var y_index						= (Int32)( offsetted_scaled_position.Y / step_y );

					if( offsetted_position.Y > 0 )
						++y_index;

					var visual_offsetted_position	= m_parent_panel.logical_to_visual_point( offsetted_position );

					var grid_point				= new Point( step_x * x_index * m_parent_panel.grid_x_scale_multiplier, step_y * y_index * m_parent_panel.grid_y_scale_multiplier );
					if( ( visual_offsetted_position - m_parent_panel.logical_to_visual_point( grid_point ) ).LengthSquared < visual_snap_delta )
						current_offset				= grid_point - checked_position;
					else
					{
						grid_point				= new Point( step_x * ( x_index + 1 ) * m_parent_panel.grid_x_scale_multiplier, step_y * ( y_index - 1 ) * m_parent_panel.grid_y_scale_multiplier );
						if( ( visual_offsetted_position - m_parent_panel.logical_to_visual_point( grid_point ) ).LengthSquared < visual_snap_delta )
							current_offset				= grid_point - checked_position;
						else
						{
							grid_point				= new Point( step_x * x_index * m_parent_panel.grid_x_scale_multiplier, step_y * ( y_index - 1 ) * m_parent_panel.grid_y_scale_multiplier );
							if( ( visual_offsetted_position - m_parent_panel.logical_to_visual_point( grid_point ) ).LengthSquared < visual_snap_delta )
								current_offset				= grid_point - checked_position;
							else
							{
								grid_point				= new Point( step_x * ( x_index + 1 ) * m_parent_panel.grid_x_scale_multiplier, step_y * y_index * m_parent_panel.grid_y_scale_multiplier );
								if( ( visual_offsetted_position - m_parent_panel.logical_to_visual_point( grid_point ) ).LengthSquared < visual_snap_delta )
									current_offset				= grid_point - checked_position;
							}
						}
					}
				}

				if( m_parent_panel.m_snap_to_point_toggle.IsChecked == true )
				{
					var checked_position	= m_parent_panel.points_positions[m_parent_panel.points_positions.Count - 1];
					var visual_snap_delta	= new Vector( 5, 5 ).LengthSquared;
					var offsetted_visual_checked_position = m_parent_panel.logical_to_visual_point( checked_position + current_offset );

					foreach( var key in m_parent_panel.items.OfType<visual_curve>( ).SelectMany( curve => curve.keys ).Where( key => !m_parent_panel.selected_keys.Contains( key ) ) )
					{
						if( ( offsetted_visual_checked_position - m_parent_panel.logical_to_visual_point( key.position ) ).LengthSquared < visual_snap_delta )
						{
							current_offset = key.position - checked_position;
							break;
						}
					}
				}

				if( m_parent_panel.selected_points_top_left.X + current_offset.X < m_parent_panel.field_left_limit )
					current_offset.X = m_parent_panel.field_left_limit - m_parent_panel.selected_points_top_left.X;

				if( m_parent_panel.selected_points_top_left.Y + current_offset.Y > m_parent_panel.field_top_limit )
					current_offset.Y = m_parent_panel.field_top_limit - m_parent_panel.selected_points_top_left.Y;

				if( m_parent_panel.selected_points_bottom_right.X + current_offset.X > m_parent_panel.field_right_limit )
					current_offset.X = m_parent_panel.field_right_limit - m_parent_panel.selected_points_bottom_right.X;

				if( m_parent_panel.selected_points_bottom_right.Y + current_offset.Y < m_parent_panel.field_bottom_limit )
					current_offset.Y = m_parent_panel.field_bottom_limit - m_parent_panel.selected_points_bottom_right.Y;

			
				var count = m_parent_panel.points_positions.Count;
				for( var i = 0; i < count; ++i )
				{
					var key = m_parent_panel.selected_keys[i];

					if( key.parent_curve.monotone_type == float_curve_monotone_type.none )
						continue;

					if( !key.is_first_key && !key.prev_key.is_selected && m_parent_panel.points_positions[i].X + current_offset.X < key.prev_key.position_x )
						current_offset.X = key.prev_key.position_x - m_parent_panel.points_positions[i].X;

					if( !key.is_last_key && !key.next_key.is_selected && m_parent_panel.points_positions[i].X + current_offset.X > key.next_key.position_x )
						current_offset.X = key.next_key.position_x - m_parent_panel.points_positions[i].X;
				}

				move_keys( current_offset );
				if( curves_valid( ) )
					m_last_offset = current_offset;
				else
				{
					move_keys( m_last_offset );
					return true;
				}
				
				if( m_parent_panel.is_frequently_changed )
					foreach( var visual_curve in m_parent_panel.items.OfType<visual_curve>( ).Where( visual_curve => visual_curve.selected_keys.Count != 0 ) )
						m_parent_panel.on_float_curve_changed( visual_curve.float_curve );
				
				m_parent_panel.update_scroll_bar( );
			}
			m_parent_panel.update_selected_items_position_viewer( );

			m_last_offset = current_offset;

			return true;
		}
		public override 		Boolean		mouse_up		( MouseButtonEventArgs e )		
		{
			if( !m_is_in_action )
				return false;

			m_is_in_action = false;

			if( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.key_range_controls )
				foreach ( var curve in m_parent_panel.items.OfType<visual_curve>( ).Where(curve => curve.selected_key_range_controls.Count > 0 ) )
					m_parent_panel.on_float_curve_edit_complete( curve.float_curve );

			else if( m_parent_panel.selected_entity == curve_editor_panel.selected_entity_type.tangents )
				foreach ( var curve in m_parent_panel.items.OfType<visual_curve>( ).Where(curve => curve.selected_tangents.Count > 0 ) )
					m_parent_panel.on_float_curve_edit_complete( curve.float_curve );

			else
				foreach ( var curve in m_parent_panel.items.OfType<visual_curve>( ).Where(curve => curve.selected_keys.Count > 0 ) )
					m_parent_panel.on_float_curve_edit_complete( curve.float_curve );

			m_parent_panel.curves_panel.Cursor					= null;
			m_parent_panel.curves_panel.ReleaseMouseCapture		( );

			return true;
		}
	}
}
