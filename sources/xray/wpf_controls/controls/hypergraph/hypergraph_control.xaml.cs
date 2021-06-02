////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows;
using System.Linq;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using xray.editor.wpf_controls.property_editors;
using Cursors = System.Windows.Input.Cursors;
using KeyEventArgs = System.Windows.Input.KeyEventArgs;
using MenuItem = System.Windows.Controls.MenuItem;
using MouseEventArgs = System.Windows.Input.MouseEventArgs;
using Panel = System.Windows.Controls.Panel;

namespace xray.editor.wpf_controls.hypergraph
{
	/// <summary>
	/// Interaction logic for gipergraph_control.xaml
	/// </summary>
	public partial class hypergraph_control
	{

		#region | Initialize |


		public	hypergraph_control		( )		
		{
			InitializeComponent	( );

			m_field.hypergraph					= this;
			m_field.Children.Remove				( m_new_link_path );
			m_field.Children.Remove				( m_selection_path );
			m_field.Children.Remove				( m_link_icon_container );
			m_link_icon_container.Items.Remove	( m_link_apply_icon );

			is_nodes_moveable		= true;
			is_panable				= true;
			is_zoomable				= true;
			is_linkable				= true;
			is_selectable			= true;

			m_item_editor_selector	= new item_editor_selector	( default_editors_lists.item_editors );
			m_value_editor_selector	= new value_editor_selector	( default_editors_lists.value_editors );

			left_limit				= Double.MinValue;
			top_limit				= Double.MinValue;
			right_limit				= Double.MaxValue;
			bottom_limit			= Double.MaxValue;
		}


		#endregion

		#region |   Events   |


		public event EventHandler<menu_event_args>					menu_item_click;
		public event Action											context_menu_opening;

		public event EventHandler<selection_event_args>				selecting;	
		public event EventHandler<selection_event_args>				deleting_selected;	

		public event EventHandler<node_event_args>					node_creating;	
		public event EventHandler<link_event_args>					link_creating;	
		public event EventHandler<links_event_args>					links_enabling;
		public event EventHandler<links_event_args>					links_disabling;
		public event EventHandler<can_link_event_args>				can_link;
		public event EventHandler<node_move_event_args>				node_moved;


		#endregion

		#region |   Fields   |


		private static readonly		Brush						m_background		= new SolidColorBrush( new Color{ A = 255, R =  155, G = 155, B = 155 } );

		private const				Double						c_epsilon			= 0.0001;
		private readonly			Dictionary<String, node>	m_nodes				= new Dictionary<String, node>( );
		private readonly			HashSet<node>				m_selected_nodes	= new HashSet<node>( );
		private readonly			Dictionary<String, link>	m_links				= new Dictionary<String, link>( );
		private readonly			HashSet<link>				m_selected_links	= new HashSet<link>( );
		private readonly			List<String>				m_selecting_nodes	= new List<String>( );
		private readonly			List<link_id>				m_selecting_links	= new List<link_id>( );
		private						HashSet<node>				m_nodes_to_invalidate = new HashSet<node>( );
		private						Point						m_scale				= new Point( 1, 1 );
		private						Int32						m_top_z_index;
		
		private						item_editor_selector		m_item_editor_selector;
		private						value_editor_selector		m_value_editor_selector;

		private						Boolean						m_pan_started;
		private						Boolean						m_zoom_started;
		private						Boolean						m_select_started;
		private						Boolean						m_nodes_move_started;

		private						Point						m_mouse_position;
		private						Point						m_mouse_start_position;

		private						Boolean						m_new_link_started;
		private						link_point					m_link_start_point;
		private						Boolean						m_is_apply_link_icon_visible;


		#endregion

		#region | Properties |


