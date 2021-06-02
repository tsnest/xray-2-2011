////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using xray.editor.wpf_controls.curve_editor.effects;

namespace xray.editor.wpf_controls.curve_editor
{
	/// <summary>
	/// Interaction logic for curve_control.xaml
	/// </summary>
	public partial class visual_curve
	{

		#region | Initialize |


		public	visual_curve( )		
		{
			InitializeComponent( );

			m_keys							= new List<visual_curve_key>( );
			m_segments						= new List<visual_curve_segment>( );
			m_effects						= new List<visual_effect_base>( );
			m_selected_keys					= new ObservableCollection<visual_curve_key>( );
			m_selected_tangents				= new ObservableCollection<visual_curve_key_tangent>( ); 
			m_selected_key_range_controls	= new ObservableCollection<visual_curve_key_range_control>( );

			m_selected_keys.CollectionChanged += ( o, e ) => {
				switch( e.Action )
				{
					case NotifyCollectionChangedAction.Add:			parent_panel.selected_keys.AddRange( e.NewItems.Cast<visual_curve_key>( ) ); break;
					case NotifyCollectionChangedAction.Remove:		foreach( var key in e.OldItems.Cast<visual_curve_key>( ) ) parent_panel.selected_keys.Remove( key ); break;
					case NotifyCollectionChangedAction.Reset:		parent_panel.selected_keys.Clear( ); break;	
				}
			};

			m_selected_tangents.CollectionChanged += ( o, e ) => {
             	switch( e.Action )
             	{
             		case NotifyCollectionChangedAction.Add:			parent_panel.selected_key_tangents.AddRange( e.NewItems.Cast<visual_curve_key_tangent>( ) );  break;
					case NotifyCollectionChangedAction.Remove:		foreach( var tangent in e.OldItems.Cast<visual_curve_key_tangent>( ) ) parent_panel.selected_key_tangents.Remove( tangent ); break;
					case NotifyCollectionChangedAction.Reset:		parent_panel.selected_key_tangents.Clear( ); break;	
             	}
			};

			m_selected_key_range_controls.CollectionChanged += ( o, e ) => {
				switch( e.Action )
             	{
             		case NotifyCollectionChangedAction.Add:			parent_panel.selected_key_range_controls.AddRange( e.NewItems.Cast<visual_curve_key_range_control>( ) );  break;
					case NotifyCollectionChangedAction.Remove:		foreach( var key in e.OldItems.Cast<visual_curve_key_range_control>( ) ) parent_panel.selected_key_range_controls.Remove( key ); break;
					case NotifyCollectionChangedAction.Reset:		parent_panel.selected_key_range_controls.Clear( ); break;	
             	}
			};

			m_add_key_menu					= new ContextMenu( );
			var add_menu_item				= new MenuItem { Header = "Add key" };
			add_menu_item.Click				+= add_menu_item_click;
			m_add_key_menu.Items.Add		( add_menu_item );

			#region | Curve creation |


			m_path_figure			= new PathFigure( );

			m_path = new Path
			{
				SnapsToDevicePixels	= false,
				StrokeLineJoin		= PenLineJoin.Round,
				Stroke				= Brushes.Black,
				Data				= new PathGeometry
				{
					Figures = new PathFigureCollection { m_path_figure }
				}
			};
			m_shadow_path = new Path
			{
				SnapsToDevicePixels	= false,
				StrokeLineJoin		= PenLineJoin.Round,
				Stroke				= new SolidColorBrush( new Color{ A = 1, R = 190, G = 190, B = 190 } ),
				StrokeThickness		= 7,
				Data				= new PathGeometry
				{
					Figures = new PathFigureCollection { m_path_figure }
				}
			};
			m_shadow_path.MouseEnter					+= path_mouse_enter;
			m_shadow_path.MouseLeave					+= path_mouse_leave;
			m_shadow_path.MouseRightButtonDown			+= path_mouse_right_button_down;
			m_shadow_path.MouseLeftButtonDown			+= path_mouse_left_button_down;
			curve_elements_container.Children.Add		( m_shadow_path );
			curve_elements_container.Children.Add		( m_path );
			m_first_segment								= new BezierSegment( new Point( 0, 0 ), new Point( 0, 0 ), new Point( 0, 0 ), true );
			m_last_segment								= new BezierSegment( new Point( 0, 0 ), new Point( 0, 0 ), new Point( 0, 0 ), true );
			path_figure.Segments						= new PathSegmentCollection{ m_first_segment };
			path_figure.Segments						= new PathSegmentCollection{ m_last_segment };

			m_shadow_path.SetValue( ZIndexProperty, 5 );


			#endregion

		}

		
		#endregion

