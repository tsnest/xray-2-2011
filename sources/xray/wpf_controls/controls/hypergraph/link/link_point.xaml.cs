////////////////////////////////////////////////////////////////////////////
//	Created		: 06.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Reflection;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace xray.editor.wpf_controls.hypergraph
{
	/// <summary>
	/// Interaction logic for property_link_place.xaml
	/// </summary>
	public partial class link_point: INotifyPropertyChanged
	{

		internal static void  initialize( )
		{
			 _info = typeof(Dispatcher).GetField( "_disableProcessingCount", BindingFlags.NonPublic | BindingFlags.Instance );
		}

		private static FieldInfo _info;

		public event		PropertyChangedEventHandler		PropertyChanged;

		public				link_point		( )						
		{
			InitializeComponent( );
			Loaded += ( o, e ) => 
			{
				if ( !DesignerProperties.GetIsInDesignMode( this ) )
					update_field_position( );
			};
		}

		private				String			m_id;
		private				Point			m_absolute_point_position;
		private				Boolean			m_is_connected;
		private readonly	List<link>		m_links	= new List<link>( );

		public				String			id							
		{
			get
			{
				return m_id;
			}
			set
			{
				if( m_id != value )
				{
					if( m_id != null && node != null )
						node.remove_link_point( m_id );

					m_id = value;

					if( m_id != null && node != null )
						node.add_link_point( m_id, this );
				}
			}
		}
		public				node			node						
		{
			get;set;
		}
		public				List<link>		links						
		{
			get{ return m_links; }
		}
		public				Object			tag							
		{
			get;set;
		}

		public				Boolean			is_connected				
		{
			get { return m_is_connected; }
			set { m_is_connected = value; }
		}
		internal			Point			connection_point			
		{
			get 
			{
				return m_absolute_point_position;
			}
		}

		private				void			link_place_mouse_down		( Object sender, MouseButtonEventArgs e )	
		{
			node.hypergraph.start_link_creating( this );
			e.Handled = true;
		}
		protected			void			on_property_changed			( String property_name )					
		{
			if( PropertyChanged != null )
				PropertyChanged( this, new PropertyChangedEventArgs( property_name ) );
		}
		protected override	void			OnPropertyChanged			( DependencyPropertyChangedEventArgs e )	
		{
			if( e.Property.Name == "parent_node" )
			{
				if( node != null )
					node.remove_link_point( id );

				node = (node)e.NewValue;
				if( id != null && node != null )
					node.add_link_point( id, this );
			}

			base.OnPropertyChanged( e );
		}

		internal			void			refresh_field_position		( )											
		{
			m_absolute_point_position = TranslatePoint( new Point( ActualWidth / 2, ActualHeight / 2 ), node.hypergraph.field );
			on_property_changed( "field_position" );
		}
		internal			void			update_field_position		( )											
		{
			if( !m_is_connected )
				return;

			if( (Int32)_info.GetValue( Dispatcher.CurrentDispatcher ) == 0 )
				Dispatcher.CurrentDispatcher.Invoke( DispatcherPriority.Loaded, new Action( refresh_field_position ));
			
			refresh_field_position( );
		}
		
		public				void			add_link					( link link )								
		{
			m_links.Add		( link );
			m_is_connected	= true;
		}
		public				void			remove_link					( link link )								
		{
			m_links.Remove	( link );
			m_is_connected	= m_links.Count != 0;
		}

	}
}
