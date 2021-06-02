////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;

namespace xray.editor.wpf_controls.hypergraph
{
	/// <summary>
	/// Interaction logic for node.xaml
	/// </summary>
	public partial class properties_node: INotifyPropertyChanged
	{

		#region | Initialize |


		internal				properties_node			( hypergraph_control hypergraph )										
		{
			InitializeComponent( );
			type_id				= "node";
			this.hypergraph		= hypergraph;
					
			var style = new  Style( );

			m_normal_border_setter		= new Setter( Border.BorderBrushProperty, new SolidColorBrush( Colors.LightBlue ) );
			m_selected_border_setter	= new Setter( Border.BorderBrushProperty, new SolidColorBrush( Colors.Yellow ) );
			m_highlighted_border_setter = new Setter( Border.BorderBrushProperty, new SolidColorBrush( Colors.GreenYellow ) );

			style.Triggers.Add(
				new DataTrigger
				{ 
					Binding		= new Binding( "is_selected" ){Source = this},
					Value		= true,
					Setters		= { m_selected_border_setter } 
				} 
			);
			style.Triggers.Add(
				new Trigger
				{ 
					Property	= IsMouseOverProperty,
					Value		= true,
					Setters		= { m_highlighted_border_setter } 
				} 
			);

			style.Setters.Add( m_normal_border_setter );
			style.Setters.Add( new Setter( Border.BorderThicknessProperty, new Thickness( 3 ) ) );

			m_node_border.Style = style;

		}
		internal				properties_node			( hypergraph_control hypergraph, Object source ):this ( hypergraph )	
		{
			source_object		= source;
		}


		#endregion

		#region |   Fields   |


		private				Boolean								m_is_node_captured;
		private				Vector								m_capture_mouse_offset;
		private				Point								m_last_node_position;
		private				String								m_name;
		private				Setter								m_normal_border_setter;
		private				Setter								m_selected_border_setter;
		private				Setter								m_highlighted_border_setter;


		#endregion

		#region | Properties |


		public static readonly	DependencyProperty	has_expanded_property = DependencyProperty.Register("has_expanded", typeof( Boolean ), typeof( properties_node ) );
		public static readonly	DependencyProperty	is_expanded_property = DependencyProperty.Register("is_expanded", typeof( Boolean ), typeof( properties_node ), new PropertyMetadata(true) );

		public			Object					source_object			
		{
			get
			{
				return m_property_container.source_object;
			}
			set
			{
				m_property_container.source_object = value;
			}
		}
		public			Boolean					is_expanded				
		{
			get
			{
				return (Boolean)GetValue( is_expanded_property );
			}
			set
			{
				SetValue( is_expanded_property, value );
			}
		}
		public			Boolean					has_expanded			
		{
			get
			{
				return (Boolean)GetValue( has_expanded_property );
			}
			set
			{
				SetValue( has_expanded_property, value );
			}
		}

		public			String					type_id					
		{
			get;set;
		}
		public			String					name					
		{
			get
			{
				return m_name;
			}
			set
			{
				m_name = value;
				if( PropertyChanged != null )
					PropertyChanged( this, new PropertyChangedEventArgs( "name" ) );
			}
		}
		public			link_point				input_link_point		
		{
			get { return m_input_link_point; }
		}
		public			link_point				output_link_point		
		{
			get { return m_output_link_point; }
		}

		public			Boolean					input_link_place_enabled		
		{
			get
			{
				return m_input_link_point.Visibility == Visibility.Visible;
			}
			set
			{
				m_input_link_point.Visibility = (value) ? Visibility.Visible : Visibility.Collapsed;
			}
		}
		public			Boolean					output_link_place_enabled		
		{
			get
			{
				return m_output_link_point.Visibility == Visibility.Visible;
			}
			set
			{
				m_output_link_point.Visibility = (value) ? Visibility.Visible : Visibility.Collapsed;
			}
		}
		