		public		ICollection<node>			selected_nodes			
		{
			get
			{
				return m_selected_nodes;
			}
		}
		public		ICollection<link>			selected_links			
		{
			get
			{
				return m_selected_links;
			}
		}
		public		Point						viewport_position		
		{
			get
			{
				return (Point)m_field_scroller.offset;
			}
			set
			{
				m_field_scroller.offset = (Vector)value;
			}
		}
		public		Point						viewport_scale			
		{
			get
			{
				return m_scale;
			}
			set
			{
				m_scale = value;
				if( m_scale.X < c_epsilon )	m_scale.X = c_epsilon;
				if( m_scale.Y < c_epsilon )	m_scale.Y = c_epsilon;
				m_field.LayoutTransform = new ScaleTransform( m_scale.X, m_scale.Y );
			}
		}
		public		Vector						viewport_origin			
		{
			get
			{
				return m_field_scroller.origin;
			}
			set
			{
				m_field_scroller.origin = value;
			}
		}
		public		field						field					
		{
			get
			{
				return m_field;
			}
		}
		public		i_command_engine			command_engine			
		{
			get;set;
		}

		public		item_editor_selector		item_editor_selector	
		{
			get
			{
				return m_item_editor_selector;
			}
		}
		public		value_editor_selector		value_editor_selector	
		{
			get
			{
				return m_value_editor_selector;
			}
		}

		public		Boolean						need_expand_to_field	
		{
			get;set;
		}
		public		pan_direction				pan_direction			
		{
			get;set;
		}
		public		Boolean						is_grid_enabled			
		{
			get
			{
				return m_field.is_grid_enabled;
			}
			set
			{
				m_field.is_grid_enabled = value;
			}
		}
		public		Boolean						is_nodes_moveable		
		{
			get;set;
		}
		public		Boolean						is_panable				
		{
			get;set;
		}
		public		Boolean						is_zoomable				
		{
			get;set;
		}
		public		Boolean						is_linkable				
		{
			get;set;
		}
		public		Boolean						is_selectable			
		{
			get;set;
		}
		public		Boolean						is_fix_min_field_size	
		{
			get 
			{
				return m_field.is_fix_min_size_by_parent;
			}
			set 
			{
				m_field.is_fix_min_size_by_parent = value; 
				InvalidateVisual( );
			}
		}
		public		Size						field_bounds_inflate	
		{
			get 
			{
				return m_field.bounds_infalte_amount;
			}
			set 
			{
				m_field.bounds_infalte_amount = value; 
				InvalidateVisual( );
			}
		}
		public		Double						left_limit				
		{
			get;set;
		}
		public		Double						top_limit				
		{
			get;set;
		}
		public		Double						right_limit				
		{
			get;set;
		}
		public		Double						bottom_limit			
		{
			get;set;
		}

		public		Boolean						remove_menu_item_enabled				
		{
			get
			{
				return cm_remove.Visibility == Visibility.Visible;
			}
			set
			{
				cm_remove.Visibility = (value) ? Visibility.Visible : Visibility.Collapsed;
			}
		}
		public		Boolean						enable_disable_link_menu_item_enabled	
		{
			get
			{
				return cm_enable_disable_link.Visibility == Visibility.Visible;
			}
			set
			{
				cm_enable_disable_link.Visibility = (value) ? Visibility.Visible : Visibility.Collapsed;
			}
		}
		public		Dictionary<String, node>	nodes					
		{
			get
			{
				return m_nodes;
			}
		}
		public		Dictionary<String, link>	links					
		{
			get
			{
				return m_links;
			}
		}


		#endregion

		#region |   Methods  |


