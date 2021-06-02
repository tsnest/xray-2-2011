////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media.Media3D;
using xray.editor.wpf_controls.curve_editor;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for animation_timeline_control.xaml
	/// </summary>
	public partial class animation_curve_editor
	{
		#region | Initialize |


		public			animation_curve_editor					( )			
		{
			InitializeComponent( );

			IsEnabled = false;

			m_time_ruler_seconds.value_offset	= delegate{ return (Single)m_curves_panel.pan.X; };
			m_time_ruler_seconds.layout_scale	= delegate{ return (Single)m_curves_panel.scale.X; };
			m_time_ruler_seconds.layout_width	= delegate{ return (Single)m_curves_panel.ActualWidth; };

			m_time_ruler_frames.value_offset	= delegate{ return (Single)m_curves_panel.pan.X; };
			m_time_ruler_frames.layout_scale	= delegate{ return (Single)m_curves_panel.scale.X / m_fps; };
			m_time_ruler_frames.layout_width	= delegate{ return (Single)m_curves_panel.ActualWidth; };

			m_curves_panel.fix_point_position	= fix_curve_point_position;
			m_curves_panel.double_click_key_adding_enabled = false;

			m_settings_menu						= (ContextMenu)FindResource("settings_context_menu");
			m_fps_text_box						= (TextBox)LogicalTreeHelper.FindLogicalNode( m_settings_menu, "m_fps_text_box" );
			m_fps_text_box.Text					= m_fps.ToString( );

			m_curves_panel.selection_changed	+= delegate
			{
				if( selection_changed != null )
					selection_changed( selected_key_index );
			};
			m_curves_panel.float_curve_changed	+= delegate
			{
				if( time_changed != null )
				    time_changed( );
			};
		}


		#endregion

		#region |   Fields   |


		private			Double			m_animation_time;
		private			Boolean			m_is_test_playing;
		private			Int32			m_fps = 30;
		private			ContextMenu		m_settings_menu;
		private			Boolean			m_snap_to_frames;
		private			TextBox			m_fps_text_box;


		#endregion

		#region | Properties |


		public			Double		animation_time						
		{
			get
			{
				return m_animation_time;
			}
			set
			{
				m_animation_time = value;
				m_time_thumb.SetValue	( Canvas.LeftProperty, m_curves_panel.pan.X + m_animation_time * m_curves_panel.scale.X - m_time_thumb.ActualWidth / 2 );
				if( time_changed != null )
				    time_changed( );
			}
		}
		public			Int32		selected_key_index					
		{
			get
			{
				return m_curves_panel.selected_keys.Count > 0 ? m_curves_panel.selected_keys[0].index : 0;
			}
			set
			{
				m_curves_panel.select_keys_by_index( value );
			}
		}
		public			Boolean		is_camera_view						
		{
			get;set;
		}


		#endregion

		#region |   Events   |


		public			transform	get_camera_transform;

		public event	Action			time_changed;
		public event	Action			selected_to_camera_clicked;
		public event	Action			camera_to_selected_clicked;
		public event	Action<Int32>	selection_changed;


		#endregion

		#region |  Methods   |


		private			void				play_click							( Object sender, RoutedEventArgs e )			
		{
			m_is_test_playing	= true;

			m_focus_fake.Focus( );
		}
		private			void				stop_click							( Object sender, RoutedEventArgs e )			
		{
			m_is_test_playing	= false;

			m_focus_fake.Focus( );
		}
		private			void				set_key								( Object sender, RoutedEventArgs e )			
		{
			m_curves_panel.deselect_all_keys( );
			var	position	= new Vector3D( );
			var	rotation	= new Vector3D( );
			var	scale		= new Vector3D( );

			get_camera_transform( ref position, ref rotation, ref scale );

			Int32 key_index;
			var curve		= m_curves_panel.items.OfType<visual_curve>( ).First( );
			var key			= curve.find_key_by_x( m_animation_time );

			if( key == null )
				key = curve.add_key( m_animation_time );

			key_index		= key.index;

			m_curves_panel.panel_curves["position x"].keys[key_index].position_y = position.X;
			m_curves_panel.panel_curves["position y"].keys[key_index].position_y = position.Y;
			m_curves_panel.panel_curves["position z"].keys[key_index].position_y = position.Z;

			if( key_index == 0 )
			{
				m_curves_panel.panel_curves["position x"].keys[key_index].set_identity_right_tangent( );
				m_curves_panel.panel_curves["position y"].keys[key_index].set_identity_right_tangent( );
				m_curves_panel.panel_curves["position z"].keys[key_index].set_identity_right_tangent( );
			}

			if( key.parent_curve.keys.Count == 1 )
			{
				m_curves_panel.panel_curves["rotation x"].keys[key_index].position_y = rotation.X / Math.PI * 180;
				m_curves_panel.panel_curves["rotation y"].keys[key_index].position_y = rotation.Y / Math.PI * 180;
				m_curves_panel.panel_curves["rotation z"].keys[key_index].position_y = rotation.Z / Math.PI * 180;
			}
			else
			{
				if( key_index == 0 )
				{
					m_curves_panel.panel_curves["rotation x"].keys[key_index].position_y = get_fixed_rotation( rotation.X / Math.PI * 180, m_curves_panel.panel_curves["rotation x"].keys[key_index+1].position_y );
					m_curves_panel.panel_curves["rotation y"].keys[key_index].position_y = get_fixed_rotation( rotation.Y / Math.PI * 180, m_curves_panel.panel_curves["rotation y"].keys[key_index+1].position_y );
					m_curves_panel.panel_curves["rotation z"].keys[key_index].position_y = get_fixed_rotation( rotation.Z / Math.PI * 180, m_curves_panel.panel_curves["rotation z"].keys[key_index+1].position_y );
					m_curves_panel.panel_curves["rotation x"].keys[key_index].set_identity_right_tangent( );
					m_curves_panel.panel_curves["rotation y"].keys[key_index].set_identity_right_tangent( );
					m_curves_panel.panel_curves["rotation z"].keys[key_index].set_identity_right_tangent( );
				}
				else
				{
					m_curves_panel.panel_curves["rotation x"].keys[key_index].position_y = get_fixed_rotation( rotation.X / Math.PI * 180, m_curves_panel.panel_curves["rotation x"].keys[key_index-1].position_y );
					m_curves_panel.panel_curves["rotation y"].keys[key_index].position_y = get_fixed_rotation( rotation.Y / Math.PI * 180, m_curves_panel.panel_curves["rotation y"].keys[key_index-1].position_y );
					m_curves_panel.panel_curves["rotation z"].keys[key_index].position_y = get_fixed_rotation( rotation.Z / Math.PI * 180, m_curves_panel.panel_curves["rotation z"].keys[key_index-1].position_y );
				}
			}

			m_curves_panel.update_curves		( );
			curve.raise_key_changed				( key_index );

			if( key_index == 0 && !key.is_last_key )
				curve.raise_key_changed				( key_index + 1 );

			if( key_index == curve.keys.Count - 1 && !key.is_first_key )
				curve.raise_key_changed				( key_index - 1 );

			m_focus_fake.Focus( );
		}
		private			void				selected_to_camera_click			( Object sender, RoutedEventArgs e )			
		{
			if( selected_to_camera_clicked != null )
				selected_to_camera_clicked( );

			m_focus_fake.Focus( );
		}
		private			void				camera_to_selected_click			( Object sender, RoutedEventArgs e )			
		{
			if( camera_to_selected_clicked != null )
				camera_to_selected_clicked( );

			m_focus_fake.Focus( );
		}
		private			void				prev_key_click						( Object sender, RoutedEventArgs e )			
		{
			var		closest_prev_key_position	= Double.MinValue;
			var		closest_prev_key_list		= new List<visual_curve_key>( );
			foreach( var key in from visual_curve in m_curves_panel.items.OfType<visual_curve>( )
			                    from key in visual_curve.keys
			                    where key.position_x < m_animation_time
			                    select key )
			{
				if( closest_prev_key_list.Count == 0 )
				{
					closest_prev_key_list.Add	( key );
					closest_prev_key_position	= key.position_x;
				}
				else if( key.position_x == closest_prev_key_position )
				{
					closest_prev_key_list.Add	( key );
				}
				else if( key.position_x > closest_prev_key_position )
				{
					closest_prev_key_list.Clear	( );
					closest_prev_key_list.Add	( key );
					closest_prev_key_position	= key.position_x;
				}
			}

			if( closest_prev_key_list.Count > 0 )
			{
				if( m_select_keys_toggle.IsChecked == true )
				{
					m_curves_panel.deselect_all_keys( );

					foreach( var key in closest_prev_key_list )
						key.is_selected = true;

					if( selection_changed != null )
						selection_changed( closest_prev_key_list[0].index );
				}

				animation_time = closest_prev_key_position;
			}

			m_focus_fake.Focus( );
		}
		private			void				next_key_click						( Object sender, RoutedEventArgs e )			
		{
			var		closest_next_key_position	= Double.MaxValue;
			var		closest_next_key_list		= new List<visual_curve_key>( );
			foreach( var key in from visual_curve in m_curves_panel.items.OfType<visual_curve>( )
			                    from key in visual_curve.keys
			                    where key.position_x > m_animation_time
			                    select key )
			{
				if( closest_next_key_list.Count == 0 )
				{
					closest_next_key_list.Add	( key );
					closest_next_key_position	= key.position_x;
				}
				else if( key.position_x == closest_next_key_position )
				{
					closest_next_key_list.Add	( key );
				}
				else if( key.position_x < closest_next_key_position )
				{
					closest_next_key_list.Clear	( );
					closest_next_key_list.Add	( key );
					closest_next_key_position	= key.position_x;
				}
			}

			if( closest_next_key_list.Count > 0 )
			{
				if( m_select_keys_toggle.IsChecked == true )
				{
					m_curves_panel.deselect_all_keys( );

					foreach( var key in closest_next_key_list )
						key.is_selected = true;

					if( selection_changed != null )
						selection_changed( closest_next_key_list[0].index );
				}

				animation_time = closest_next_key_position;
			}

			m_focus_fake.Focus( );
		}
		private			void				camera_view_click					( Object sender, RoutedEventArgs e )			
		{
			is_camera_view = m_camera_view_button.IsChecked == true;
			m_focus_fake.Focus( );
		}
		private			void				settings_click						( Object sender, RoutedEventArgs e )			
		{
			m_settings_menu.StaysOpen	= false;
			m_settings_menu.IsOpen		= true;
		}
		private			void				snap_to_frames_checked				( Object sender, RoutedEventArgs e )			
		{
			m_snap_to_frames = true;
			m_focus_fake.Focus( );
		}
		private			void				snap_to_frames_unchecked			( Object sender, RoutedEventArgs e )			
		{
			m_snap_to_frames = false;
			m_focus_fake.Focus( );
		}
		private			void				fps_changed							( Object sender, TextChangedEventArgs e )		
		{
			Int32 new_fps; 
			if( Int32.TryParse( m_fps_text_box.Text, out new_fps ) )
			{
				if( new_fps == 0 )
					new_fps = 2;
				m_fps = new_fps;
				m_time_ruler_frames.InvalidateVisual( );
			}
		}

		private			void				curves_panel_scale_changed			( )												
		{
			m_time_ruler_seconds.InvalidateVisual	( );
			m_time_ruler_frames.InvalidateVisual	( );
			fix_thumb_location						( );
		}
		private			void				curves_panel_pan_changed			( )												
		{
			m_time_ruler_seconds.InvalidateVisual	( );
			m_time_ruler_frames.InvalidateVisual	( );
			fix_thumb_location						( );
		}
		private			void				loaded								( Object sender, RoutedEventArgs e )			
		{
			m_time_ruler_seconds.InvalidateVisual	( );
			m_time_ruler_frames.InvalidateVisual	( );
			fix_thumb_location						( );
			m_curves_panel.focus_curves				( );
		}
		private			void				timeline_mouse_down					( Object sender, MouseButtonEventArgs e )		
		{
			var new_time			= ( e.GetPosition( m_time_ruler_frames ).X - m_curves_panel.pan.X ) / m_curves_panel.scale.X;
			fix_animation_time		( ref new_time );
			animation_time			= new_time;
		}
		private			void				thumb_drag_delta					( Object sender, DragDeltaEventArgs e )			
		{
			var change				= e.HorizontalChange;
			var new_time			= Math.Max( 0 , m_animation_time + change / m_curves_panel.scale.X );
			fix_animation_time		( ref new_time );
			animation_time			= new_time;
		}
		private			void				curves_panel_float_curve_changed	( float_curve obj )								
		{
			obj.fire_curve_changed( );
		}
		private			void				speed_text_box_key_down				( Object sender, KeyEventArgs e )				
		{
			if( e.Key == Key.Enter )
				MoveFocus( new TraversalRequest( FocusNavigationDirection.Next ) );
		}

		private			Double				get_fixed_rotation					( Double new_rotation, Double prev_rotation )	
		{
			if( new_rotation < prev_rotation )
			{
				var delta		= prev_rotation - new_rotation;
				var new_delta	= Math.Abs( prev_rotation - ( new_rotation + 360 ) );
				while( new_delta < delta )
				{
					delta			= new_delta;
					new_rotation	+= 360;
				}
			}
			else
			{
				var delta		= new_rotation - prev_rotation;
				var new_delta	= Math.Abs( ( new_rotation - 360 ) - prev_rotation );
				while( new_delta < delta )
				{
					delta			= new_delta;
					new_rotation	-= 360;
				}
			}
			
			return new_rotation;
		}
		private			void				fix_thumb_location					( )												
		{
			animation_time = m_animation_time;
		}
		private			void				fix_animation_time					( ref Double time )								
		{
			if( m_snap_to_frames )
			{
				time		= Math.Round( time * m_fps ) / m_fps;
			}
		}
		private			void				fix_curve_point_position			( ref Point point_position )					
		{
			var x				= point_position.X;
			fix_animation_time	( ref x );
			point_position.X	= x;
		}

		internal		void				deselect_all						( )												
		{
			m_curves_panel.deselect_all_groups( );
		}

		public			panel_curve_group	add_group							( String name )									
		{
			IsEnabled = true;
			return m_curves_panel.add_group( name );
		}
		public			void				clear								( )												
		{
			m_curves_panel.clear	( );
			IsEnabled				= false;
			is_camera_view			= false;
		}
		public			void				reset								( )												
		{
			clear							( );

			time_changed					= null;
			selected_to_camera_clicked		= null; 
			camera_to_selected_clicked		= null;
			selection_changed				= null;

			animation_time = 0;
		}
		public			Object				get_viewed_object					( )												
		{
			if( time_changed != null )
				return time_changed.Target;
            return null;    
		}
		public			void				tick								( Single elapsed_time )							
		{
			if( m_is_test_playing )
			{
				animation_time += elapsed_time * m_speed_slider.Value;
			}
			if( m_curves_panel.edited_curves.Count > 0 && m_is_test_playing )
			{
				var last_time = m_curves_panel.edited_curves.Select( curve => curve.keys.Last( ) ).Max( key => key.position_x );
				if( animation_time > last_time )
				{
					animation_time = last_time;
					m_is_test_playing = false;
				}
			}
		}
		public			void				update_curves						( )												
		{
			m_curves_panel.update_curves( );
		}
		public			void				raise_curves_change					( )												
		{
			m_curves_panel.raise_curves_change( );
		}
		public			void				focus_on_curves						( )												
		{
			m_curves_panel.focus_curves( );
		}
		public			void				update_chacked_key_type_buttons		( )												
		{
			m_curves_panel.update_chacked_key_type_buttons( );
		}

		public delegate void				transform							( ref Vector3D position, ref Vector3D rotation, ref Vector3D scale );


		#endregion

	}
}
