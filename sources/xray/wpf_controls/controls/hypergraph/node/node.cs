////////////////////////////////////////////////////////////////////////////
//	Created		: 03.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace xray.editor.wpf_controls.hypergraph
{
	public class node: ContentControl
	{

		#region | Initialize |


		public		node( )											
		{
			m_link_points		= new Dictionary<String, link_point>( );
			SizeChanged			+= node_size_changed;
			PreviewMouseDown	+= node_mouse_down;
			PreviewMouseUp		+= node_mouse_up;

			set_parent_node		( this, this );
		}

		public		node( hypergraph_control hypergraph ): this( )	
		{
			this.hypergraph		= hypergraph;
		}


		#endregion

		#region |   Fields   |


		private				String								m_id;
		private				Dictionary<String, link_point>		m_link_points;
		private				Boolean								m_is_node_moved;
		

		#endregion

		#region | Properties |


		public static readonly	DependencyProperty	is_selected_property = DependencyProperty.Register("is_selected", typeof( Boolean ), typeof( node ) );
		public static readonly	DependencyProperty	parent_node_property = DependencyProperty.RegisterAttached("parent_node", typeof(node), typeof(node), new FrameworkPropertyMetadata( null, FrameworkPropertyMetadataOptions.Inherits ) );

		
		public			String					id					
		{
			get
			{
				return m_id;
			}
			set
			{
				if( value == m_id )
					return;

				if ( m_id == null )
				{
					m_id = value;
					return;
				}

				if( hypergraph != null && hypergraph.nodes.ContainsKey( m_id ) )
					hypergraph.nodes.Remove( m_id );
				
				m_id = value;

				if( hypergraph != null )
					hypergraph.nodes.Add( m_id, this );

			}
		}
		public			hypergraph_control		hypergraph			
		{
			get;set;
		}
		public			Boolean					is_selected			
		{
			get
			{
				return (Boolean)GetValue( is_selected_property );
			}
			internal set
			{
				SetValue( is_selected_property, value );
			}
		}
		public			Point					position			
		{
			get
			{
				return new Point( (Double)GetValue( Canvas.LeftProperty ), (Double)GetValue( Canvas.TopProperty ) ); 
			}
			set
			{
				SetValue( Canvas.LeftProperty, value.X );
				SetValue( Canvas.TopProperty, value.Y );
				hypergraph.recalculate_field		( this );
				update_link_points_positions( );
			}
		}
		public			Boolean					is_movable			= true;
		public			Boolean					is_selectable		= true;
		public			Boolean					is_node_moved
		{
			get 
			{
				return m_is_node_moved; 
			}
			set 
			{
				m_is_node_moved = value; 
			}
		}

		#endregion

		#region | Delegates  |


		public		Func<Boolean>			is_header_captured_callback;
		

		#endregion

		#region |   Methods  |


		private			void				node_size_changed					( Object sender, SizeChangedEventArgs e )		
		{
			hypergraph.recalculate_field	( this );
			update_link_points_positions	( );
		}
		private			void				node_mouse_down						( Object sender, MouseButtonEventArgs e )		
		{
			if( hypergraph.is_nodes_moveable && e.LeftButton == MouseButtonState.Pressed )
			{
				if( is_header_captured_callback != null && is_header_captured_callback( ) )
				{
					hypergraph.start_nodes_moveing( );
					m_is_node_moved				= false;
					e.Handled					= true;
				}
			}
		}
		private			void				node_mouse_up						( Object sender, MouseButtonEventArgs e )		
		{
			if( !m_is_node_moved && is_selectable )
				hypergraph.fire_node_selecting( new selection_event_args( new List<String>{ id }, new List<link_id>( ) ) );
		}

		internal		void				update_link_points_positions		( )												
		{
			if( hypergraph == null )
				return;
			
			foreach( var link_point in m_link_points.Values )
				link_point.update_field_position( );
		}
		internal		void				destruct							( )												
		{
			if( m_link_points.Values.Any( link_point => link_point.is_connected ) )
				throw new InvalidOperationException( "Removing node must contains no linked link points. Unlink node before removing" );
		}

		public			void				add_link_point						( String link_id, link_point link_point )		
		{
			m_link_points.Add( link_id, link_point );
		}
		public			link_point			get_link_point						( String link_id )								
		{
			return m_link_points[link_id];
		}
		public			void				remove_link_point					( String link_id )								
		{
			m_link_points.Remove( link_id );
		}

		public static	node				get_parent_node						( DependencyObject obj )						
		{
			return (node)obj.GetValue( parent_node_property );
		}
		public static	void				set_parent_node						( DependencyObject obj, node node )			
		{
			obj.SetValue( parent_node_property, node );
		}


		#endregion

	}
}