		private				void			field_scroller_size_changed		( Object sender, SizeChangedEventArgs e	)			
		{
			m_field.InvalidateMeasure( );
		}
		private				void			mouse_button_down				( Object sender, MouseButtonEventArgs e )			
		{
			if( is_selectable && e.LeftButton == MouseButtonState.Pressed )
			{
				m_select_started		= true;
				m_mouse_position		= Mouse.GetPosition( m_field );
				m_mouse_start_position	= Mouse.GetPosition( m_field );
				CaptureMouse			( );
				m_selection_rect.Rect		= new Rect( m_mouse_start_position, m_mouse_start_position + new Vector( 1, 1 ) );
				if( m_selection_path.Parent == null )
					m_field.Children.Add( m_selection_path );
				e.Handled = true;
			}
		}
		private				void			preview_mouse_button_down		( Object sender, MouseButtonEventArgs e )			
		{
			//Focus( );
			m_mouse_position	= Mouse.GetPosition( this );

			if ( is_panable && e.ChangedButton == MouseButton.Middle && Keyboard.PrimaryDevice.IsKeyDown(Key.LeftAlt))
			{
				m_pan_started		= true;
				CaptureMouse		( );
				Cursor				= Cursors.SizeAll;
				e.Handled			= true;
			}
			else if( is_zoomable && e.ChangedButton == MouseButton.Right && Keyboard.PrimaryDevice.IsKeyDown(Key.LeftAlt))
			{
				m_zoom_started		= true;
				CaptureMouse		( );
				Cursor				= Cursors.SizeAll;
				e.Handled			= true;
			}
			else if( e.ChangedButton == MouseButton.Right )
			{
				m_mouse_position		= Mouse.GetPosition( m_field );
				var rect				= new Rect( m_mouse_position, new Size( 1, 1 ) );
				m_selection_rect.Rect	= rect;
				m_selecting_links.Clear( );
				m_selecting_nodes.Clear( );
				VisualTreeHelper.HitTest( m_field, null, selecting_visual, new GeometryHitTestParameters( m_selection_rect ) );

				var need_raise_event	= m_selecting_nodes.Select( node_id => m_nodes[node_id] ).Any( selecting_node => !m_selected_nodes.Contains( selecting_node ) );

				if( need_raise_event )
					on_node_selecting( new selection_event_args( m_selecting_nodes, m_selecting_links ) );
			}
		}
		private				void			preview_mouse_move				( Object sender, MouseEventArgs e )					
		{
			if ( m_pan_started )
			{
				var new_mouse_position	= Mouse.GetPosition( this );
				var mouse_offset		= (Point)( new_mouse_position - m_mouse_position );
				m_mouse_position		= new_mouse_position;

				if( pan_direction == pan_direction.vertical )
					mouse_offset.X = 0;

				if( pan_direction == pan_direction.horizontal )
					mouse_offset.Y = 0;

				viewport_position		-= (Vector)mouse_offset;
				e.Handled				= true;
				return;
			}
			if( m_zoom_started )
			{
				var new_mouse_position	= Mouse.GetPosition( this );
				var mouse_offset		= new_mouse_position - m_mouse_position;
				m_mouse_position		= new_mouse_position;

				var mouse_start			= (Vector)Mouse.GetPosition( m_field );

				mouse_offset			/= 100;
				mouse_offset.Y			= mouse_offset.X;
				viewport_scale			+= mouse_offset;

				var mouse_end			= new Vector( mouse_start.X * ( 1+mouse_offset.X), mouse_start.Y * (1+mouse_offset.Y) );
				viewport_position		+= mouse_end - mouse_start;
				e.Handled				= true;
				return;
			}
			if( m_new_link_started )
			{
				var pt = Mouse.GetPosition( m_field );
				m_new_link_line.EndPoint = pt;
				m_link_icon_container.SetValue( Canvas.LeftProperty, pt.X );
				m_link_icon_container.SetValue( Canvas.TopProperty, pt.Y );

				Object elem				= m_field.InputHitTest( Mouse.GetPosition( m_field ) );
				if( elem is Ellipse )
				{
					var place =  ( (Ellipse)elem ).Parent as link_point;
					if( place != null )
					{
						if( can_link_impl( place, m_link_start_point )  )
							show_link_apply( );
						else
							show_link_cancel( );
					}
				}
				else
					show_link_cancel( );
				e.Handled				= true;

				return;
			}
			if( m_select_started )
			{
				var new_position		= Mouse.GetPosition( m_field );
				m_selection_rect.Rect	= new Rect( m_mouse_start_position, new_position );
				e.Handled				= true;

				return;
			}
			if( m_nodes_move_started )
			{
				var new_mouse_position	= Mouse.GetPosition( this );
				var mouse_offset		= new_mouse_position - m_mouse_position;
				m_mouse_position		= new_mouse_position;

				move_selected_nodes		( mouse_offset );

				e.Handled				= true;

				return;
			}
		}
		private				void			preview_mouse_button_up			( Object sender, MouseButtonEventArgs e )			
		{
			if( m_pan_started || m_zoom_started )
			{
				m_pan_started		= false;
				m_zoom_started		= false;
				ReleaseMouseCapture	( );
				Cursor				= null;
				e.Handled			= true;
			}
			if( m_new_link_started )
			{
				m_new_link_started		= false;
				ReleaseMouseCapture		( );
				m_field.Children.Remove	( m_new_link_path );
				m_field.Children.Remove	( m_link_icon_container );
				VisualTreeHelper.HitTest( m_field, null,  result =>
				{
					if( result.VisualHit is Ellipse )
					{
						var place =  ( (Ellipse)result.VisualHit ).Parent as link_point;
						if( place != null )
						{
							place.refresh_field_position( );

							var destination	= place;
							var source		= m_link_start_point;

							if( !can_link_impl( source, destination ) )
								return HitTestResultBehavior.Stop;

							on_link_creating( 
								new link_event_args(
									new link_id( source.node.id, source.id, destination.node.id, destination.id ),
									source,
									destination
								 )
							);

							return HitTestResultBehavior.Stop;
						}
					}
					return HitTestResultBehavior.Continue;
				}, new PointHitTestParameters( Mouse.GetPosition( m_field ) ) );
				e.Handled			= true;
			}
			if( m_select_started )
			{
				m_select_started		= false;
				ReleaseMouseCapture		( );
				m_field.Children.Remove	( m_selection_path );
				var new_position		= Mouse.GetPosition( m_field );
				var rect				= new Rect( m_mouse_start_position, new_position );
				if( rect.Width == 0 )	rect.Width = 1;
				if( rect.Height == 0 )	rect.Height = 1;
				m_selection_rect.Rect	= rect;
				m_selecting_links.Clear( );
				m_selecting_nodes.Clear( );
				VisualTreeHelper.HitTest( m_field, null, selecting_visual, new GeometryHitTestParameters( m_selection_rect ) );
				on_node_selecting( new selection_event_args( m_selecting_nodes, m_selecting_links ) );
				e.Handled				= true;
			}
			if( m_nodes_move_started )
			{
				m_nodes_move_started	= false;
				ReleaseMouseCapture		( );
				e.Handled				= true;
			}
		}
		private				void			preview_key_down				( Object sender, KeyEventArgs e )					
		{
			if( e.Key == Key.F )
			{
				e.Handled							= true;
				m_field_scroller.offset				= new Vector( 0, 0 );
				viewport_scale						= new Point( 1, 1 );

				m_field_scroller.InvalidateArrange	( );
				m_field.InvalidateArrange			( );
				m_field.InvalidateVisual			( );
			}
		}
		private				void			show_link_apply					( )													
		{
			if( !m_is_apply_link_icon_visible )
			{
				m_link_icon_container.Items.Add		( m_link_apply_icon );
				m_link_icon_container.Items.Remove	( m_link_cancel_icon );
				m_is_apply_link_icon_visible		= true;
			}
		}
		private				void			show_link_cancel				( )													
		{
			if( m_is_apply_link_icon_visible )
			{
				m_link_icon_container.Items.Remove	( m_link_apply_icon );
				m_link_icon_container.Items.Add		( m_link_cancel_icon );
				m_is_apply_link_icon_visible		= false;
			}
		}
		private				void			context_menu_opened				( Object sender, RoutedEventArgs e )				
		{
			if( context_menu_opening != null )
				context_menu_opening( );

			cm_remove.IsEnabled					= m_selected_links.Count != 0 || m_selected_nodes.Count != 0;
			cm_enable_disable_link.IsEnabled	= m_selected_links.Count != 0;
			cm_enable_disable_link.Header		= ( m_selected_links.Any( link => link.is_enabled ) ) ? "Disable Link" : "Enable Link";
		}
		private				void			context_menu_remove_click		( Object sender, RoutedEventArgs e )				
		{
			cmd_delete_selected( );
		}
		private				void			enable_disable_link_click		( Object sender, RoutedEventArgs e )				
		{
			if( !m_selected_links.Any( link => link.is_enabled ) )
				on_links_enabling( new links_event_args( m_selected_links.Select( link => link.id.id ).ToList( ) ) );
			else
				on_links_disabling( new links_event_args( m_selected_links.Select( link => link.id.id ).ToList( ) ) );
		}

