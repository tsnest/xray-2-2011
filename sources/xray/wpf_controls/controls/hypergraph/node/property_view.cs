////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using xray.editor.wpf_controls.property_editors;
using xray.editor.wpf_controls.property_editors.item;

namespace xray.editor.wpf_controls.hypergraph
{
	/// <summary>
	/// Interaction logic for property.xaml
	/// </summary>
	public class property_view: property_items_control, INotifyPropertyChanged
	{
		public		property_view	( property prop, item_editor_selector item_editor_selector, value_editor_selector value_editor_selector )
			: base( prop, item_editor_selector, value_editor_selector, 0 )
		{
			hypergraph_property		= prop;
			id						= prop.name;
			node					= hypergraph_property.node;
			prop.property_view		= this;
			is_create_properties	= true;
			
			if( prop.input_enabled )
			{
				m_input_link_point = new link_point
				{
					id				= prop.name + "input",
					node			= node,
					tag				= ( prop.descriptor is property_descriptor ) ? ( (property_descriptor)prop.descriptor ).tag : null
				};
				node.set_parent_node( m_input_link_point, node );
			}

			if( prop.output_enabled )
			{
				m_output_link_point = new link_point
				{
					id				= prop.name + "output",
					node			= node,
					tag				= ( prop.descriptor is property_descriptor ) ? ( (property_descriptor)prop.descriptor ).tag : null
				};
				node.set_parent_node( m_output_link_point, node );
			}

			if( prop.sub_properties == null || prop.sub_properties.Count == 0 )
				expand_visibility = Visibility.Hidden;

			m_item_editor					= m_item_editor_selector.select_editor( hypergraph_property );
			m_item_editor.value_editor_selector = m_value_editor_selector;
			m_item_editor.DataContext		= null;
			m_item_editor.parent_container	= this;
			Header							= m_item_editor;
			m_item_editor.DataContext		= hypergraph_property;
		}

		private readonly	item_editor_base					m_item_editor;

		private readonly	link_point							m_input_link_point;
		private readonly	link_point							m_output_link_point;

		private readonly	Dictionary<String, property_view>	m_property_views	= new Dictionary<String, property_view>( );
		
		internal			Dictionary<String, property_view>	properties	
		{
			get
			{
				return m_property_views;
			}
		}

		public				node			node						
		{
			get;set;
		}
		public				property		hypergraph_property			
		{
			get;set;
		}
		public				String			id							
		{
			get;set;
		}
		public				String			property_path				
		{
			get 
			{
				var ret_path		= id;
				var property_view	= this;

				while( property_view.Parent is property_view )
				{
					property_view	= (property_view)property_view.Parent;
					ret_path		= property_view.id + "/" + ret_path;
				}

				return ret_path;
			}
		}

		public				link_point		input_link_point			
		{
			get
			{
				if( m_input_link_point == null )
				{

				}
				return m_input_link_point;
			}
		}
		public				link_point		output_link_point			
		{
			get
			{
				return m_output_link_point;
			}
		}

		public event		PropertyChangedEventHandler		PropertyChanged;

		protected			void							on_property_changed				( String property_name )			
		{
			if( PropertyChanged != null )
				PropertyChanged( this, new PropertyChangedEventArgs( property_name ) );
		}

		internal override	property_items_control			create_container_for_property	( property_editors.property prop )	
		{
			var view	= new property_view( (property)prop, m_item_editor_selector, m_value_editor_selector );
			m_property_views.Add( prop.name, view );
			return view;
		}
		internal override	void							remove_sub_container			( Int32 index )						
		{
			m_property_views.Remove		( ( (property_view)Items[index] ).m_property.name );
			base.remove_sub_container	( index );

			if( m_property.values[0] is property_container_collection )
			{
				var template = ( (property_container_collection)m_property.values[0] ).item_name_template;
				for ( var i = index; i < m_property_views.Count; ++i )
				{
					var prop_view				= (property_view)Items[i];
					prop_view.m_property.name	= String.Format( template, i );

					m_property_views.Remove		( prop_view.id );
					prop_view.id				= prop_view.m_property.name;
					m_property_views.Add		( prop_view.id,prop_view );
				}
			}
		}

		internal			void							update_links					( )									
		{
			if( m_input_link_point != null )
				m_input_link_point.update_field_position	( );

			if( m_output_link_point != null )
				m_output_link_point.update_field_position	( );

			foreach( var property_view in m_property_views.Values )
				property_view.update_links( );
		}
		internal			link_point						get_link_place					( String link_place_key )			
		{
			return link_place_key == "input" ? m_input_link_point : m_output_link_point;
		}
		
		public override		void							OnApplyTemplate					( )									
		{
			base.OnApplyTemplate	( );

			left_pocket.MinWidth		= 14;
			right_pocket.MinWidth		= 14;

			if( m_input_link_point != null )
				add_left_pocket_outer	( m_input_link_point );

			if( m_output_link_point != null )
				add_right_pocket_outer	( m_output_link_point );
		}
		public override		void							clear_sub_items					( )									
		{
			m_property_views.Clear( );
			base.clear_sub_items( );
		}
	}
}