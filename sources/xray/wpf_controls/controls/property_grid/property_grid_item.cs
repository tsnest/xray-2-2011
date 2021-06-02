////////////////////////////////////////////////////////////////////////////
//	Created		: 10.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Input;
using xray.editor.wpf_controls.property_editors;

namespace xray.editor.wpf_controls.property_grid
{
	public class property_grid_item: property_items_control
	{
		internal static void initialize ( )	
		{
			DefaultStyleKeyProperty.OverrideMetadata( typeof( property_grid_item ), new FrameworkPropertyMetadata( typeof(property_grid_item) ) );
		}

		internal		property_grid_item		( property prop, item_editor_selector item_editor_selector, value_editor_selector value_editor_selector, Int32 level, hierarchy_node node ): base( prop, item_editor_selector, value_editor_selector, level )
		{
			is_create_properties	= true;
			m_hierarchy_node		= node;

			if( prop.sub_properties == null || prop.sub_properties.Count == 0 )
				expand_visibility = Visibility.Hidden;

			var editor							= m_item_editor_selector.select_editor( m_property );
			editor.value_editor_selector		= m_value_editor_selector;
			editor.parent_container				= this;
			editor.DataContext					= null;
			item_editor							= editor;
			editor.DataContext					= m_property;

			is_expanded				= is_expanded || m_hierarchy_node != null;

			PreviewMouseDown		+= preview_mouse_down;
		}

		private readonly	hierarchy_node			m_hierarchy_node;
		private				FrameworkElement		m_item_property_editor;
		private				Point					m_mouse_down_point;

		private				void					preview_mouse_down				( Object sender, MouseButtonEventArgs e )	
		{
			m_mouse_down_point = e.GetPosition( this );
		}
		private				void					mouse_down						( Object sender, MouseButtonEventArgs e )		
		{
			if( e.ChangedButton == MouseButton.Left )
			{
				if( Keyboard.PrimaryDevice.IsKeyDown( Key.LeftCtrl ) )
					( (property)m_property ).is_selected = !( (property)m_property ).is_selected;
				else
					( (property)m_property ).owner_property_grid.deselect_all_properties( );

				e.Handled = true;
			}
		}
		private				void					mouse_move						( Object sender, MouseEventArgs e )				
		{
			if( e.LeftButton == MouseButtonState.Pressed &&  ( (Vector)m_mouse_down_point - (Vector)e.GetPosition( this ) ).LengthSquared > 4 )
				( (property)m_property ).is_selected = true;
		}

		internal override	property_items_control	create_container_for_property	( property_editors.property prop )				
		{
			return new property_grid_item(
				(property)prop,
				m_item_editor_selector,
				m_value_editor_selector,
				level + 1,
				( m_hierarchy_node != null ) ? m_hierarchy_node.sub_nodes.Find( node => node.name == prop.name ) : null
			);
		}

		public override		void					OnApplyTemplate					( )												
		{
			m_item_property_editor	= (FrameworkElement)GetTemplateChild( "PART_item_property_editor" );
			Debug.Assert( m_item_property_editor != null );
			m_item_property_editor.MouseDown += mouse_down;
			m_item_property_editor.MouseMove += mouse_move;

			base.OnApplyTemplate( );
		}
	}
}