		public			Brush					background				
		{
			get
			{
				return (Brush)m_node_border.GetValue( BackgroundProperty );
			}
			set
			{
				m_node_border.SetValue( BackgroundProperty, value );
			}
		}
		public			Brush					border_normal			
		{
			get
			{
				return (Brush)m_normal_border_setter.Value;
			}
			set
			{
				m_normal_border_setter.Value = value;
			}
		}
		public			Brush					border_selected			
		{
			get
			{
				return (Brush)m_selected_border_setter.Value;
			}
			set
			{
				m_selected_border_setter.Value = value;
			}
		}
		public			Brush					border_highlighted		
		{
			get
			{
				return (Brush)m_highlighted_border_setter.Value;
			}
			set
			{
				m_highlighted_border_setter.Value = value;
			}
		}

		public			Double					header_panel_height		
		{
			get
			{
				return m_header_panel.Height;
			}
			set
			{
				m_header_panel.Height = value;
			}
		}
		public			Double					border_min_width		
		{
			get
			{
				return m_node_border.MinWidth;
			}
			set
			{
				m_node_border.MinWidth = value;
			}
		}
		public			Object					above_header_content	
		{
			get
			{
				return m_above_header_content.Content;
			}
			set
			{
				m_above_header_content.Content = value;
			}
		}

		public			DockPanel				content_panel			
		{
			get
			{
				return m_content_panel;
			}
		}

		
		#endregion

		#region |   Events   |


		public event PropertyChangedEventHandler PropertyChanged;


		#endregion

		#region |   Methods  |


		private		void				header_mouse_down				( Object sender, MouseButtonEventArgs e )	
		{
			if( hypergraph.is_nodes_moveable && e.LeftButton == MouseButtonState.Pressed )
			{
				if ( !is_selected )
				{
					hypergraph.deselect_all_nodes();
					hypergraph.deselect_all_links();
					hypergraph.select_node( id );
				}
				m_last_node_position		= position;
				m_is_node_captured			= true;
				m_capture_mouse_offset		= (Vector)Mouse.GetPosition( this );
			 	m_header.CaptureMouse		( );
				e.Handled					= true;
				is_node_moved				= false;
			}
		}
		private		void				header_mouse_move				( Object sender, MouseEventArgs e )			
		{
			if( m_is_node_captured )
			{
				var parent_mouse_position		= Mouse.GetPosition( (FrameworkElement)Parent );
				Vector offset					= position - (parent_mouse_position - m_capture_mouse_offset);
				if (offset == new Vector(0,0))
					return;

				hypergraph.move_selected_nodes		( offset );
			}
		}
		private		void				header_mouse_up					( Object sender, MouseButtonEventArgs e	)	
		{
			if( m_is_node_captured && e.LeftButton == MouseButtonState.Released )
			{
				//hypergraph.run_command		( new node_move_command( hypergraph, this, m_last_node_position, position ) );
				Vector offset					= new Vector(m_last_node_position.X-position.X, m_last_node_position.Y-position.Y);
				hypergraph.on_node_moved		( offset );
				m_header.ReleaseMouseCapture	( );
				m_is_node_captured				= false;
			}
		}
		
		protected override void			OnPropertyChanged				( DependencyPropertyChangedEventArgs e )	
		{
			if( e.Property.Name == "Tag" )
			{
				input_link_point.tag	= e.NewValue;
				output_link_point.tag	= e.NewValue;
			}
			base.OnPropertyChanged( e );
		}

		internal	property_view		get_property_view				( String view_key )							
		{
			return m_property_container.property_views[view_key];
		}
		public		void				remove							( )											
		{
			for( var i = input_link_point.links.Count - 1; i >= 0; --i )
				hypergraph.remove_link( input_link_point.links[i].id );

			for( var i = output_link_point.links.Count - 1; i >= 0; --i )
				hypergraph.remove_link( output_link_point.links[i].id );

		    foreach( var prop_view in m_property_container.property_views.Values )
		    {
				if( prop_view.input_link_point != null )
				{
					for( var i = prop_view.input_link_point.links.Count-1; i >= 0; --i )
						hypergraph.remove_link( prop_view.input_link_point.links[i].id ); 
				}
				if( prop_view.output_link_point != null )
				{
			        for( var i = prop_view.output_link_point.links.Count-1; i >= 0; --i )
			            hypergraph.remove_link( prop_view.output_link_point.links[i].id );
				}
		    }
		}
		public		void				update_properties				( )											
		{
			m_property_container.update_properties( );
		}


		#endregion

	}
}
