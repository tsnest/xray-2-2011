////////////////////////////////////////////////////////////////////////////
//	Created		: 06.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using System.ComponentModel;
using xray.editor.wpf_controls.curve_editor.templates;
using xray.editor.wpf_controls.curve_editor.tools;
using DPChangedEventArgs = System.Windows.DependencyPropertyChangedEventArgs;
using xray.editor.wpf_controls.type_editors.curve_editor.templates;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for visual_curve_panel.xaml
	/// </summary>
	
	public partial class curve_editor_panel: INotifyPropertyChanged
	{

		#region | Initialize |


		public curve_editor_panel	( )	
		{
			InitializeComponent( );
			m_curves_panel.parent_panel = this;

			m_just_selected_keys		= new HashSet<visual_curve_key>( ); 
			m_just_selected_tangents	= new HashSet<visual_curve_key_tangent>( ); 
			m_just_selected_key_range_controls = new HashSet<visual_curve_key_range_control>( );
			m_selection_rect			= new RectangleGeometry( );
			m_selection_path			= new Path
			{
				Stroke						= Brushes.Black,
				StrokeDashArray 			= new DoubleCollection { 3, 2 },
				StrokeThickness 			= 1,
				Data						= m_selection_rect,
				SnapsToDevicePixels			= true
			};
			SnapsToDevicePixels			= true;
			pan							= new Point( 0, 0 );
			scale						= new Point( 100, 100 );
			ClipToBounds				= true;
			is_default_range_enabled	= true;
			is_frequently_changed		= false;
			double_click_key_adding_enabled = true;
			focus_sample_count			= 10;
			
			field_left_limit			= Double.NaN;
			field_right_limit			= Double.NaN;
			field_top_limit				= Double.NaN;
			field_bottom_limit			= Double.NaN;

			m_panel_context_menu		= (ContextMenu)FindResource( "key_context_menu" );

			m_move_tool					= new move_tool			( this ); 
			m_scale_tool				= new scale_tool		( this );
			m_move_range_tool			= new move_range_tool	( this );
			m_add_tool					= new add_tool			( this );
			m_remove_tool				= new remove_tool		( this );
			m_active_tool				= m_move_tool;

			custom_templates			= new List<float_curve>( );
		}
		
		
		#endregion

		#region |   Fields   |


		private readonly static Pen								m_stroke_pen = new Pen( new SolidColorBrush( Colors.Black ), 0 );

		private readonly	Dictionary<String, float_curve>		m_panel_curves = new Dictionary<String, float_curve>( );
		private				float_curve_group					m_edited_group;
		private				Point								m_pan;
		private				Point								m_scale;

		private readonly	List<visual_curve_key>				m_selected_keys = new List<visual_curve_key>( );
		private readonly	List<visual_curve_key_tangent>		m_selected_key_tangents = new List<visual_curve_key_tangent>( );
		private readonly	List<visual_curve_key_range_control>m_selected_key_range_controls = new List<visual_curve_key_range_control>( );
		private readonly	HashSet<visual_curve_key>			m_just_selected_keys;
		private readonly	HashSet<visual_curve_key_tangent>	m_just_selected_tangents;
		private readonly	HashSet<visual_curve_key_range_control>	m_just_selected_key_range_controls;

		private readonly	List<Point>							m_points_positions = new List<Point>( );
		private				Point								m_scaling_start_position;
		private				Point								m_start_scale;
		private				Point								m_selection_start_position;
		private				Point								m_selected_points_top_left;
		private				Point								m_selected_points_bottom_right;
		private				Boolean								m_is_dotted_selection;

		private				Boolean								m_is_tool_captured;
		private				Boolean								m_is_selection_started;
		private				Boolean								m_is_field_move_captured;
		private				Boolean								m_is_field_scale_captured;
		private				Boolean								m_is_scroll_bar_updating;

		private				Double								m_field_left_limit;
		private				Double								m_field_right_limit;
		private				Double								m_field_top_limit;
		private				Double								m_field_bottom_limit;

		private				Point								m_mouse_position;
		private				Boolean?							m_is_zoom_locked;
		private				selected_entity_type				m_selected_entity;

		private				tool_base							m_active_tool;
		private	readonly 	move_tool							m_move_tool;
		private	readonly 	move_range_tool						m_move_range_tool;
		private	readonly	scale_tool							m_scale_tool;
		private readonly	add_tool							m_add_tool;
		private	readonly 	remove_tool							m_remove_tool;

		private readonly	ContextMenu							m_panel_context_menu;

		private readonly	Path								m_selection_path;
		private readonly	RectangleGeometry					m_selection_rect;

		public static		DependencyProperty					is_toolbar_visible_property = DependencyProperty.Register( "is_toolbar_visible", typeof( Boolean ), typeof( curve_editor_panel ), new FrameworkPropertyMetadata( true ) );
		public static		DependencyProperty					is_hierarchy_visible_property = DependencyProperty.Register( "is_hierarchy_visible", typeof( Boolean ), typeof( curve_editor_panel ), new FrameworkPropertyMetadata( true ) );
		public static		DependencyProperty					decorator_property = DependencyProperty.Register( "decorator", typeof( UIElement ), typeof( curve_editor_panel ), new FrameworkPropertyMetadata( null, decorator_changed ) );


		#endregion

		#region | Properties |


		internal	List<visual_curve_key_tangent>			selected_key_tangents			
		{
			get
			{
				return m_selected_key_tangents;
			}
		}
		internal	List<visual_curve_key_range_control>	selected_key_range_controls		
		{
			get
			{
				return m_selected_key_range_controls;
			}
		}

		internal	curves_panel			curves_panel					
		{
			get
			{
				return m_curves_panel;
			}
		}
		internal	selected_entity_type	selected_entity					
		{
			get
			{
				return m_selected_entity;
			}
			set
			{
				m_selected_entity = value;
			}
		}
		internal	Point					selected_points_top_left		
		{
			get
			{
				return m_selected_points_top_left;
			}
		}
		internal	Point					selected_points_bottom_right	
		{
			get
			{
				return m_selected_points_bottom_right;
			}
		}
		internal	List<Point>				points_positions				
		{
			get
			{
				return m_points_positions;
			}
		}

		public		float_curve				edited_curve					
		{
			set
			{
				edited_curves = new List<float_curve>{ value };
			}
		}
		public		List<float_curve>		edited_curves					
		{
			get
			{
				return m_panel_curves.Values.ToList( );
			}
			set
			{
				clear( );
				if( value != null )
				{
					var root_group = add_group( "Curves" );
					foreach ( var curve in value )
						root_group.add_curve( curve.name, curve.name, curve );

					for( var i = 0; i < items.Count; i++ )
					{
						var item = items[i];
						if( item is visual_curve )
							( (visual_curve)item ).update( );
					}
				}
				InvalidateVisual( );
			}
		}
		public		float_curve_group		edited_group					
		{
			get
			{
				return m_edited_group;
			}
			set
			{
				clear( );
				if( value != null )
					add_group( value );

				m_edited_group = value;

				InvalidateVisual( );
			}
		}

		public		UIElementCollection		items							
		{
			get
			{
				return m_curves_panel.Children;
			}
		}
		public		Boolean					has_selected_keys				
		{
			get
			{
				return items.OfType<visual_curve>( ).Aggregate( false, ( current, curv ) => current | curv.has_selected_keys );
			}
		}

		public		List<visual_curve_key>	selected_keys					
		{
			get{ return m_selected_keys; }
		}
		public		Point					pan								
		{
			get 
			{
				return m_pan; 
			}
			set 
			{ 
				m_pan = value;
				if( PropertyChanged != null )
					PropertyChanged( this, new PropertyChangedEventArgs( "field_clip_rect_pos" ) );
			}
		}
		public		Point					scale							
		{
			get 
			{
				return m_scale; 
			}
			private set 
			{
				m_scale = value; 
			}
		}
		public		Boolean					is_default_range_enabled		
		{
			get;set;
		}

		public		Double					field_left_limit				
		{
			get 
			{
				return m_field_left_limit; 
			}
			set 
			{
				m_field_left_limit					= value;
				fix_keys_positions_to_limits		( );
				m_curves_panel.InvalidateVisual		( ); 
			}
		}
		public		Double					field_right_limit				
		{
			get 
			{
				return m_field_right_limit; 
			}
			set 
			{
				m_field_right_limit					= value;
				fix_keys_positions_to_limits		( );
				m_curves_panel.InvalidateVisual		( );  
			}
		}
		public		Double					field_top_limit					
		{
			get 
			{
				return m_field_top_limit; 
			}
			set 
			{ 
				m_field_top_limit					= value;
				fix_keys_positions_to_limits		( );
				m_curves_panel.InvalidateVisual		( ); 
			}
		}
		public		Double					field_bottom_limit				
		{
			get 
			{
				return m_field_bottom_limit; 
			}
			set 
			{
				m_field_bottom_limit				= value;
				fix_keys_positions_to_limits		( );
				m_curves_panel.InvalidateVisual		( ); 
			}
		}

		public		Dictionary<String, float_curve>	panel_curves			
		{
			get
			{
				return m_panel_curves;
			}
		}
		public		Boolean					is_zoom_locked					
		{
			get
			{
				return m_is_zoom_locked == null ? settings.curve_editor_lock_zoom : m_is_zoom_locked.Value;
			}
			set
			{
				m_is_zoom_locked = value;
			}
		}
		public		Boolean					is_read_only					
		{
			get;
			set;
		}
		public		Boolean					is_frequently_changed			
		{
			get;
			set;
		}
		public		Visibility				focus_default_range_visibility	
		{
			get
			{
				return m_focus_range_button.Visibility;
			}
			set
			{
				m_focus_range_button.Visibility = value;
			}
		}
		public		Visibility				toggle_range_visibility			
		{
			get
			{
				return m_toggle_range_button.Visibility;
			}
			set
			{
				m_toggle_range_button.Visibility = value;
			}
		}
		public		Visibility				scroll_bar_visibility			
		{
			get
			{
				return m_curves_scroll_bar.Visibility;
			}
			set
			{
				m_curves_scroll_bar.Visibility = value;
			}
		}
		public		Boolean					is_toolbar_visible				
		{
			get
			{
				 return (Boolean)GetValue( is_toolbar_visible_property );
			}
			set
			{
				SetValue( is_toolbar_visible_property, value );
			}
		}
		public		Boolean					is_hierarchy_visible			
		{
			get
			{
				return (Boolean)GetValue( is_hierarchy_visible_property );
			}
			set
			{
				SetValue( is_hierarchy_visible_property, value );
			}
		}
		public		Boolean					is_horizontal_numbers_visible	
		{
			get
			{
				return m_curves_panel.is_horizontal_numbers_visible;
			}
			set
			{
				m_curves_panel.is_horizontal_numbers_visible = value;
			}
		}
		public		Boolean					is_vertical_numbers_visible		
		{
			get
			{
				return m_curves_panel.is_vertical_numbers_visible;
			}
			set
			{
				m_curves_panel.is_vertical_numbers_visible = value;
			}
		}
		
		public		Double					divide_factor_x_min				
		{
			get
			{
				return m_curves_panel.divide_factor_x_min;
			}
			set
			{
				m_curves_panel.divide_factor_x_min = value;
			}
		}
		public		Double					divide_factor_y_min				
		{
			get
			{
				return m_curves_panel.divide_factor_y_min;
			}
			set
			{
				m_curves_panel.divide_factor_y_min = value;
			}
		}

		public		Double					grid_x_scale_multiplier			
		{
			get
			{
				return m_curves_panel.grid_x_scale_multiplier;
			}
			set
			{
				m_curves_panel.grid_x_scale_multiplier = value;
			}
		}
		public		Double					grid_y_scale_multiplier			
		{
			get
			{
				return m_curves_panel.grid_y_scale_multiplier;
			}
			set
			{
				m_curves_panel.grid_y_scale_multiplier = value;
			}
		}

		public		Boolean					double_click_key_adding_enabled	
		{
			get;
			set;
		}
		public		Int32					focus_sample_count				
		{
			get;set;
		}

		public		UIElement				decorator						
		{
			get
			{
				return (UIElement)GetValue( decorator_property );
			}
			set
			{
				SetValue( decorator_property, value );
			}
		}
		public		List<float_curve>		custom_templates				
		{
			get;
			private set;
		}


		#endregion

		#region |   Events   |

		
		public event		Action<float_curve>				float_curve_changed;
		public event		Action<float_curve>				float_curve_edit_complete;

		public event		PropertyChangedEventHandler		PropertyChanged;
		public event		Action							scale_changed;
		public event		Action							pan_changed;
		public event		Action							selection_changed;

		public delegate		void							ref_point_delegate	( ref Point point_position );
		public				ref_point_delegate				fix_point_position;


		#endregion

		#region |  Methods   |


		private static	void					decorator_changed				( DependencyObject d, DPChangedEventArgs e )	
		{
			var editor = (curve_editor_panel)d;

			var decorator = (FrameworkElement)e.NewValue;
			( (Panel)decorator.Parent ).Children.Remove( decorator );

			editor.m_decorator_panel.Content = decorator;
		}
		private			void					handle_input					( Object sender, InputCommandEventArgs e )		
		{
			if( e.command_string == "F" || e.command_string == "Del" )
				e.Handled = true;
		}
		private			void					tool_changed					( Object sender, RoutedEventArgs e )			
		{
			if( e.Source == m_move_tool_button )
				m_active_tool = m_move_tool;

			else if( e.Source == m_scale_tool_button )
				m_active_tool = m_scale_tool;

			else if( e.Source == m_move_range_tool_button )
				m_active_tool = m_move_range_tool;

			else if( e.Source == m_add_tool_button )
				m_active_tool = m_add_tool;

			else if( e.Source == m_remove_tool_button )
				m_active_tool = m_remove_tool;
		}
		private			void					scroll_bar_size_changed			( Object sender, SizeChangedEventArgs e )		
		{
			update_scroll_bar( );
		}

		private			void					open_remove_menu				( List<visual_curve_key> visual_keys )			
		{
			if ( visual_keys.Count <= 0 )
				return;

			m_panel_context_menu.IsOpen		= true;
		}
		private			void					remove_menu_item_click			( Object sender, RoutedEventArgs e )			
		{
			var changed_curves = items.OfType<visual_curve>( ).Where( visual_curve => visual_curve.selected_keys.Count != 0 ).ToArray( );

			foreach( var key in m_selected_keys.ToArray( ) )
				key.remove( );

			foreach( var visual_curve in changed_curves )
				on_float_curve_changed( visual_curve.float_curve );

			deselect_all_keys( );
			hide_all_tangents( );
		}

		private			void					key_x_position_text_box_key_down( Object sender, KeyEventArgs e )				
		{
			if( e.Key == Key.Escape )
			{
				m_keys_x_position_text_box.Text = (String)m_keys_x_position_text_box.Tag;
				m_keys_x_position_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
			}

			if( e.Key == Key.Enter )
			{
				Single value;
				if( !Single.TryParse( m_keys_x_position_text_box.Text, out value ) )
					return;

				var count			= m_selected_keys.Count;
				if( count > 0 )
				{
					foreach( var key in m_selected_keys )
						key.position_x = value;
				}
				m_keys_x_position_text_box.Tag = m_keys_x_position_text_box.Text;
				m_keys_x_position_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
			}
		}
		private			void					key_y_position_text_box_key_down( Object sender, KeyEventArgs e )				
		{
			if( e.Key == Key.Escape )
			{
				m_keys_y_position_text_box.Text = (String)m_keys_y_position_text_box.Tag;
				m_keys_y_position_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
			}

			if( e.Key == Key.Enter )
			{
				Single value;
				if( !Single.TryParse( m_keys_y_position_text_box.Text, out value ) )
					return;

				if( m_selected_entity == selected_entity_type.keys )
				{
					var count			= m_selected_keys.Count;
					if( count > 0 )
					{
						foreach( var key in m_selected_keys )
							key.position_y = value;
					}
				}
				else if( m_selected_entity == selected_entity_type.key_range_controls )
				{
					var count			= m_selected_key_range_controls.Count;
					if( count > 0 )
					{
						foreach( var range_control in m_selected_key_range_controls )
							range_control.range_delta = value;
					}
				}
				m_keys_y_position_text_box.Tag = m_keys_y_position_text_box.Text;
				m_keys_y_position_text_box.MoveFocus( new TraversalRequest( FocusNavigationDirection.Previous ) );
			}
		}
		private			void					key_x_position_text_box_focus_lost( Object sender, RoutedEventArgs e )			
		{
			Single value;
			if( !Single.TryParse( m_keys_x_position_text_box.Text, out value ) )
			{
				m_keys_x_position_text_box.Text = (String)m_keys_x_position_text_box.Tag;
				return;
			}

			var count			= m_selected_keys.Count;
			if( count > 0 )
			{
				foreach( var key in m_selected_keys )
					key.position_x = value;
			}
			m_keys_x_position_text_box.Tag = m_keys_x_position_text_box.Text;
		}
		private			void					key_y_position_text_box_focus_lost( Object sender, RoutedEventArgs e )			
		{
			Single value;
			if( !Single.TryParse( m_keys_y_position_text_box.Text, out value ) )
			{
				m_keys_y_position_text_box.Text = (String)m_keys_y_position_text_box.Tag;
				return;
			}

			var count			= m_selected_keys.Count;
			if( count > 0 )
			{
				foreach( var key in m_selected_keys )
					key.position_y = value;
			}
			m_keys_y_position_text_box.Tag = m_keys_y_position_text_box.Text;
		}
		private			void					toggle_range_button_click		( Object sender, RoutedEventArgs e )			
		{
			var curves				= new HashSet<visual_curve>( );
			var is_range_enabled	= m_toggle_range_button.IsChecked == true;

			foreach( var key in m_selected_keys )
			{
				curves.Add( key.parent_curve );
				key.range_control_visibility = ( is_range_enabled ) ? Visibility.Visible : Visibility.Collapsed;
			}

			foreach( var curve in curves )
				curve.is_range = is_range_enabled;
		}
		private			void					linked_auto_click				( Object sender, RoutedEventArgs e )			
		{
			selected_entity_set_type	( float_curve_key_type.locked_auto );
			toggle_key_type_buttons		( m_linked_auto_button );
		}
		private			void					linear_click					( Object sender, RoutedEventArgs e )			
		{
			selected_entity_set_type	( float_curve_key_type.linear );
			toggle_key_type_buttons		( m_linear_button );
		}	
		private			void					step_click						( Object sender, RoutedEventArgs e )			
		{
			selected_entity_set_type	( float_curve_key_type.step );
			toggle_key_type_buttons		( m_step_button );
		}
		private			void					linked_manual_click				( Object sender, RoutedEventArgs e )			
		{
			selected_entity_set_type	( float_curve_key_type.locked_manual );
			toggle_key_type_buttons		( m_linked_manual_button );
		}
		private			void					breaked_click					( Object sender, RoutedEventArgs e )			
		{
			selected_entity_set_type	( float_curve_key_type.breaked );
			toggle_key_type_buttons		( m_breaked_button );
		}
		private			void					focus_click						( Object sender, RoutedEventArgs e )			
		{
			if( m_selected_keys.Count > 0 )
				focus_selection( );
			else
				focus_curves( );
		}
		private			void					focus_default_range_click		( Object sender, RoutedEventArgs e )			
		{
			focus_default_range( );
		}

		private			void					scroll_bar_value_changed		( Object sender, RoutedPropertyChangedEventArgs<Double> e )
		{
			if( m_is_scroll_bar_updating )
				return;

			pan = new Point( -e.NewValue, pan.Y );

			foreach( var curve in items.OfType<visual_curve>( ) )
					curve.update( );

			if( pan_changed != null )
					pan_changed( );

			m_curves_panel.InvalidateVisual( );
		}
		
		private			void					fix_keys_positions_to_limits	( )												
		{
			var affected_curves = new HashSet<visual_curve>( );
			foreach( var visual_curve in items.OfType<visual_curve>( ) )
			{
				foreach( var key in visual_curve.keys )
				{
					var key_position	= key.position;
					var is_changed		= false;

					if( key_position.X > m_field_right_limit )		{ key_position.X = m_field_right_limit; is_changed = true; }
					if( key_position.X < m_field_left_limit )		{ key_position.X = m_field_left_limit; is_changed = true; }
					if( key_position.Y > m_field_top_limit )		{ key_position.Y = m_field_top_limit; is_changed = true; }
					if( key_position.Y < m_field_bottom_limit )		{ key_position.Y = m_field_bottom_limit; is_changed = true; }

					if( is_changed )
					{
						key.move_to			( key_position );
						affected_curves.Add	( visual_curve );
					}
				}
			}

			foreach( var curve in affected_curves )
				curve.float_curve.fire_curve_changed( );

		}
		private			void					toggle_key_type_buttons			( ToggleButton checked_button )					
		{
			m_linked_auto_button.IsChecked		= m_linked_auto_button == checked_button;
			m_linked_manual_button.IsChecked	= m_linked_manual_button == checked_button;
			m_linear_button.IsChecked			= m_linear_button == checked_button;
			m_step_button.IsChecked				= m_step_button == checked_button;
			m_breaked_button.IsChecked			= m_breaked_button == checked_button;
		}
		private			void					toggle_key_type_buttons			( List<visual_curve_key> keys )					
		{
			m_linked_auto_button.IsChecked		= false;
			m_linked_manual_button.IsChecked	= false;
			m_linear_button.IsChecked			= false;
			m_step_button.IsChecked				= false;
			m_breaked_button.IsChecked			= false;

			foreach( var selected_key in keys )
			{
				m_linked_auto_button.IsChecked		= m_linked_auto_button.IsChecked == true || selected_key.type_of_key == float_curve_key_type.locked_auto;
				m_linked_manual_button.IsChecked	= m_linked_manual_button.IsChecked == true || selected_key.type_of_key == float_curve_key_type.locked_manual;
				m_linear_button.IsChecked			= m_linear_button.IsChecked == true || selected_key.type_of_key == float_curve_key_type.linear;
				m_step_button.IsChecked				= m_step_button.IsChecked == true || selected_key.type_of_key == float_curve_key_type.step;
				m_breaked_button.IsChecked			= m_breaked_button.IsChecked == true || selected_key.type_of_key == float_curve_key_type.breaked;
			}
		}
		private			void					process_just_selected_entities	( )												
		{
			foreach( var button in m_key_type_buttons.Children.OfType<UIElement>( ) )
					button.IsEnabled = false;

			m_toggle_range_button.IsChecked				= false;
			m_keys_x_position_text_box.Text				= "";
			m_keys_y_position_text_box.Text				= "";

			var shift_pressed	= Keyboard.IsKeyDown( Key.LeftShift ) || Keyboard.IsKeyDown( Key.RightShift );
			var ctrl_pressed	= Keyboard.IsKeyDown( Key.LeftCtrl ) || Keyboard.IsKeyDown( Key.RightCtrl );
			var new_selection	= !shift_pressed && !ctrl_pressed;
			
			m_selected_entity = selected_entity_type.none;

			m_points_positions.Clear( );

			toggle_key_type_buttons( new List<visual_curve_key>( ) );

			if ( new_selection )
			{
				deselect_all( );
				if( m_just_selected_tangents.Count == 0 && m_just_selected_keys.Count == 0 )
					hide_all_tangents( );

				hide_all_key_range_controls( );
			}

			if ( ( new_selection || !has_selected_keys ) && m_just_selected_keys.Count == 0 )
			{
				deselect_all_keys( );
				if( m_just_selected_tangents.Count > 0 )
				{
					foreach ( var tangent in m_just_selected_tangents )
						tangent.process_selection( );
					m_selected_entity = selected_entity_type.tangents;

					m_linked_auto_button.IsEnabled	= true;
					m_linear_button.IsEnabled		= true;
					m_step_button.IsEnabled			= true;
				}
				else if( m_just_selected_key_range_controls.Count > 0 )
				{
					deselect_all_tangents( );
					foreach ( var control in m_just_selected_key_range_controls )
						control.process_selection( );
					
					m_selected_entity = selected_entity_type.key_range_controls;

					( (UIElement)m_keys_y_position_text_box.Parent ).IsEnabled = true;
					
					initialize_selected_items_position_viewer( );
				}

				return;
			}
			
			if( !has_selected_keys )
				hide_all_tangents( );

			deselect_all_tangents( );
			foreach ( var key in m_just_selected_keys )
				key.process_selection( );

			m_selected_entity = selected_entity_type.keys;

			toggle_key_type_buttons( m_selected_keys );

			foreach( var button in m_key_type_buttons.Children.OfType<UIElement>( ) )
				button.IsEnabled = true;

			m_toggle_range_button.IsChecked = m_selected_keys.Any( key => key.parent_curve.is_range );

			initialize_selected_items_position_viewer( );
		}
		private			void					selected_entity_set_type		( float_curve_key_type key_type )				
		{
			switch( selected_entity )
			{
				case selected_entity_type.keys:				selected_keys_set_type( key_type );			break;
				case selected_entity_type.tangents:			selected_tangents_set_type( key_type );		break;
			}
		}
		private			void					selected_keys_set_type			( float_curve_key_type key_type )				
		{
			foreach ( var curv in items.OfType<visual_curve>( ) )
			{
				var flag = false;
				
				if( selected_entity == selected_entity_type.keys )
				{
					
					foreach ( var visual_key in curv.selected_keys )
					{
						visual_key.type_of_key = key_type;
						flag = true;
					}
				}
				if( selected_entity == selected_entity_type.tangents )
				{
					foreach ( var tangent in curv.selected_tangents )
					{
						tangent.compute_tangent(  );
						flag = true;
					}
				
				}

				if( flag )
					on_float_curve_changed( curv.float_curve );
			}
		}
		private			void					selected_tangents_set_type		( float_curve_key_type key_type )				
		{
			foreach ( var curv in items.OfType<visual_curve>( ) )
			{
				var flag = false;
				foreach ( var visual_tangent in curv.selected_tangents )
				{
					visual_tangent.compute_tangent_as( key_type );
					flag = true;
				}
				if( flag )
					on_float_curve_changed( curv.float_curve );
			}
		}

		private			void					mouse_down						( Object sender, MouseButtonEventArgs e )		
		{
			if( !is_read_only )
				if( m_active_tool.mouse_down( e ) )
				{
					m_is_tool_captured = true;

					m_points_positions.Clear( );

					if( m_selected_entity == selected_entity_type.key_range_controls )
						foreach( var range_control in items.OfType<visual_curve>( ).SelectMany( item => item.selected_key_range_controls ) )
							m_points_positions.Add( new Point( 0, range_control.range_delta ) );
					
					else if( m_selected_entity == selected_entity_type.tangents )
						foreach( var tangent in items.OfType<visual_curve>( ).SelectMany( item => item.selected_tangents ) )
							m_points_positions.Add( tangent.position );

					else if( m_selected_keys.Count > 0 )
						foreach( var key in m_selected_keys )
							m_points_positions.Add( key.position );
					

					if( m_points_positions.Count > 0 )
					{
						m_selected_points_top_left		= m_points_positions[0];
						m_selected_points_bottom_right	= m_selected_points_top_left;

						var count = m_points_positions.Count;
						for ( var i = 1; i < count; ++i )
						{
							var key_pos = m_points_positions[i];

							if( key_pos.X < m_selected_points_top_left.X ) m_selected_points_top_left.X = key_pos.X;
							if( key_pos.Y > m_selected_points_top_left.Y ) m_selected_points_top_left.Y = key_pos.Y;
							if( key_pos.X > m_selected_points_bottom_right.X ) m_selected_points_bottom_right.X = key_pos.X;
							if( key_pos.Y < m_selected_points_bottom_right.Y ) m_selected_points_bottom_right.Y = key_pos.Y;
						}
					}

					return;
				}


			if ( e.ChangedButton == MouseButton.Left )
			{
				if( is_read_only )
					return;				

				m_is_selection_started		= true;
				m_is_dotted_selection		= true;
				m_selection_start_position	= Mouse.GetPosition( m_curves_panel );
				m_curves_panel.CaptureMouse	( );
				m_selection_rect.Rect		= new Rect(m_selection_start_position, m_selection_start_position + new Vector(1, 1));
				if( m_selection_path.Parent == null )
					items.Add	( m_selection_path );
				e.Handled = true;
			}
			else if ( e.ChangedButton == MouseButton.Middle && Keyboard.PrimaryDevice.IsKeyDown( Key.LeftAlt ) )
			{
				m_mouse_position			= e.GetPosition( m_curves_panel );
				m_is_field_move_captured	= true;
				m_curves_panel.CaptureMouse( );
				m_curves_panel.Cursor		= Cursors.SizeAll;
			}
			else if( e.ChangedButton == MouseButton.Right  && Keyboard.PrimaryDevice.IsKeyDown( Key.LeftAlt ) )
			{
				m_is_field_scale_captured	= true;
				m_mouse_position			= Mouse.GetPosition( m_curves_panel );
				m_scaling_start_position	= m_mouse_position;
				m_start_scale				= scale;
				m_curves_panel.CaptureMouse( );
				m_curves_panel.Cursor		= Cursors.SizeAll;
			}
			else if( e.ChangedButton == MouseButton.Right )
			{
				if( is_read_only )
					return;

				if( Keyboard.PrimaryDevice.IsKeyDown( Key.LeftAlt ) )
					return;

				if( items.Count > 0 )
					open_remove_menu( m_selected_keys );
			}
		}
		private			void					mouse_move						( Object sender, MouseEventArgs e )				
		{
			if( m_is_tool_captured )
				if( m_active_tool.mouse_move( e ) )
					return;

			if ( m_is_field_move_captured || m_is_field_scale_captured )
			{
				var new_mouse_position	= Mouse.GetPosition( m_curves_panel );
				var mouse_offset		= (Point)( new_mouse_position - m_mouse_position );
				m_mouse_position		= new_mouse_position;
				mouse_offset.Y			= -mouse_offset.Y;

				if( m_is_field_move_captured )
				{
					pan		+= (Vector) new Point( mouse_offset.X, -mouse_offset.Y );
				}
				if( m_is_field_scale_captured )
				{
					var logic_mouseposition = visual_to_logical_point(m_scaling_start_position);

					if( is_zoom_locked )
						mouse_offset.Y		= mouse_offset.X;
					
					scale					= new Point(
						Math.Max( scale.X + ( m_start_scale.X * mouse_offset.X / 200 ), m_start_scale.X / 20 ),
						Math.Max( scale.Y + ( m_start_scale.Y * mouse_offset.Y / 200 ), m_start_scale.Y / 20 )
					);
					
					var logic_mouse_offset	= logical_to_visual_point( logic_mouseposition );
					pan						+= (Vector)m_scaling_start_position - (Vector)logic_mouse_offset;
				}

				foreach( var curve in items.OfType<visual_curve>( ) )
					curve.update( );

				m_curves_panel.InvalidateVisual( );

				if( m_is_field_move_captured && pan_changed != null )
					pan_changed( );

				if( m_is_field_scale_captured && scale_changed != null )
					scale_changed( );

				update_scroll_bar( );
			}
			else if( m_is_selection_started )
			{
				var new_position		= Mouse.GetPosition( m_curves_panel );
				m_selection_rect.Rect	= new Rect( m_selection_start_position, new_position );
				if( m_selection_start_position != new_position )
					m_is_dotted_selection = false;
			}
		}
		private			void					mouse_up						( Object sender, MouseButtonEventArgs e )		
		{
			if( m_is_tool_captured )
				if( m_active_tool.mouse_up( e ) )
					return;

			if ( ( m_is_field_scale_captured || m_is_field_move_captured ) && e.MiddleButton == MouseButtonState.Released )
			{
				m_is_tool_captured			= false;
				m_is_field_move_captured	= false;
				m_is_field_scale_captured	= false;
				m_curves_panel.ReleaseMouseCapture( );
				m_curves_panel.Cursor		= null;
				return;
			}

			if( e.ChangedButton == MouseButton.Left )
			{
				if ( !m_is_selection_started ) 
					return;

				m_is_selection_started						= false;

				m_just_selected_keys.Clear					( );
				m_just_selected_tangents.Clear				( );
				m_just_selected_key_range_controls.Clear	( );

				VisualTreeHelper.HitTest	( m_curves_panel, filter_visual, selecting_visual, new GeometryHitTestParameters( m_selection_rect ) );

				m_curves_panel.ReleaseMouseCapture			( );
				items.Remove								( m_selection_path );

				process_just_selected_entities				( );

				if( selection_changed != null )
					selection_changed( );
			}
				
		}

		private			void					groups_panel_mouse_down			( Object sender, MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Left )
				deselect_all_groups( );
		}
		private			void					preview_mouse_down				( Object sender, MouseButtonEventArgs e )		
		{
			Focus( );
		}
		private			void					preview_key_down				( Object sender, KeyEventArgs e )				
		{
			switch( e.Key )
			{
				case Key.W:		m_move_tool_button.IsChecked = true;		break;
				case Key.R:		m_scale_tool_button.IsChecked = true;		break;
				case Key.A:		m_add_tool_button.IsChecked = true;			break;
				case Key.F:		
					if( m_selected_keys.Count > 0 )
						focus_selection( );
					else
						focus_curves( );
					break;
				case Key.D:
					if( is_default_range_enabled )
						focus_default_range( );
					break;
				case Key.Delete:
					remove_menu_item_click( this, new RoutedEventArgs( ) );
					break;
			}
		}

		private static	HitTestFilterBehavior	filter_visual					( DependencyObject potential_hittest_target )	
		{
			if( potential_hittest_target is Path )
				return HitTestFilterBehavior.ContinueSkipSelfAndChildren;

			return HitTestFilterBehavior.Continue;
		}
		private			HitTestResultBehavior	selecting_visual				( HitTestResult result )						
		{
			if( result.VisualHit is visual_curve_key_range_control )
			{
				var key_range_control = (visual_curve_key_range_control)result.VisualHit;
				if( key_range_control.Visibility == Visibility.Visible && key_range_control.parent_key.parent_curve.Visibility == Visibility.Visible )
				{
					m_just_selected_key_range_controls.Add( key_range_control );
					return m_is_dotted_selection ? HitTestResultBehavior.Stop : HitTestResultBehavior.Continue;
				}

				return HitTestResultBehavior.Continue;
			}

			if( result.VisualHit is Path )
			{
				var dot = ( (Path)result.VisualHit ).Parent;
				if( dot is visual_curve_key_tangent )
				{
					var key_tangent = (visual_curve_key_tangent)dot;
					if( key_tangent.Visibility == Visibility.Visible && key_tangent.parent_key.parent_curve.Visibility == Visibility.Visible )
					{
						m_just_selected_tangents.Add( key_tangent );
						return m_is_dotted_selection ? HitTestResultBehavior.Stop : HitTestResultBehavior.Continue;
					}

					return HitTestResultBehavior.Continue;
				}
			}

			if( result.VisualHit is Rectangle )
			{
				var dot = ( (Rectangle)result.VisualHit ).Parent;

				if( dot is visual_curve_key_tangent )
				{
					var key_tangent = (visual_curve_key_tangent)dot;
					if( key_tangent.Visibility == Visibility.Visible && key_tangent.parent_key.parent_curve.Visibility == Visibility.Visible )
					{
						m_just_selected_tangents.Add(key_tangent);
						return m_is_dotted_selection ? HitTestResultBehavior.Stop : HitTestResultBehavior.Continue;
					}
					return HitTestResultBehavior.Continue;
				}

				dot = ( (FrameworkElement)dot ).Parent;
				if( dot is visual_curve_key )
				{
					var key = (visual_curve_key)dot;
					if( key.parent_curve.Visibility == Visibility.Visible )
					{
						m_just_selected_keys.Add(key);
						return m_is_dotted_selection ? HitTestResultBehavior.Stop : HitTestResultBehavior.Continue;
					}
					return HitTestResultBehavior.Continue;
				}
			}
			return HitTestResultBehavior.Continue;
		}

		private			void					size_changed					( Object sender, SizeChangedEventArgs e )		
		{
			foreach( var curve in items )
			{
				if( ! ( curve is visual_curve ) )
					continue;
				
				var curv = (visual_curve)curve;

				curv.update_first_segment_visual( );
				curv.update_last_segment_visual( );
			}
		}

		private			void					deselect_all					( )												
		{
			deselect_all_keys				( );
			deselect_all_tangents			( );
			deselect_all_keys_range_controls( );
		}
		private			void					deselect_all_keys_range_controls( )												
		{
			foreach ( var curve in items.OfType<visual_curve>( ) )
					curve.deselect_all_range_controls( );
		}
		private			void					deselect_all_tangents			( )												
		{
			foreach ( var curve in items.OfType<visual_curve>( ) )
				curve.deselect_all_tangents( );
		}
		private			void					hide_all_tangents				( )												
		{
			foreach (var item in items)
			{
				if (item is visual_curve)
				{
					var v_curve = (visual_curve)item;
					v_curve.hide_all_tangents();
				}
			}
		}
		private			void					hide_all_key_range_controls		( )												
		{
			foreach ( var item in items )
			{
				if ( item is visual_curve )
				{
					var v_curve = (visual_curve)item;
					v_curve.hide_all_key_range_controls( );
				}
			}
		}
		private			void					focus_rect						( Rect rect )									
		{
			var border_size				= new Point( 100, 100 );

			var actual_width	= m_curves_panel.ActualWidth;
			var actual_height	= m_curves_panel.ActualHeight;

			if( actual_height == 0 )		actual_height	= 5;
			if( actual_width == 0 )			actual_width	= 5;

			if( rect.Width < math.epsilon_4 )
			{
				rect.Union( new Point( rect.X + ( actual_width - border_size.X ) / 2, rect.Y ) );
				rect.Union( new Point( rect.X - ( actual_width - border_size.X ) / 2, rect.Y ) );
			}

			if( rect.Height < math.epsilon_4 )
			{
				rect.Union( new Point( rect.X, rect.Y + ( actual_height - border_size.Y ) / 2 ) );
				rect.Union( new Point( rect.X, rect.Y - ( actual_height - border_size.Y ) / 2 ) );
			}

			var horizontal_scale	= actual_width	/ ( rect.Size.Width );
			var vertical_scale		= actual_height	/ ( rect.Size.Height );
			
			scale					= new Point( horizontal_scale, vertical_scale );

			if( scale == new Point( 0, 0 ) )
				scale = new Point( 1, 1 );

			m_scale.X				*= actual_width / ( actual_width + border_size.X );
			m_scale.Y				*= actual_height / ( actual_height + border_size.Y );

			pan						= new Point( -rect.X * m_scale.X + border_size.X / 2, rect.Bottom * m_scale.Y + border_size.Y / 2 );

			foreach( var curve in items.OfType<visual_curve>( ) )
				curve.update( );

			m_curves_panel.InvalidateVisual( );

			if( pan_changed != null )
				pan_changed		( );
			if( scale_changed != null )
				scale_changed	( );

			update_scroll_bar	( );
		}
		private			void					focus_visual_rect				( Rect visual_rect )							
		{
			focus_rect( new Rect( visual_to_logical_point( new Point( visual_rect.X, visual_rect.Y + visual_rect.Height ) ) , new Size( visual_rect.Width / m_scale.X, visual_rect.Height / m_scale.Y ) ) );
		}
		private			Rect					get_visible_curves_rect			( )												
		{
			var visuals_rect		= Rect.Empty;

			foreach ( var curve in items.OfType<visual_curve>( ).Where( curve => curve.Visibility == Visibility.Visible ) )
			{
				var visual_rect			=  curve.path.Data.GetRenderBounds( m_stroke_pen );
                if ( visual_rect == Rect.Empty )
					continue;

				visual_rect.X		= curve.left_end;
				visual_rect.Width	= curve.right_end - curve.left_end;

				if( visuals_rect == Rect.Empty )
					visuals_rect = visual_rect;
				else
					visuals_rect.Union( visual_rect );

				if( curve.is_range )
				{
					var range_visual_rect	= curve.range.path.Data.GetRenderBounds( m_stroke_pen );
					if ( range_visual_rect == Rect.Empty )
						continue;

					range_visual_rect.X		= curve.left_end;
					range_visual_rect.Width	= curve.right_end - curve.left_end;

					if( visuals_rect == Rect.Empty )
						visuals_rect = range_visual_rect;
					else
						visuals_rect.Union( range_visual_rect );
				}
			}

			return visuals_rect;
		}
		private			String					find_curve_key					( float_curve curve )							
		{
			foreach( var curve_pair in m_panel_curves )
			{
				if( curve_pair.Value == curve )
					return curve_pair.Key;
			}
			return null;
		}

		private			visual_curve			get_curve						( )												
		{
			if( selected_keys.Count > 0 )
				return selected_keys[0].parent_curve;

			var selected_curves = new List<visual_curve>( );

			foreach( panel_curve_group group in m_curves_groups.Items )
				group.get_selected_curves( selected_curves );

			if( selected_curves.Count > 0 )
				return selected_curves[0];

			return null;
		}
		private			void					set_template					( curve_template_base template )				
		{
			var curve = get_curve( );
			if( curve != null )
			{
				template.curve			= curve;
				template.commit			( );
				template.show_settings	( );
			}
		}
		private			void					set_sinusoid_template			( Object sender, RoutedEventArgs e )			
		{
			set_template( new sinusoid_curve_template( ) );
		}
		private			void					set_sqrt_template				( Object sender, RoutedEventArgs e )			
		{
			set_template( new sqrt_curve_template( ) );
		}
		private			void					set_stairs_template				( Object sender, RoutedEventArgs e )			
		{
			new sinusoid_curve_template( ).show_settings( );
		}
		private			void					set_digital_signal_template		( Object sender, RoutedEventArgs e )			
		{
			new sinusoid_curve_template( ).show_settings( );
		}

		internal		void					initialize_selected_items_position_viewer	( )									
		{
			switch( m_selected_entity )
			{
				case selected_entity_type.keys:
					{
						if( m_selected_keys.All( key => key.position_x == m_selected_keys[0].position_x ) ) 
							m_keys_x_position_text_box.Text		= "_";

						if( m_selected_keys.All( key => key.position_y == m_selected_keys[0].position_y ) ) 
							m_keys_y_position_text_box.Text		= "_";
					}
					break;
				case selected_entity_type.key_range_controls:
					{
						m_keys_x_position_text_box.Text			= "";
						m_keys_x_position_text_box.Tag			= m_keys_x_position_text_box.Text;

						if( m_selected_key_range_controls.All( range_control => range_control.range_delta == m_selected_key_range_controls[0].range_delta ) ) 
						    m_keys_y_position_text_box.Text		= "_";
					}
					break;
			}
			update_selected_items_position_viewer( );
		}
		internal		void					update_selected_items_position_viewer		( )									
		{
			switch( m_selected_entity )
			{
				case selected_entity_type.keys:
					{
						if( m_selected_keys ==null || m_selected_keys[0] == null )
							return;

						if( m_keys_x_position_text_box.Text != "" ) 
						{
							m_keys_x_position_text_box.Text		= String.Format( "{0:F3}", m_selected_keys[0].position_x );
							m_keys_x_position_text_box.Tag		= m_keys_x_position_text_box.Text;
						}

						if( m_keys_y_position_text_box.Text != "" ) 
						{
							m_keys_y_position_text_box.Text		= String.Format( "{0:F3}", m_selected_keys[0].position_y );
							m_keys_y_position_text_box.Tag		= m_keys_y_position_text_box.Text;
						}
					}
					break;
				case selected_entity_type.key_range_controls:
					{
						m_keys_x_position_text_box.Text			= "";
						m_keys_x_position_text_box.Tag			= m_keys_x_position_text_box.Text;

						if( m_keys_y_position_text_box.Text	!= "" && m_selected_key_range_controls.Count > 0 ) 
						{
						    m_keys_y_position_text_box.Text		= String.Format( "{0:F3}", m_selected_key_range_controls[0].range_delta );
						    m_keys_y_position_text_box.Tag		= m_keys_y_position_text_box.Text;
						}
						else
							m_keys_y_position_text_box.Text	= "";
					}
					break;
			}
		}
		internal		void					update_scroll_bar							( )									
		{
			if( m_curves_scroll_bar.Visibility != Visibility.Visible )
				return;

			m_is_scroll_bar_updating = true;

			var minimum = Double.MaxValue;
			var maximum = Double.MinValue;

			foreach( var curve in items.OfType<visual_curve>( ) )
			{
				if( curve.keys[0].position_x * scale.X < minimum )
					minimum = curve.keys[0].position_x * scale.X;

				if( curve.keys[curve.keys.Count - 1].position_x * scale.X > maximum )
					maximum = curve.keys[curve.keys.Count - 1].position_x * scale.X;
			}

			minimum -= 100;
			maximum += 100;

			m_curves_scroll_bar.IsEnabled		= maximum - minimum > m_curves_panel.ActualWidth;

			maximum -= m_curves_panel.ActualWidth;

			if( m_curves_scroll_bar.IsEnabled )
			{
				m_curves_scroll_bar.Minimum			= minimum;
				m_curves_scroll_bar.Maximum			= maximum;
				m_curves_scroll_bar.ViewportSize	= m_curves_panel.ActualWidth;
				m_curves_scroll_bar.Value			= -pan.X;
				m_curves_scroll_bar.SmallChange		= ( m_curves_scroll_bar.Maximum - m_curves_scroll_bar.Minimum ) / 100;
				m_curves_scroll_bar.LargeChange		= ( m_curves_scroll_bar.Maximum - m_curves_scroll_bar.Minimum ) / 10;
			}

			m_is_scroll_bar_updating = false;
		}

		internal		visual_curve			add_curve						( String key, float_curve curve )				
		{
			var v_curve				= new visual_curve
			{ 
				parent_panel = this,
				brush = new SolidColorBrush( curve.color ) 
			};
			m_panel_curves.Add		( key, curve );
			items.Add				( v_curve );
			v_curve.float_curve		= curve;

			v_curve.update( );

			update_scroll_bar( );

			return v_curve;
		}
		internal		void					remove_curve					( float_curve curve )							
		{
			m_panel_curves.Remove( find_curve_key( curve ) );
			for( var i = items.Count - 1; i >= 0; --i )
			{
				if( !( items[i] is visual_curve ) || ( (visual_curve)items[i] ).float_curve != curve )
					continue;

				items.RemoveAt( i );
				return;
			}
		}
		internal		void					on_float_curve_changed			( float_curve curve )							
		{
			curve.fire_curve_changed( );
			if ( float_curve_changed != null )
				float_curve_changed( curve );
		}
		internal		void					on_float_curve_edit_complete	( float_curve curve )							
		{
			curve.fire_edit_completed( );
			if ( float_curve_edit_complete != null )
				float_curve_edit_complete( curve );
		}
		internal		void					deselect_all_keys				( )												
		{
			foreach ( var item in items.OfType<visual_curve>( ) )
			{
				var count = item.selected_keys.Count - 1;
				for ( var i = count; i >= 0; --i)
				{
					item.selected_keys[i].is_selected = false;
				}
				item.selected_keys.Clear( );
			}
			m_selected_keys.Clear( );
		}
		internal		Point					visual_to_logical_point			( Point point )									
		{
			return new Point(
				( point.X - pan.X ) / scale.X,
				-( point.Y - pan.Y ) / scale.Y
			);
		}
		internal		Point					logical_to_visual_point			( Point point )									
		{
			return new Point(
				( point.X * scale.X ) + pan.X, 
				-( point.Y * scale.Y ) + pan.Y
			);
		}
		internal		void					deselect_all_groups				( )												
		{
			foreach( var group in m_curves_groups.Items.OfType<panel_curve_group>( ) )
				group.is_selected = false;
		}

		public			void					select_keys_by_index			( Int32 index )									
		{
			m_just_selected_tangents.Clear				( );
			m_just_selected_keys.Clear					( );
			m_just_selected_key_range_controls.Clear	( );

			foreach( var curve in items.OfType<visual_curve>( ).Where( curve => curve.keys.Count > index ) )
				m_just_selected_keys.Add( curve.keys[index] );

			process_just_selected_entities				( );
		}
		public			panel_curve_group		add_group						( String name )									
		{
			var group					= new panel_curve_group( this ) { header = name };
			m_curves_groups.Items.Add	( group );
			return group;
		}
		public			panel_curve_group		add_group						( float_curve_group group )						
		{
			var panel_group					= new panel_curve_group( this, group ) { header = group.name };

			m_curves_groups.Items.Add		( panel_group );

			return panel_group;
		}
		public			void					focus_curves					( )												
		{
			if( m_panel_curves.Count == 0 )
				return;

			var visuals_rect		= get_visible_curves_rect( );
			
			if( visuals_rect.IsEmpty )
				visuals_rect = new Rect( 0, 0, 1, 1 );	
			
			focus_visual_rect( visuals_rect );
		}
		public			void					focus_selection					( )												
		{
			if( m_selected_keys.Count == 0 )
				return;

			var rect = new Rect( m_selected_keys[0].visual_position, m_selected_keys[0].visual_position );

			foreach( var curve in items.OfType<visual_curve>( ) )
			{
				if( !curve.has_selected_keys )
					continue;

				var start_index	= curve.keys.Count - 1;
				var end_index	= 0;

				foreach( var selected_key in curve.selected_keys )
				{
					if( selected_key.index < start_index )
						start_index = selected_key.index;

					if( selected_key.index > end_index )
						end_index = selected_key.index;
				}

				if( start_index != 0 )
					--start_index;
				if( end_index != curve.keys.Count - 1 )
					++end_index;

				Single delta;
				Single end;
				visual_curve_key left_key;
				visual_curve_key right_key;

				if( start_index == end_index )
				{
					left_key	= curve.keys[start_index];
					rect.Union	( logical_to_visual_point( left_key.position ) );	
				}
				else
				{
					for( ; start_index < end_index; ++start_index )
					{
						left_key	= curve.keys[start_index];
						right_key	= curve.keys[start_index + 1];

						hermite_spline_evaluator.hermite_create	( left_key.key, right_key.key );

						delta		= (Single)( right_key.position_x - left_key.position_x ) / focus_sample_count;
						end			= (Single)( right_key.position_x - left_key.position_x );

						for( Single on_spline_position = 0; on_spline_position < end; on_spline_position += delta )
							rect.Union( logical_to_visual_point( new Point( left_key.position_x + on_spline_position, hermite_spline_evaluator.evaluate( on_spline_position ) ) ) );	
					}
					rect.Union	( logical_to_visual_point( curve.keys[end_index].position ) );
				}
			}

			focus_visual_rect( rect );
		}
		public			void					focus_default_range				( )												
		{
			var visuals_rect			= get_visible_curves_rect( );

			if( !Double.IsNaN( m_field_left_limit ) )
			{
				var point			= logical_to_visual_point( new Point( m_field_left_limit, 0 ) );
				point.Y				= visuals_rect.Y;
				visuals_rect.Union	( point );
			}

			if( !Double.IsNaN( m_field_right_limit ) )
			{
				var point			= logical_to_visual_point( new Point( m_field_right_limit, 0 ) );
				point.Y				= visuals_rect.Y;
				visuals_rect.Union	( point );
			}

			if( !Double.IsNaN( m_field_top_limit ) )
			{
				var point			= logical_to_visual_point( new Point( 0, m_field_top_limit ) );
				point.X				= visuals_rect.X;
				visuals_rect.Union	( point );
			}

			if( !Double.IsNaN( m_field_bottom_limit ) )
			{
				var point			= logical_to_visual_point( new Point( 0, m_field_bottom_limit ) );
				point.X				= visuals_rect.X;
				visuals_rect.Union	( point );
			}

			focus_visual_rect ( visuals_rect );
		}
		public			void					clear							( )												
		{
			items.Clear					( );
			m_curves_groups.Items.Clear	( );

			m_panel_curves.Clear		( );
			m_selected_keys.Clear		( );
		}
		public			curves_link				link_curves						( IList<visual_curve> curves )					
		{
			return new curves_link( curves );
		}
		public			void					update_curves					( )												
		{
			foreach( var curve in items.OfType<visual_curve>( ) )
				curve.update( );
		}
		public			void					update_visual					( )												
		{
			foreach( var curve in items.OfType<visual_curve>( ) )
				curve.update_visual( );

			InvalidateVisual	( );
		}
		public			void					raise_curves_change				( )												
		{
			foreach( var curve in items.OfType<visual_curve>( ) )
			{
				curve.raise_curve_changed( );
			}
		}
		public			void					update_chacked_key_type_buttons	( )												
		{
			toggle_key_type_buttons( selected_keys );
		}
		public			void					set_scale						( Vector new_scale )							
		{
			m_scale = (Point)new_scale;
			update_visual( );
		}
		public			void					add_custom_template				( float_curve template )						
		{
			custom_templates.Add( template );
			var custom_template = new custom_curve_template( template );
			var menu_item		= new MenuItem{ Header = template.name, Tag = custom_template };
			menu_item.Click		+= ( o, e ) =>
			{
				var templ = (custom_curve_template)( (MenuItem)o ).Tag;
				var curve = get_curve( );
				if( curve != null )
				{
					templ.curve		= curve;
					templ.commit	( ); 
				}
				
			};
			m_set_template_menu_item.Items.Add( menu_item );
		}


		#endregion

		#region | InnerTypes |


		internal enum selected_entity_type
		{
			none,
			keys,
			tangents,
			key_range_controls,
		}


		#endregion

	}
}