////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace xray.editor.wpf_controls.hypergraph
{
	/// <summary>
	/// Interaction logic for edge.xaml
	/// </summary>
	public partial class link: INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		public link( )	
		{
			InitializeComponent		( );
			set_parent_link			( this, this );
			DataContext				= this;
		}
		public link( link_point source, link_point destination ): this( )
		{
			this.source	= source;
			this.destination	= destination;

			this.source.PropertyChanged		+= output_property_changed;
			this.destination.PropertyChanged		+= input_property_changed;

			output_property_changed	( this, new PropertyChangedEventArgs( "" ) );
			input_property_changed	( this, new PropertyChangedEventArgs( "" ) );
		}

		private		Boolean					m_is_selected;
		private		Boolean					m_is_enabled			= true;

		public static readonly DependencyProperty	parent_link_property = DependencyProperty.RegisterAttached("parent_link", typeof(link), typeof(link), new FrameworkPropertyMetadata( null, FrameworkPropertyMetadataOptions.Inherits ) );

		public		link_id					id						
		{
			get;set;
		}
		public		hypergraph_control		hypergraph				
		{
			get;set;
		}
		public		link_point				source					
		{
			get; private set; 
		}
		public		link_point				destination				
		{
			get; private set; 
		}
		public		Boolean					is_selected				
		{
			get 
			{
				return m_is_selected; 
			}
			set 
			{
				m_is_selected = value;
				
				if( m_is_selected )
					hypergraph.select_link_impl( this );
				else
					hypergraph.deselect_link_impl( this );

				on_property_changed( "is_selected" );
			}
		}
		public		Boolean					is_enabled				
		{
			get 
			{
				return m_is_enabled; 
			}
			set 
			{
				m_is_enabled = value; 
				on_property_changed( "is_enabled" );
			}
		}
		public		Point					start_point				
		{
			get
			{
				return source.connection_point;
			}
		}
		public		Point					end_point				
		{
			get
			{
				return destination.connection_point;
			}
		}
		public		Point					start_point_tangent		
		{
			get
			{
				return source.connection_point + new Vector( Math.Max( 50, ( destination.connection_point.X - source.connection_point.X ) / 3 ), 0 );
			}
		}
		public		Point					end_point_tangent		
		{
			get
			{
				return destination.connection_point - new Vector( Math.Max( 50, ( destination.connection_point.X - source.connection_point.X ) / 3 ), 0 );
			}
		}
		public		Point					arrow_point_1			
		{
			get
			{
				return destination.connection_point + new Vector(5, 0);
			}
		}
		public		Point					arrow_point_2			
		{
			get
			{
				return destination.connection_point - new Vector(5, 5);
			}
		}
		public		Point					arrow_point_3			
		{
			get
			{
				return destination.connection_point - new Vector(5, -5);
			}
		}

		public		Object					content					
		{
			get
			{
				return m_link_content.Content;
			}
			set
			{
				m_link_content.Content = value;
			}
		}
		
		public		Object					source_tag				
		{
			get
			{
				return source.tag;
			}
		}
		public		Object					destination_tag			
		{
			get
			{
				return destination.tag;
			}
		}

		public static	link				get_parent_link	( DependencyObject obj )						
		{
			return (link)obj.GetValue( parent_link_property );
		}
		public static	void				set_parent_link	( DependencyObject obj, link link )				
		{
			obj.SetValue( parent_link_property, link );
		}

		private		void		link_content_size_changed	( Object sender, SizeChangedEventArgs e )		
		{
			fix_content_position( );
		}
		private		void		output_property_changed		( Object sender, PropertyChangedEventArgs e )	
		{
			m_line_path.StartPoint	= start_point;
			m_line_segment.Point1	= start_point_tangent;
			m_line_segment.Point2	= end_point_tangent;

			fix_content_position( );
		}
		private		void		input_property_changed		( Object sender, PropertyChangedEventArgs e )	
		{
			m_line_segment.Point3	= end_point;
			m_line_segment.Point1	= start_point_tangent;
			m_line_segment.Point2	= end_point_tangent;
			
			m_arrow.StartPoint		= arrow_point_1;
			((LineSegment)m_arrow.Segments[0]).Point = arrow_point_1;
			((LineSegment)m_arrow.Segments[1]).Point = arrow_point_2;
			((LineSegment)m_arrow.Segments[2]).Point = arrow_point_3;

			fix_content_position( );
		}

		private		void		fix_content_position		( )									
		{
			Point tangent;
			Point content_position;
			m_path_geometry.GetPointAtFractionLength( 0.3, out content_position, out tangent );
			var half_conent_size	= new Vector( m_link_content.ActualWidth / 2, m_link_content.ActualHeight / 2 );
			content_position		-= half_conent_size;

			m_link_content.SetValue( Canvas.LeftProperty, content_position.X );
			m_link_content.SetValue( Canvas.TopProperty, content_position.Y );
		}

		protected	void		on_property_changed			( String property_name )			
		{
			if( PropertyChanged != null )
				PropertyChanged( this, new PropertyChangedEventArgs( property_name ) );
		}


	}
}