		private				HitTestResultBehavior selecting_visual			( HitTestResult result )							
		{
			if ( result.VisualHit is Path )
			{
				var link = ((Path)result.VisualHit).DataContext as link;
				if( link != null && !m_selecting_links.Contains( link.id ) )
					m_selecting_links.Add( link.id );

				return HitTestResultBehavior.Continue;
			}
			var node_obj = node.get_parent_node( result.VisualHit );
			if( node_obj != null && !m_selecting_nodes.Contains( node_obj.id ) )
			{
				m_selecting_nodes.Add(node_obj.id );
			}

			return HitTestResultBehavior.Continue;
		}
		private				void			context_item_click				( Object sender, RoutedEventArgs e )				
		{
			if( menu_item_click == null )
				return;

			var item		= (MenuItem)sender;
			menu_item_click	( this, new menu_event_args( item, item.Tag.ToString( ), item.Header.ToString( ) ) );
		}
		
		private				void			add_link						( link link )										
		{
			link.hypergraph			= this;
			m_links.Add				( link.id.id, link );
			m_field.Children.Add	( link );
		}
		private				void			remove_link						( link link )										
		{
			m_links.Remove			( link.id.id );
			m_field.Children.Remove	( link );
		}
		private				void			add_node						( node node )										
		{
			node.hypergraph			= this;
			node.SetValue			( Panel.ZIndexProperty, m_top_z_index );
			m_nodes.Add				( node.id, node );
			m_field.Children.Add	( node );
			m_top_z_index++;
		}
		private				void			remove_node						( node node )										
		{
			if ( selected_nodes.Contains( node ) )
			{
				selected_nodes.Remove(node);
			}
			node.destruct				( );
			m_nodes.Remove			( node.id );
			m_field.Children.Remove	( node );
		}