		#region |   Events   |


		public event	Action<visual_curve_key>	key_added;
		public event	Action<visual_curve_key>	key_removed;
		public event	Action<Int32>				key_changed;
		public event	Action<Int32, Int32>		keys_swaped;
		public event	Action						double_click;
		public event	Action						is_selected_changed;


		#endregion

		#region |   Fields   |


		private		float_curve							m_float_curve;
		readonly	List<visual_curve_key>				m_keys;
		readonly	List<visual_curve_segment>			m_segments;
		readonly	List<visual_effect_base>			m_effects;
		readonly	ObservableCollection<visual_curve_key>					m_selected_keys;
		readonly	ObservableCollection<visual_curve_key_tangent>			m_selected_tangents;
		readonly	ObservableCollection<visual_curve_key_range_control>	m_selected_key_range_controls;

		readonly	Path								m_path;
		readonly	PathFigure							m_path_figure;
		readonly	Path								m_shadow_path;

		readonly	ContextMenu							m_add_key_menu;

		private		Boolean								m_is_selected;

		private		Point								m_mouse_position;
		private		BezierSegment						m_first_segment;
		private		BezierSegment						m_last_segment;
		private		Int32								m_last_left_button_down;


		#endregion

		#region | Properties |


		internal	visual_curve_range				range								
		{
			get;set;
		}
		internal	List<visual_curve_segment>		segments							
		{
			get
			{
				return m_segments;
			}
		}
		internal	Boolean							is_range							
		{
			get	
			{
				return m_float_curve.is_range;
			}
			set	
			{
				m_float_curve.is_range = value;
				if( value )
				{
					if( range == null )
					{
						range = new visual_curve_range( this );
						Children.Add( range.path );
					}
					range.path.Visibility = Visibility.Visible;
				}
				else
				{
					if( range != null )
						range.path.Visibility = Visibility.Collapsed;
				}
			}
		}
		internal	ObservableCollection<visual_curve_key_tangent>			selected_tangents				
		{
			get 
			{
				return m_selected_tangents;
			}
		}
		internal	ObservableCollection<visual_curve_key_range_control>	selected_key_range_controls		
		{
			get 
			{
				return m_selected_key_range_controls;
			}
		}

		internal	List<visual_effect_base>		effects								
		{
			get 
			{
				return m_effects;
			}
		}
		
		public		curve_editor_panel				parent_panel						
		{
			get;set;
		}
		public		float_curve						float_curve							
		{
			get	
			{
				return m_float_curve;
			}
			set	
			{
				m_float_curve		= value;
				name				= m_float_curve.name;
				make_visual_curve	( ); 
			}
		}
		public		String							name								
		{
			get;set;
		}
		public		float_curve_monotone_type		monotone_type						
		{
			get
			{
				return m_float_curve.monotone_type;
			}
		}
		public		List<visual_curve_key>			keys								
		{
			get 
			{
				return m_keys;
			}
		}
		public		ObservableCollection<visual_curve_key>	selected_keys				
		{
			get { return m_selected_keys;}
		}
		
		public		Path							path								
		{
			get 
			{
				return m_path;
			}
		}
		public		PathFigure						path_figure							
		{
			get 
			{
				return m_path_figure;
			}
		}

		public		Boolean							is_selected							
		{
			get
			{
				return m_is_selected;
			}
			set
			{
				if( m_is_selected != value )
				{
					if( value )
					{
						m_path.Tag		= m_path.Stroke;
						m_path.Stroke	= Brushes.Yellow;
					}
					else
					{
						m_path.Stroke	= (Brush)m_path.Tag;
					}
				}
				
				m_is_selected = value;
                
				if( is_selected_changed != null )
					is_selected_changed( );
			}
		}
		public		Boolean							has_selected_keys					
		{
			get{ return selected_keys.Count > 0; }
		}
		
