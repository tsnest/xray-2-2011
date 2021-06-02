////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System.Windows;

namespace xray.editor.wpf_controls.property_editors.item
{
	/// <summary>
	/// Interaction logic for material_stage_editor.xaml
	/// </summary>
	public class material_stage_editor: item_editor_base
	{
		static				material_stage_editor	( )												
		{
			DefaultStyleKeyProperty.OverrideMetadata( typeof( material_stage_editor ), new FrameworkPropertyMetadata( typeof(material_stage_editor) ) );
		}
		public				material_stage_editor	( )								 				
		{
			DataContextChanged					+=	delegate 
			{
				if( DataContext == null )
					return;

				m_property					= (property)DataContext;
				m_property.owner_editor		= this;

				parent_container.fill_sub_properties	= fill_sub_properties;
				parent_container.expand_visibility		= Visibility.Visible;
				parent_container.is_expanded			= true;

			};
		}

		private				void		fill_sub_properties		( )									
		{
			m_property.sub_properties			= property_extractor.extract( m_property.values, m_property, m_property.extract_settings );
		}
	}
}