////////////////////////////////////////////////////////////////////////////
//	Created		: 11.07.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.control_containers;
using xray.editor.wpf_controls.helpers;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.item
{
	public class control_container_editor: item_editor_base
	{
		public				control_container_editor	( )		
		{
			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property = (property)DataContext;

				create_controls	( );
				update			( );
			};
		}

		public				void					create_controls	( )
		{
			var is_first_panel			= true;
			Panel	wpf_panel			= null;
			control_container first_container = null;

			foreach ( var attr in m_property.descriptors
					.Select	( desc => (control_container_attribute) desc.Attributes[typeof (control_container_attribute)] )
					.Where	( attr => attr != null ) )
			{
				var container		= attr.m_control_container;
				
				if( is_first_panel )
				{
					first_container = container;
					wpf_panel		= container.generate_panel( );
					is_first_panel	= false;
					continue;
				}

				container.merge_to( first_container );
			}
			Content = wpf_panel;
		}

		public override		void					update			( )										
		{
			((UIElement)Content).IsEnabled = !m_property.is_read_only;
		}
	}
}