        public		Brush                           brush								
        {
            get { return m_path.Stroke; }
            set { m_path.Stroke = value; }
        }
		public		Double							right_end							
		{
			get 
			{
				return m_keys[m_keys.Count-1].visual_position.X;
			}
		}
		public		Double							left_end							
		{
			get 
			{
				return m_keys[0].visual_position.X;
			}
		}
		public		Point							visual_start_point					
		{
			get
			{
				return m_path_figure.StartPoint;
			}
		}
		public		Point							visual_end_point					
		{
			get
			{
				return m_last_segment.Point3;
			}
		}
		

		#endregion

		#region |  Methods   |


		private		void				path_mouse_enter				( Object sender, MouseEventArgs e )			
		{
			is_selected = true;
		}
		private		void				path_mouse_leave				( Object sender, MouseEventArgs e )			
		{
			is_selected = false;
		}
		private		void				path_mouse_left_button_down		( Object sender, MouseButtonEventArgs e)	
		{
			if( parent_panel.is_read_only )
				return;

			var cur_time = Environment.TickCount;
			if( ( cur_time - m_last_left_button_down ) * 1.0f / TimeSpan.TicksPerSecond < 0.00003f )
			{
				// double click maked
				if( parent_panel.double_click_key_adding_enabled )
				{
					m_add_key_menu.Tag		= m_path_figure;
					m_mouse_position		= Mouse.GetPosition( m_path );
					add_menu_item_click		( this, new RoutedEventArgs( ) );
					e.Handled				= true;
				}
				if( double_click != null )
					double_click( );
			}
			m_last_left_button_down = cur_time;
		}
		private		void				path_mouse_right_button_down	( Object sender, MouseButtonEventArgs e )	
		{
			if( parent_panel.is_read_only )
				return;

			m_add_key_menu.Tag		= m_path_figure;
			m_add_key_menu.IsOpen	= true;
			m_mouse_position		= Mouse.GetPosition( m_path );
			e.Handled				= true;
		}
		private		float_curve_key		get_new_key_in_position			( Double logical_x_position, Int32 segment_index )	
		{
			var left_key	= keys[segment_index-1];
			var right_key	= keys[segment_index];

			hermite_spline_evaluator.hermite_create( left_key.key, right_key.key );

			var on_spline_position = logical_x_position - left_key.position_x;
			var y = hermite_spline_evaluator.evaluate( (Single)on_spline_position );

			var ret_key = new float_curve_key( new Point( logical_x_position, y ) );

			var right_thrid	= (Single)( right_key.position_x - logical_x_position ) / 3;
			var left_thrid	= (Single)( logical_x_position - left_key.position_x ) / 3;

			var new_right_tangent	= new Point( logical_x_position + right_thrid, hermite_spline_evaluator.evaluate( (Single)on_spline_position + right_thrid / 10000 ) );
			var new_left_tangent 	= new Point( logical_x_position - left_thrid, hermite_spline_evaluator.evaluate( (Single)on_spline_position - left_thrid / 10000 ) );

			new_right_tangent.Y		= y + ( new_right_tangent.Y - y ) * 10000;
			new_left_tangent.Y		= y + ( new_left_tangent.Y - y ) * 10000;

			ret_key.right_tangent	= new_right_tangent;
			ret_key.left_tangent	= new_left_tangent;

			Debug.Assert( ( ret_key.position_x >= ret_key.left_tangent.X ) && ( ret_key.right_tangent.X >= ret_key.position_x ), "wrong new point calculation in curve editor" );

			ret_key.range_delta		= (Single)( left_key.key.range_delta + ( right_key.key.range_delta - left_key.key.range_delta ) *  ( ( logical_x_position - left_key.position_x ) / ( right_key.position_x - left_key.position_x ) ) );

			return ret_key;
		}
		private		void				add_menu_item_click				( Object sender, RoutedEventArgs e )		
		{
			add_key( visual_to_logical_point( m_mouse_position ).X );
		}

		private		void				make_visual_curve				( )											
		{
			curve_elements_container.Children.Clear	( );
			path_figure.Segments.Clear				( );
			m_keys.Clear							( );
			path_figure.Segments.Add				( m_first_segment );
			curve_elements_container.Children.Add	( m_path );
			curve_elements_container.Children.Add	( m_shadow_path );

			visual_curve_key previous_key = null;
			foreach( var key in m_float_curve.keys )
			{
				var	visual_key					= new visual_curve_key			( this, key, m_keys.Count ); 
			
				if( previous_key != null )
				{
					var segment					= new visual_curve_segment	( this, m_segments.Count, previous_key, visual_key );
					visual_key.left_segment		= segment;
					previous_key.right_segment	= segment;
				}
		
				curve_elements_container.Children.Add	( visual_key );
				m_keys.Add								( visual_key );

				previous_key = visual_key;
			}

			path_figure.Segments.Add				( m_last_segment );

			if ( m_float_curve.is_range )
			{
				range = new visual_curve_range( this );
				Children.Add( range.path );
			}
		}

