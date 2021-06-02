////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using xray.editor.wpf_controls.property_editors;
using xray.editor.wpf_controls.property_editors.item;
using xray.editor.wpf_controls.property_editors.value;

namespace xray.editor.wpf_controls.property_grid
{
	/// <summary>
	/// Interaction logic for PropertyGrid.xaml
	/// </summary>
	public partial class property_grid_control
	{
		#region | Initialize |


		public property_grid_control( ) 
		{
			m_properties			= new List<property>( );
			m_value_editors			= default_editors_lists.value_editors;
			m_item_editors			= default_editors_lists.item_editors;
			m_value_editor_selector	= new value_editor_selector( m_value_editors );
			m_item_editor_selector	= new item_editor_selector( m_item_editors, typeof(named_item_editor) );
			m_browsable_attributes	= new List<Attribute>();

			m_selection_rect			= new RectangleGeometry();
			m_selection_path			= new Path
			{
				Stroke						= Brushes.Black,
				StrokeDashArray 			= new DoubleCollection { 3, 2 },
				StrokeThickness 			= 1,
				Data						= m_selection_rect,
				SnapsToDevicePixels			= true
			};
			m_selection_path.SetValue( Panel.ZIndexProperty, -2000 );
			SnapsToDevicePixels			= true;
			selected_properties			= new List<property>( );
			remember_expanded_hierarchy = true;
			m_root_hierarchy_node					= new hierarchy_node{ name = "root" };

			InitializeComponent( );
		}


		#endregion

		#region |   Fields   |


		private readonly	Path					m_selection_path;
		private readonly	RectangleGeometry		m_selection_rect;

		private		Object[]						m_selected_objects;
		private		List<editor_picker>				m_value_editors;
		private		List<editor_picker>				m_item_editors;
		internal	value_editor_selector			m_value_editor_selector;
		internal	item_editor_selector			m_item_editor_selector;
		private		Boolean							m_read_only_mode;

		private		List<property>					m_properties;
		private		List<Attribute>					m_browsable_attributes;

		private		Boolean							m_auto_update;
		private		Boolean							m_is_focused;

		private		hierarchy_node					m_root_hierarchy_node;
		

		#endregion

		#region | Properties |


		internal	Boolean							setting_values					
		{
			get;
			private set;
		}
		internal	hierarchy_node					root_hierarchy_node				
		{
			get
			{
				return m_root_hierarchy_node;
			}
		}
		
		public		ListCollectionView				data_view						
		{
			get{return (ListCollectionView)CollectionViewSource.GetDefaultView(m_properties_panel.ItemsSource);}
		}
		public		Object							selected_object					
		{
			get 
			{
				if( m_selected_objects == null )
					return null;

				return m_selected_objects[0]; 
			}
			set 
			{
				selected_objects = (value != null) ? new[] { value }: null; 
			}
		}
		public		Object[]						selected_objects				
		{
			get
			{
				return m_selected_objects; 
			}
			set
			{
				m_selected_objects				= value;
				fill_properties_from_objects	( );

				if( data_view != null && data_view.GroupDescriptions != null )
					data_view.GroupDescriptions.Add	( new category_group_description( ) );
			}
		}
		public		Boolean							auto_update						
		{
			get { return m_auto_update; }
			set
			{
				m_auto_update = value;
				if ( m_auto_update )
					Dispatcher.Invoke( new Action( timer_update_properties ), TimeSpan.FromSeconds( 0.05 ), null );
			}
		}
		public		Boolean							read_only_mode					
		{
			get 
			{
				return m_read_only_mode; 
			}
			set 
			{
				var update = m_read_only_mode != value;
				m_read_only_mode = value; 
				if( update )
					reset();
			}
		}
		public		List<Attribute>					browsable_attributes			
		{
			get { return m_browsable_attributes;}
		}
		public		browsable_state					browsable_attribute_state		
		{
			get; set; 
		}
		public		Boolean							browsable_attribute_affects_children	
		{
			get; set; 
		}
		public		List<property>					selected_properties				
		{
			get;
			set;
		}
		public		Boolean							remember_expanded_hierarchy		
		{
			get;
			set;
		}


		#endregion

		#region |   Events   |


		public	event value_changed_event_handler	property_value_changed;
		public	event EventHandler					refresh_property_changed;


		#endregion

		#region |   Methods  |