		private				Boolean			can_link_impl					( link_point first, link_point second )				
		{
			if( can_link != null )
			{
				var args	= new can_link_event_args( first, second );
				can_link	( this, args );

				return args.can_link;
			}
			
			return true;
		}
		private				link			link							( link_point source, link_point destination )		
		{
			return			link( new link_id( source.node.id, source.id, destination.node.id, destination.id ), source, destination );
		}
		private				link			link							( link_id link_id, link_point source, link_point destination )	
		{
			var link					= new link			( source, destination );
			link.id						= link_id;
			source.add_link				( link );
			destination.add_link		( link );
			add_link					( link );

			return link;
		}
		private				void			unlink							( link link )										
		{
			var input	= link.source;
			var output	= link.destination;

			input.remove_link	( link );
			output.remove_link	( link );
			remove_link			( link );
		}
		private				void			on_node_selecting_impl			( List<String> selecting_nodes, List<link_id> selecting_links )			
		{
			if( Keyboard.PrimaryDevice.IsKeyDown( Key.LeftCtrl ) )
			{
				foreach ( var id in selecting_nodes )
				{
					var node = get_node( id );
					if( node.is_selected )
						deselect_node( id );
					else
						select_node( id );
				}

				foreach ( var id in selecting_links )
				{
					var lnk = get_link( id );
					if( lnk.is_selected )
						deselect_link( id );
					else
						select_link( id );
				}
			}
			else
			{
				if(selecting_nodes.Count==1 && m_selected_nodes.Contains(get_node(selecting_nodes[0])))
					return; // for multi move;

				deselect_all_nodes();
				deselect_all_links();
				foreach ( var id in selecting_nodes )
					select_node( id );

				foreach ( var id in selecting_links )
					select_link( id );
			}
		}