		internal	visual_curve_key	add_key							( Double logical_x_position )				
		{
			if( parent_panel.fix_point_position != null )
			{
				var pt					= new Point( logical_x_position, 0 );
				parent_panel.fix_point_position( ref pt );
				logical_x_position		= pt.X;
			}

			var count		= keys.Count;
			var i = 0;
			for (; i < count; ++i)
			{
				if ( m_keys[i].position_x >= logical_x_position )
					break;
			}
			var new_key_index = i;

			float_curve_key float_key	= null;

			if( new_key_index != keys.Count && new_key_index != 0 )
			{
				float_key = get_new_key_in_position( logical_x_position, new_key_index );

				if( keys[new_key_index - 1].key.key_type == float_curve_key_type.linear && keys[new_key_index].key.key_type == float_curve_key_type.linear )
					float_key.key_type = float_curve_key_type.linear;
				else
					float_key.key_type = float_curve_key_type.locked_manual;
			}
			else
			{
				if( new_key_index == 0 )
					float_key = new float_curve_key( new Point( logical_x_position, keys[0].position_y ), float_curve_key_type.linear ){range_delta = keys[0].key.range_delta};

				else if( new_key_index == keys.Count )
				{
					float_key = new float_curve_key( new Point( logical_x_position, keys[keys.Count - 1].position_y ), float_curve_key_type.linear ){range_delta = keys[keys.Count - 1].key.range_delta};
				}
			}

			Debug.Assert( float_key != null );

			var visual_key				= new visual_curve_key		( this, float_key, new_key_index );

			var left_key				= ( new_key_index != count ) ? visual_key : m_keys[new_key_index - 1];
			var right_key				= ( new_key_index != count ) ? m_keys[new_key_index] : visual_key;

			for ( var j = new_key_index; j < count; j++ )
				++keys[j].index;

			var segment					= new visual_curve_segment	( this, left_key.index, left_key, right_key );
			left_key.right_segment		= segment;
			left_key.left_segment		= ( left_key.index == 0 ) ? null : m_segments[segment.index - 1];

			right_key.left_segment		= segment;

			m_float_curve.keys.Insert		( new_key_index, float_key );
			m_keys.Insert					( new_key_index, visual_key );
            
			curve_elements_container.Children.Add		( visual_key );

			deselect_all_keys				( );
			visual_key.is_selected			= true;
			parent_panel.selected_keys.Add	( visual_key );

			if( key_added != null )
				key_added( visual_key );

			parent_panel.on_float_curve_changed( float_curve );

			visual_key.compute_key			( );

			update_first_segment_visual		( );
			update_last_segment_visual		( );
			
			parent_panel.update_scroll_bar	( );

			return visual_key;
		}
		internal	void				deselect_all_keys				( )											
		{
			if( parent_panel != null )
				parent_panel.deselect_all_keys( );
		}
		internal	void				deselect_all_range_controls		( )											
		{
			var count = selected_key_range_controls.Count - 1;
			for ( var i = count; i >= 0; --i )
				selected_key_range_controls[i].is_selected = false;

			m_selected_key_range_controls.Clear( );
		}
		internal	void				deselect_all_tangents			( )											
		{
			var count = selected_tangents.Count - 1;
			for ( var i = count; i >= 0; --i )
				selected_tangents[i].is_selected = false;

			m_selected_tangents.Clear( );
		}
		internal	void				hide_all_tangents				( )											
		{
			foreach ( var key in m_keys )
				key.hide_tangents( );
		}
		internal	void				hide_all_key_range_controls		( )											
		{
			foreach ( var key in m_keys )
				key.hide_key_range_control( );
		}
		internal	Point				visual_to_logical_point			( Point point )								
		{
			return parent_panel.visual_to_logical_point( point );
		}
		internal	Point				logical_to_visual_point			( Point point )								
		{
			return parent_panel.logical_to_visual_point( point );
		}
		internal	void				update							( )											
		{
			foreach ( var key in m_keys )
				key.compute_key( );

			update_first_segment_visual( );
			update_last_segment_visual ( );

			foreach ( var effect in m_effects )
				effect.update_visual( );
		}
		internal	void				update_visual					( )											
		{
			foreach ( var key in m_keys )
				key.update_visual( );

			foreach ( var effect in m_effects )
				effect.update_visual( );
		}
		internal	void				fire_key_changed				( visual_curve_key key )					
		{
			if( key_changed != null )
				key_changed( key.index );
		}
		internal	void				fire_key_swapped				( Int32 first_index, Int32 second_index )	
		{
			if( keys_swaped != null )
				keys_swaped( first_index, second_index );
		}

