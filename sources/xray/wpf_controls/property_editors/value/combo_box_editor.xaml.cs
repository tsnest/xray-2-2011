////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Windows;
using System.Windows.Input;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	partial class combo_box_editor
	{
		public	combo_box_editor	( )					
		{
			InitializeComponent();

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property				= (property)DataContext;
				m_items_attribute		= (combo_box_items_attribute)m_property.descriptors[0].Attributes[typeof(combo_box_items_attribute)];
			
				if( m_property.is_multiple_values )
				{
					m_combo_box.IsEnabled = true;
					if( m_property.is_read_only )
					    m_combo_box.IsEnabled = false;
				}
			};
		}

		private const	String							c_multivalues_item = "<many>";
		private			combo_box_items_attribute		m_items_attribute;
		private			Boolean							m_last_set_many;
		private			Object							m_last_set_item;
		private			Boolean							m_initialized;
		private			Boolean							m_updateing;

		public event	Action							selection_changed;

		public override void	update								( )												
		{
			m_updateing = true;

			if( need_refill	( ) )
				fill_combo_box		( );

			if( need_update_value( ) )
				select_current_item	( );

			m_updateing = false;
		}

		private			void    user_control_loaded					( Object sender,  RoutedEventArgs e )			
		{
			if ( m_initialized )
				return;

			if( m_items_attribute == null )
				return;

			clear_combo_box	( );
			fill_combo_box	( );
			select_current_item ( );

			m_combo_box.SelectionChanged += combo_box_selected_item_changed;
			m_combo_box.PreviewMouseDown += combo_box_preview_mouse_down;

			m_initialized = true;
		}
		private			void	clear_combo_box						( )												
		{
			m_combo_box.Items.Clear( );
		}
		private			void	fill_combo_box						( )												
		{
			if ( m_items_attribute.items_count_func != null )
			{
				m_combo_box.Items.Clear( );
				var count	= m_items_attribute.items_count_func( );
				for ( var i = 0; i < count; ++i )
					m_combo_box.Items.Add( m_items_attribute.get_item_func( i ) );
			}
			else if ( m_items_attribute.get_items != null )
			{
				foreach ( var item in m_items_attribute.get_items( m_items_attribute.argument ) )
					m_combo_box.Items.Add( item );
			}
			else if ( m_combo_box.Items.Count == 0 )
			{
				foreach ( var item in m_items_attribute.items )
					m_combo_box.Items.Add( item );
			}
		}
		private			void	select_current_item				( )												
		{
			if( m_property.is_multiple_values )
			{
				m_combo_box.Items.Add		( c_multivalues_item );
				m_combo_box.SelectedItem	= c_multivalues_item;
			}
			else if( m_items_attribute.is_associative )
			{
				var index					= m_items_attribute.values.IndexOf( m_property.value );
				m_combo_box.SelectedIndex	= index;
			}
			else if( m_property.type == typeof(String) )
			{
	            m_combo_box.SelectedItem	= m_property.value;
			}
			else
			{
				m_combo_box.SelectedIndex	= (Int32)m_property.value;
			}
		}
		private			Boolean	need_refill							( )												
		{
			if ( m_items_attribute.items_count_func != null )
			{
				var count	= m_items_attribute.items_count_func( );

				if( m_combo_box.Items.Count != count )
					return true;

				for ( var i = 0; i < count; ++i )
				{
					if( m_combo_box.Items[i].ToString( ) != m_items_attribute.get_item_func( i ) ) 
						return true;
				}
			}
			else if ( m_items_attribute.get_items != null )
			{
				var new_items = m_items_attribute.get_items( m_items_attribute.argument );

				if( m_combo_box.Items.Count != new_items.Count( ) )
					return true;

				var i = 0;
				foreach ( var item in new_items )
				{
					if( m_combo_box.Items[i].ToString( ) != item ) 
						return true;
					++i;
				}
			}

			return false;
		}
		private			Boolean	need_update_value					( )												
		{
			if( m_property.is_multiple_values )
			{
				if( m_combo_box.SelectedItem.ToString( ) != c_multivalues_item )
					return true;
			}
			else if( m_items_attribute.is_associative )
			{
				var index					= m_items_attribute.values.IndexOf( m_property.value );
				if( m_combo_box.SelectedIndex != index )
					return true;
			}
			else if( m_property.type == typeof(String) )
			{
				if( m_combo_box.SelectedItem != m_property.value )
					return true;
			}
			else
			{
				if( m_combo_box.SelectedIndex != (Int32)m_property.value )
					return true;
			}

			return false;
		}
		private			void    combo_box_preview_mouse_down		( Object sender, MouseButtonEventArgs e )		
		{
			if( m_combo_box.IsDropDownOpen )
				return;

			m_last_set_item				= m_combo_box.SelectedItem;
			m_last_set_many				= (String)m_last_set_item == c_multivalues_item;
			
			if ( m_items_attribute.items_count_func != null )
			{
				m_combo_box.Items.Clear( );
				var count	= m_items_attribute.items_count_func( );
				for ( var i = 0; i < count; ++i )
					m_combo_box.Items.Add( m_items_attribute.get_item_func( i ) );
			}
			else if ( m_items_attribute.get_items != null )
			{
				m_combo_box.Items.Clear( );
				foreach ( var item in m_items_attribute.get_items( m_items_attribute.argument ) )
					m_combo_box.Items.Add( item );
			}
		}
        private			void    combo_box_selected_item_changed		( Object sender, RoutedEventArgs e )			
        {
			if( m_updateing )
				return;

			if( m_combo_box.SelectedItem == null || m_combo_box.SelectedItem.ToString( ) == c_multivalues_item )
				return;

			if( m_items_attribute.is_associative )
				m_property.value		= m_items_attribute.values[m_combo_box.SelectedIndex];
			else
				m_property.value		= m_property.type == typeof(String) ? m_combo_box.SelectedItem : m_combo_box.SelectedIndex;

			on_selection_changed	( );
        }
		private			void	combo_box_drop_down_opened			( Object sender, EventArgs e )					
		{
			m_combo_box.Items.Remove	( c_multivalues_item );
		}
		private			void	combo_box_drop_down_closed			( Object sender, EventArgs e )					
		{
			if( m_combo_box.SelectedItem == null )
			{
				if( m_last_set_many )
				{
					m_combo_box.Items.Add		( c_multivalues_item );
					m_combo_box.SelectedItem	= c_multivalues_item;
				}
				else
				{
					m_combo_box.SelectedItem = m_last_set_item;
				}
			}
		}

		private			void	on_selection_changed				( )												
		{
			if( selection_changed != null )
				selection_changed( );
		}
    }
}
