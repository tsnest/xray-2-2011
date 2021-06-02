////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Windows;
using xray.editor.wpf_controls.curve_editor;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for float_curve_editor.xaml
	/// </summary>
	public partial class float_curve_editor
	{
		public				float_curve_editor			( )			
		{
			InitializeComponent( );
			m_curve_panel.float_curve_changed		+= float_curve_changed;

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property	= (property)DataContext;
				
				fill_curves	( );

				var attribute = (float_curve_editor_attribute)m_property.descriptor.Attributes[typeof(float_curve_editor_attribute)];
				if( attribute != null )
				{
					m_curve_panel.field_left_limit		= attribute.initial_left_limit;
					m_curve_panel.field_top_limit		= attribute.initial_top_limit;
					m_curve_panel.field_right_limit		= attribute.initial_right_limit;
					m_curve_panel.field_bottom_limit	= attribute.initial_bottom_limit;

					attribute.left_limit_setter			= limit => m_curve_panel.field_left_limit		= limit;
					attribute.top_limit_setter			= limit => m_curve_panel.field_top_limit		= limit;
					attribute.right_limit_setter		= limit => m_curve_panel.field_right_limit		= limit;
					attribute.bottom_limit_setter		= limit => m_curve_panel.field_bottom_limit		= limit;

					foreach( var template in attribute.templates )
						m_curve_panel.add_custom_template	( template );
				}

				m_curve_panel.is_read_only = m_property.is_read_only;
			};
		}

		private				void		big_editor_click			( Object sender, RoutedEventArgs e )  		
		{
			var curve_editor_panel = new curve_editor_panel( );
			curve_editor_panel.float_curve_changed	+= float_curve_changed;
			curve_editor_panel.is_read_only			= m_property.is_read_only;

			var form								= utils.new_form_host( "Curve Editor", curve_editor_panel );
			form.Width								= 800;
			form.Height								= 600;

			curve_editor_panel.edited_curves		= m_curve_panel.edited_curves;
			m_curve_panel.IsEnabled					= false;
			big_editor.IsEnabled					= false;

			curve_editor_panel.field_left_limit		= m_curve_panel.field_left_limit;
			curve_editor_panel.field_top_limit		= m_curve_panel.field_top_limit;
			curve_editor_panel.field_right_limit	= m_curve_panel.field_right_limit;
			curve_editor_panel.field_bottom_limit	= m_curve_panel.field_bottom_limit;

			foreach( var template in m_curve_panel.custom_templates )
				curve_editor_panel.add_custom_template( template );

			form.Load += delegate 
			{
				curve_editor_panel.focus_curves( );
			};
			form.Closed += delegate 
			{
				m_curve_panel.edited_curves			= curve_editor_panel.edited_curves;
				m_curve_panel.LayoutUpdated			+= layout_updated_handler;
				m_curve_panel.IsEnabled				= true;
				big_editor.IsEnabled				= true;
			};
			form.Show( );
		}
		private				void		layout_updated_handler		( Object sender, EventArgs e )				
		{
			m_curve_panel.focus_curves	( );
			m_curve_panel.LayoutUpdated	-= layout_updated_handler;
		}
		private static		void		float_curve_changed			( float_curve curve )						
		{
			curve.fire_edit_completed( );
		}
		private				void		loaded						( Object sender, RoutedEventArgs e )		
		{
			m_curve_panel.focus_curves( );
		}
		private				void		fill_curves					( )											
		{
			var value	= m_property.value;

			if( value is float_curve )
				m_curve_panel.edited_curves		= new List<float_curve>{ (float_curve)value };
			else if( value is List<float_curve> )
				m_curve_panel.edited_curves		= (List<float_curve>)value;
		}

		public override		void		update						( )											
		{
			fill_curves	( );
		}
	}
}