		protected			void			on_node_selecting				( selection_event_args e )							
		{
			if( selecting!= null )
				selecting( this, e );
			else
				on_node_selecting_impl(e.selected_node_ids, e.selected_link_ids);
		}
		protected			void			on_deleting_selected			( selection_event_args e )							
		{
			if( deleting_selected!= null )
			{
				deleting_selected( this, e );
				m_selected_nodes.Clear();
				m_selected_links.Clear();
			}
		}
		protected			void			on_node_creating				( node_event_args e )								
		{
			if( node_creating!= null )
				node_creating( this, e );
		}
		protected			void			on_link_creating				( link_event_args e )								
		{
			if( link_creating != null )
				link_creating( this, e );
		}
		protected			void			on_links_enabling				( links_event_args e )								
		{
			if( links_enabling!= null )
				links_enabling( this, e );
		}
		protected			void			on_links_disabling				( links_event_args e )								
		{
			if( links_disabling != null )
				links_disabling( this, e );
		}
		internal			void			on_node_moved					( Vector offset )									
		{
			if( node_moved != null )
			{
				List<node_move_args> lst = new List<node_move_args>();
				foreach(var node in m_selected_nodes)
					lst.Add(new node_move_args(node, offset));

				node_moved( this, new node_move_event_args(lst) );
			}
		}
		internal			void			move_selected_nodes				( Vector offset )									
		{
			foreach( var node in m_selected_nodes )
			{
				if ( !node.is_movable )
					return;

				node.position		-= offset;
				node.is_node_moved	= true;

				if( node.position.X < left_limit )
					node.position = new Point( left_limit, node.position.Y );

				if( node.position.Y < top_limit )
					node.position = new Point( node.position.X, top_limit );

				if( node.position.X + node.ActualWidth > right_limit )
					node.position = new Point( right_limit - node.ActualWidth, node.position.Y );

				if( node.position.Y + node.ActualHeight > bottom_limit )
					node.position = new Point( node.position.X, bottom_limit - node.ActualHeight );
			}
		}
		protected override	void			OnRender						( DrawingContext drawing_context )					
		{
			if( m_nodes_to_invalidate.Count > 0 )
			{
				foreach ( var node in m_nodes_to_invalidate.OfType<properties_node>( ) )
					node.update_link_points_positions( );
				
				m_nodes_to_invalidate.Clear( );
			}

			base.OnRender( drawing_context );

			SnapsToDevicePixels = true;
			drawing_context.DrawRectangle( m_background, null, new Rect( 0, 0, ActualWidth, ActualHeight ) );
		}

		internal			void			start_nodes_moveing				( )													
		{
			m_mouse_position			= Mouse.GetPosition( this );
			m_nodes_move_started		= true;
			CaptureMouse				( );
		}

		internal			void			start_link_creating				( link_point point )								
		{
			if( is_linkable  )
			{
				point.refresh_field_position	( );
				m_link_start_point				= point;
				m_new_link_started				= true;
				m_field.Children.Add			( m_new_link_path );
				m_field.Children.Add			( m_link_icon_container );
				m_new_link_line.StartPoint		= point.connection_point;
				CaptureMouse					( );
			}
		}
		internal			void			fire_node_selecting				( selection_event_args e )							
		{
			on_node_selecting( e );
		}

		internal			link_point		get_link_point					( String node_id, String link_id )					
		{
			var node						= m_nodes[node_id];
			return node.get_link_point		( link_id );
		}
		internal			void			recalculate_field				( node n )											
		{
			m_nodes_to_invalidate.Add(n);
			m_field.InvalidateMeasure( );
		}

		internal			void			select_link_impl				( link link )										
		{
			m_selected_links.Add( link );
		}
		internal			void			deselect_link_impl				( link link )										
		{
			m_selected_links.Remove( link );
		}

		public				void			deselect_all_links				( )													
		{
			foreach( var link in m_selected_links.ToArray( ))
				link.is_selected = false;

			m_selected_links.Clear( );
		}
		public				void			deselect_all_nodes				( )													
		{
			foreach( var node in m_selected_nodes.ToArray( ) )
				node.is_selected = false;

			m_selected_nodes.Clear( );
		}

		public				void			cmd_delete_selected				( )													
		{
			on_deleting_selected ( new selection_event_args( m_selected_nodes.Select( node => node.id ).ToList( ), m_selected_links.Select( link => link.id ).ToList( ) ) );
		}
		public				void			cmd_enable_selected_links		( )													
		{
			on_links_enabling( new links_event_args( m_selected_links.Where( link => !link.is_enabled ).Select( link => link.id.id ).ToList( ) ) );
		}
		public				void			cmd_disable_selected_links		( )													
		{
			on_links_disabling( new links_event_args( m_selected_links.Where( link => link.is_enabled ).Select( link => link.id.id ).ToList( ) ) );
		}
		public				MenuItem		add_context_menu_item			( String item_name )								
		{
			var path_parts = item_name.Split('/');

			var items = m_context_menu.Items;
			MenuItem item = null;

			foreach( var part in path_parts )
			{
				var some_part	= part;
				item			= items.Cast<MenuItem>( ).FirstOrDefault( itm => itm.Header.ToString() == some_part );
				if( item == null )
				{
					item		= new MenuItem { Header = part };
					items.Add	( item );
				}
				items		= item.Items;
			}

			Debug.Assert( item != null );

			item.Tag	= item_name;
			item.Click	+= context_item_click;

			return item; 
		}

