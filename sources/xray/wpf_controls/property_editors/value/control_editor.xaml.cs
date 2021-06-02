////////////////////////////////////////////////////////////////////////////
//	Created		: 27.10.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Linq;
using System.Windows;
using System.Windows.Controls;
using xray.editor.wpf_controls.control_containers;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for button_editor.xaml
	/// </summary>
	public partial class control_editor
	{
		public				control_editor	( )		
		{
			InitializeComponent( );

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property = (property)DataContext;

				update		( );
			};
		}

		public override		void					update			( )										
		{
			m_panel.Children.Clear( );
			var is_first = true;
			FrameworkElement	control			= null;
			control				first_control	= null;

			foreach ( var attr in m_property.descriptors
					.Select	( desc => (control_attribute) desc.Attributes[typeof (control_attribute)] )
					.Where	( attr => attr != null ) )
			{
				if( is_first )
				{
					first_control			= attr.m_control;
					control					= attr.m_control.generate_control( );
					DockPanel.SetDock		( control, Dock.Left );
					m_panel.Children.Add	( control );
					is_first				= false;
				}
				else
				{
					if( attr.m_control.same_as( first_control ) )
						attr.m_control.merge_to( control );
				}
			}
			
			m_panel.IsEnabled = !m_property.is_read_only;
		}
	}
}