		private static	void	invalidate_properties				( IEnumerable properties )						
		{
			if(properties == null)
				return;

			foreach(property property in properties)
			{
				property.is_valid = false;
				invalidate_properties( property.sub_properties );
			}
		}
		private			void	timer_update_properties				( )												
		{
			if ( m_is_focused || m_properties == null )
				return;
		
			update( );
			
			if ( m_auto_update )
				Dispatcher.Invoke( new Action( timer_update_properties ), TimeSpan.FromSeconds( 0.05 ), null );
		}
		private			void	fill_properties_from_objects		( )												
		{
			if( remember_expanded_hierarchy && m_properties_panel.Items.OfType<property_grid_item>( ).Count( ) > 0 )
			{
				m_root_hierarchy_node.sub_nodes.Clear	( );
				save_expanded_hierarchy					( m_properties_panel, m_root_hierarchy_node );
			}

			invalidate_properties			( m_properties );
			m_properties_panel.ItemsSource		= null;
			m_properties_panel.Items.Clear		( );

			if ( m_properties != null )
				m_properties.Clear				( );

			m_properties = ( selected_objects != null && selected_objects.Length != 0 ) ? new List<property>( ) : null;
            selected_properties.Clear();
			if ( m_properties == null )
				return;

			var settings = new property_extractor.settings
           	{
           		browsable_attribute_affects_children	= browsable_attribute_affects_children,
           		browsable_attribute_state				= browsable_attribute_state,
           		browsable_attributes					= m_browsable_attributes,
           		read_only_mode							= read_only_mode,
				create_prop_delegate					= ( ) => new property( this )
           	};

			m_properties				= property_extractor.extract<List<property>>( selected_objects, null, settings );
			m_properties_panel.DataContext = null;

			var grid_items				= new List<property_grid_item>( m_properties.Count );
			for( var i = 0; i < m_properties.Count; ++i )
			{
				var prop = m_properties[i];

				var item = new property_grid_item(
					prop, 
					m_item_editor_selector, 
					m_value_editor_selector, 
					0, 
					m_root_hierarchy_node.sub_nodes.Find( node => node.name == prop.name )
				);

				grid_items.Add				( item );
			}

			m_properties_panel.ItemsSource = grid_items;
		}
		private			void	this_ctrl_got_focus					( Object sender, RoutedEventArgs e )			
		{
			m_is_focused = true;
		}
		private			void	this_ctrl_lost_focus				( Object sender, RoutedEventArgs e )			
		{
			m_is_focused = false;
		}

		private			void	mouse_down							( Object sender, MouseButtonEventArgs e )		
		{
			deselect_all_properties( );
		}
		private static	void	save_expanded_hierarchy				( ItemsControl items, hierarchy_node node )		
		{
			foreach( var item in items.Items.OfType<property_grid_item>( ).Where( item => item.is_expanded ) )
			{
				var hierarchy_node			= new hierarchy_node{ name = item.name };
				node.sub_nodes.Add			( hierarchy_node );
				save_expanded_hierarchy		( item, hierarchy_node );
			}
		}

		public			void	deselect_all_properties				( )												
		{
			var count = selected_properties.Count;
			for( var i = count - 1; i >= 0; --i )
				selected_properties[i].is_selected = false;
		}
		internal		void	on_property_changed					( property prop, Object old_value )				
		{
			if( property_value_changed != null )
				property_value_changed( this, new value_changed_event_args( prop, old_value ) );
		}
		internal		void	on_refresh_property_changed			( )												
		{
			if( refresh_property_changed != null )
				refresh_property_changed( this, EventArgs.Empty );
		}

		public			void	set_properties_values				( Object value )								
		{
			setting_values = true;
			
			foreach ( var property in selected_properties )
			{
				try
				{
					if( property.owner_editor is property_container_editor ||
						( property.owner_editor is named_item_editor && ( (named_item_editor)property.owner_editor ).value_editor is property_container_editor ) )
					{
						var editor =
							(property.owner_editor is named_item_editor) ?
							(property_container_editor)( (named_item_editor)property.owner_editor ).value_editor:
							(property_container_editor)property.owner_editor;

						if( editor.inner_property != null )
							editor.inner_property.value = value;
					}
					else
						property.value = value;
				}
				catch( InvalidCastException )
				{
				}
			}
			setting_values = false;
		}
		public			void	reset								( )												
		{
			selected_objects = selected_objects;
		}
		public			void	update								( )												
		{
			foreach( property_items_control control in m_properties_panel.Items )
				control.update_hierarchy( );
		}
		public			void	remove_object_from_selection		( Object @object )								
		{
			if( ( (IList)m_selected_objects ).Contains( @object ) )
			{
				var count					= m_selected_objects.Length;
				var new_selected_objects	= new ArrayList( count - 1 );

				for( var i = 0; i < count; ++i )
				{
					if( m_selected_objects[i] == @object )
						continue;

					new_selected_objects.Add( m_selected_objects[i] );
				}
				
				selected_objects = new_selected_objects.ToArray( );
			}
		}


		#endregion

	}
}