		public				node			create_node						( String id )										
		{
			var node		= new node( this ) { id = id };
			add_node		( node );
			node.position	= TranslatePoint( m_mouse_position, m_field );
			return node;
		}
		public				properties_node	create_properties_node			( String id )										
		{
			var node		= new properties_node( this ) { id = id };
			add_node		( node );
			node.position	= TranslatePoint( m_mouse_position, m_field );
			return node;
		}
		public				properties_node	create_node						( String id, Object source )						
		{
			var node = new properties_node( this, source ) { id = id };
			add_node( node );
			node.position = TranslatePoint( m_mouse_position, m_field );
			return node;
		}
		public				properties_node	create_node						( String id, Object source, Point position )		
		{
			var node = new properties_node( this, source ) { id = id };
			add_node( node );
			node.position = position;
			return node;
		}
		public				void			remove_node						( String node_id )									
		{
			remove_node( m_nodes[node_id] );
		}
		public				void			remove_nodes					( IEnumerable<String> node_ids )					
		{
			foreach( var node_id in node_ids )
			{
				remove_node( m_nodes[node_id] );
			}
		}
		public				link			create_link						( String output_node_id, String input_node_id, String output_link_id, String input_link_id )			
		{
			return link			( get_link_point( output_node_id, output_link_id ), get_link_point( input_node_id, input_link_id ) );
		}
		public				link			create_link						( link_id link_id )									
		{
			return link			( link_id, get_link_point( link_id.output_node_id, link_id.output_link_point_id ), get_link_point( link_id.input_node_id, link_id.input_link_point_id ) );
		}
		public				link			create_link						( property_descriptor descr, properties_node properties_node )									
		{
			var link_point = ((property)descr.owner_property).property_view.output_link_point;
			var link_id  = new link_id( link_point.node.id, link_point.id, properties_node.id, "input" );
			return link			( link_id, get_link_point( link_id.output_node_id, link_id.output_link_point_id ), get_link_point( link_id.input_node_id, link_id.input_link_point_id ) );
		}
		public				void			remove_link						( link_id link_id )									
		{
			unlink	( m_links[link_id.id] );
		}
		public				void			remove_link						( String output_node_id, String input_node_id, String output_link_id, String input_link_id )			
		{
			unlink	( m_links[ new link_id( output_node_id, output_link_id, input_node_id, input_link_id ).id ] );
		}
		public				void			set_selected_nodes				( List<String> nodes_ids )							
		{
			foreach( var node in m_selected_nodes )
				deselect_node( node.id );

			if( nodes_ids == null )
				return;

			foreach( var id in nodes_ids )
				select_node( id );
		}
		public				void			set_selected_nodes				( List<node> selecting_nodes )								
		{
			foreach( var node in m_selected_nodes )
				deselect_node( node.id );

			if( selecting_nodes == null )
				return;

			foreach( var node in selecting_nodes )
				select_node( node.id );
		}
		public				void			select_node						( String node_id )									
		{
			if ( !m_nodes[node_id].is_selectable )
				return;
			m_selected_nodes.Add			( m_nodes[node_id] );
			m_nodes[node_id].SetValue		( Panel.ZIndexProperty, ++m_top_z_index );
			m_nodes[node_id].is_selected	= true;
		}
		public				void			deselect_node					( String node_id )									
		{
			m_selected_nodes.Remove			( m_nodes[node_id] );
			m_nodes[node_id].is_selected	= false;
		}
		public				void			select_link						( link_id link )									
		{
			m_links[link.id].is_selected = true;
		}
		public				void			deselect_link					( link_id link )									
		{
			m_links[link.id].is_selected	= false;
		}
		public				void			select_all_links				( )													
		{
			foreach( var pair in m_links )
				pair.Value.is_selected = true;
		}
		public				void			select_all_nodes				( )													
		{
			foreach( var pair in m_nodes )
				pair.Value.is_selected = true;
		}

		public				node			get_node						( String node_id )										
		{
			return m_nodes[node_id];
		}
		public				link			get_link						( link_id link_id )										
		{
			return m_links[link_id.id];
		}


		#endregion

	}
}
