////////////////////////////////////////////////////////////////////////////
//	Created		: 04.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Media;
using xray.editor.wpf_controls.color_picker;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for color_editor.xaml
	/// </summary>
	public partial class color_editor
	{
		#region | Initialize |


		public color_editor( )
		{
			InitializeComponent( );

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property				= (property)DataContext;

				var values				= m_property.values;

				m_color_helpers			= new color_helper[values.Length];
				var count				= values.Length;

				for( var i = 0; i < count; ++i )
				{
					m_color_helpers[i] = new color_helper(
						( values[i] is Color) ? (color_rgb)(Color)values[i] : (color_rgb)values[i],
						m_property
					);
					m_color_helpers[i].editor	= this;
				}
				
				if( m_property.is_multiple_values )
					m_multivalue_text_block.Text = "<many>";

				if( ( (property)DataContext ).is_read_only )
					m_pick_color_button.IsEnabled		= false;

				var desc = (DescriptionAttribute)m_property.descriptors[0].Attributes[ typeof( DescriptionAttribute ) ];
				m_multivalue_text_block.ToolTip = desc == null ? null : desc.Description;

				if( item_editor.parent_container != null )
				{
					item_editor.parent_container.expand_visibility		= Visibility.Visible;
					item_editor.parent_container.fill_sub_properties	= fill_sub_items;
				}
			};
		}


		#endregion

		#region |   Fields   |


		private		color_helper[]		m_color_helpers;


		#endregion

		#region |  Methods   |


		private				void	fill_sub_items				( )										
		{
			var objects					= new Object[m_color_helpers.Length];
			m_color_helpers.CopyTo		( objects, 0 );
			m_property.sub_properties	= property_extractor.extract( objects, m_property, m_property.extract_settings );
		}
		private				void	make_text_unvisible			( )										
		{
			m_multivalue_text_block.Text = "";
		}
		private				void	pick_color_button_click		( Object sender, RoutedEventArgs e )	
		{
			var dlg				= new dialog{
		  		selected_color = ( m_property.value != null )
					? ( m_property.value is Color ) ? (color_rgb)(Color) m_property.value : (color_rgb) m_property.value
		  			: new color_rgb(0, 0, 0, 0)
		  	};

			var result = dlg.ShowDialog( );

			if( result.HasValue && result.Value )
			{
				m_property.is_multiple_values	= false;
				m_multivalue_text_block.Text	= "";
			    m_property.value				= ( m_property.value is Color ) ? (Color)dlg.selected_color : (Object)dlg.selected_color;
				var count = m_color_helpers.Length;
				
				for( var i = 0; i < count; ++i )
					m_color_helpers[i].color		= ( m_property.value is Color ) ? (color_rgb)(Color)m_property.value : (color_rgb)m_property.value;

				if( m_property.sub_properties != null )
				{
					count = m_property.sub_properties.Count;
					for( var i = 0; i < count; ++i )
					{
						m_property.sub_properties[i].update_owner_editor( );
					}
				}
			}
		}

		public override		void	update						( )										
		{
			m_property.invalidate_value( );
		}

	
		#endregion

		#region | InnerTypes |


		private class color_helper
		{
			public	color_helper		( color_rgb color, property prop )		
			{
				this.color	= color;
				this.prop	= prop;
				editor		= null;
			}

			public	color_rgb		color;
			public	property		prop;
			public	color_editor	editor;

			[value_range_and_format_attribute(0, 1, 0.01, 2)]
			public	Double	a		
			{
				get
				{
					return color.a;
				}
				set
				{
					color.a	= value;
					set_color	(color);
				}
			}
			[value_range_and_format_attribute(0, 1, 0.01, 2)]
			public	Double	r		
			{
				get
				{
					return color.r;
				}
				set
				{
					color.r	= value;
					set_color	(color);
				}
			}
			[value_range_and_format_attribute(0, 1, 0.01, 2)]
			public	Double	g		
			{
				get
				{
					return color.g;
				}
				set
				{
					color.g	= value;
					set_color	(color);
				}
			}
			[value_range_and_format_attribute(0, 1, 0.01, 2)]
			public	Double	b		
			{
				get
				{
					return color.b;
				}
				set
				{
					color.b	= value;
					set_color	(color);
				}
			}

			private void	set_color	( color_rgb new_color )	
			{
				prop.is_multiple_values		= false;
				prop.value					= ( prop.value is Color ) ? (Color)new_color : (Object)new_color;
				editor.make_text_unvisible	( );
			}

		}


		#endregion
	}
}
