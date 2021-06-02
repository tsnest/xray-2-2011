////////////////////////////////////////////////////////////////////////////
//	Created		: 05.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using xray.editor.wpf_controls.property_editors.item;
using xray.editor.wpf_controls.property_editors.attributes;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.property_editors
{
	public class property_items_control: expandable_items_control
	{
		public property_items_control( property property, item_editor_selector item_editor_selector, value_editor_selector value_editor_selector, Int32 level )
		{
			m_level					= level;
			m_item_editor_selector	= item_editor_selector;
			m_value_editor_selector = value_editor_selector;
			m_property				= property;
			DataContext				= property;
		}

		private readonly	Int32								m_level;

		public				item_editor_selector				m_item_editor_selector;
		public				value_editor_selector				m_value_editor_selector;
		public				property							m_property;
		public				Boolean								is_create_properties;
		public				Action								fill_sub_properties;
		public				Action<property_items_control>		decorate_container;

		public				Int32								level									
		{
			get
			{
				return m_level;
			}
		}
		public				item_editor_base					item_editor								
		{
			get
			{
				return (item_editor_base)Header;
			}
			set
			{
				Header = value;
			}
		}
		public				String								name									
		{
			get
			{
				return m_property.name;
			}
		}

		public virtual		void					clear_sub_items					( )					
		{
			if( m_property.sub_properties != null )
			{
				foreach( var property in m_property.sub_properties )
					property.destroy( );

				m_property.sub_properties.Clear	( );
			}

			foreach( property_items_control container in Items )
			{
				container.m_property	= null;
				container.DataContext	= null;
				container.item_editor.DataContext = null;
			}

			Items.Clear						( );
		}
		public virtual		void					fill_sub_items					( )					
		{
			if( fill_sub_properties == null )
				return;
				
			fill_sub_properties( );

			foreach( var prop in m_property.sub_properties )
			{
				var container			= create_container_for_property( prop );
				Items.Add				( container );

				if( decorate_container != null )
					decorate_container	( container );
			}
		}
		public				void					reset_sub_properties			( )					
		{
			clear_sub_items	( );
			fill_sub_items	( );
		}

		protected override	void					on_expanded_changed				( )					
		{
			if( is_create_properties && Items.Count == 0 )
			{
				clear_sub_items	( );
				fill_sub_items	( );
			}
			base.on_expanded_changed( );
		}

		internal virtual	property_items_control	create_container_for_property	( property prop )	
		{
			return new property_items_control( prop, m_item_editor_selector, m_value_editor_selector, m_level + 1 );
		}
		internal virtual	void					remove_sub_container			( Int32 index )		
		{
			Items.RemoveAt( index );
		}

		internal			void					insert_container_for_property	( Int32 index, property property )	
		{
			var container			= create_container_for_property( property );
			Items.Insert			( index, container );
			container.ApplyTemplate	( );

			if( decorate_container != null )
				decorate_container	( container );
		}
		internal			void					update_hierarchy				( )					
		{
			item_editor.update( );

			foreach( property_items_control control in Items )
				control.update_hierarchy( );
		}

		public override		String					ToString						( )					
		{
			return name;
		}
	}
}