		public override		String		ToString						( )											
		{
			return "curve: " + name;
		}
		public		visual_curve_key	find_key_by_x					( Double animation_time )					
		{
			return m_keys.FirstOrDefault( key => Math.Abs( key.position_x - animation_time ) < Single.Epsilon );
		}
		public		void				remove_key						( visual_curve_key key_to_remove )			
		{
			var cur_curve		= key_to_remove.parent_curve;
			var key_index		= key_to_remove.index;

			if(cur_curve.keys.Count == 1)
			{
				MessageBox.Show("You can not remove last key.", "Stop", MessageBoxButton.OK, MessageBoxImage.Stop);
				return;
			}

			key_to_remove.is_selected		= false;
			var is_last_key					= key_to_remove.is_last_key;

			if( !is_last_key )
				m_segments[key_index].remove( );
			else
				m_segments[key_index - 1].remove_visuals( );

			cur_curve.m_float_curve.keys.RemoveAt	( key_index );
			cur_curve.m_keys.RemoveAt				( key_index );
			Children.Remove							( key_to_remove );

			var cnt = cur_curve.m_keys.Count;
			for ( var j = key_index; j < cnt; j++ )
				--cur_curve.m_keys[j].index;

			if( !is_last_key )
			{
				m_keys[key_to_remove.index].left_segment = key_to_remove.left_segment;
				m_keys[key_to_remove.index].compute_key( );
			}
			else
			{
				m_keys[key_to_remove.index-1].right_segment = null;
			}

			update_first_segment_visual			( );
			update_last_segment_visual			( );

			parent_panel.on_float_curve_changed	( float_curve );

			if( key_removed != null )
				key_removed						( key_to_remove );

			key_to_remove.parent_curve			= null;
			parent_panel.update_scroll_bar		( );
		}
		public		void				update_first_segment_visual		( )											
		{
			var visual_position			= keys.Count == 0 ? new Point( 0, 0 ) : keys[0].visual_position;
			
			var limit_point				= Double.IsNaN( parent_panel.field_left_limit ) ? new Point( -10, 0 ) : logical_to_visual_point( new Point( parent_panel.field_left_limit, 0 ) );

			m_path_figure.StartPoint	= new Point( Math.Max( limit_point.X, -10 ) , visual_position.Y );
			m_first_segment.Point1		= m_path_figure.StartPoint;
			m_first_segment.Point2		= m_path_figure.StartPoint;
			m_first_segment.Point3		= visual_position;

			if( range != null )
				range.update_start( );
		}
		public		void				update_last_segment_visual		( )											
		{
			var visual_position		= keys.Count == 0 ? new Point( 0, 0 ) : keys[keys.Count - 1].visual_position;
			var limit_point			= Double.IsNaN( parent_panel.field_right_limit ) ? new Point( ( (FrameworkElement)Parent).ActualWidth + 10, 0 ) : logical_to_visual_point( new Point( parent_panel.field_right_limit, 0 ) );

			m_last_segment.Point1	= visual_position;
			m_last_segment.Point2	= visual_position;
			m_last_segment.Point3	= new Point( Math.Min( limit_point.X, ( (FrameworkElement)Parent).ActualWidth + 10 ), visual_position.Y );

			if( range != null )
				range.update_end( );
		}
		public		void				raise_curve_changed				( )											
		{
			parent_panel.on_float_curve_changed( float_curve );
		}
		public		void				raise_key_changed				( Int32 key_index )							
		{
			if ( key_changed != null )
				key_changed( key_index );
		}
		

		#endregion

	}
}
