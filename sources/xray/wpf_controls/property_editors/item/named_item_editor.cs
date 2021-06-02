////////////////////////////////////////////////////////////////////////////
//	Created		: 01.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System.Windows;
using xray.editor.wpf_controls.property_editors.value;

namespace xray.editor.wpf_controls.property_editors.item
{
	/// <summary>
	/// Interaction logic for named_item_editor.xaml
	/// </summary>
	public class named_item_editor: item_editor_base
	{
		static				named_item_editor	( )		
		{
			DefaultStyleKeyProperty.OverrideMetadata( typeof( named_item_editor ), new FrameworkPropertyMetadata( typeof(named_item_editor) ) );
		}
		public				named_item_editor	( )		
		{
			DataContextChanged += delegate
			{
				if( DataContext == null || !( DataContext is property ) )
					return;

				m_property					= (property)DataContext;
				m_property.owner_editor		= this;

				if( m_property.inner_properties != null )
				{
					foreach( var inner_prop in m_property.inner_properties )
					{
						var editor				= value_editor_selector.select_editor( inner_prop );
						editor.item_editor		= this;
						parent_container.add_right_pocket_inner( editor );
						editor.DataContext		= inner_prop;
					}
				}

				{
					var editor			= value_editor_selector.select_editor( m_property );
					editor.DataContext	= null;
					editor.item_editor	= this;
					value_editor		= editor;
					editor.DataContext	= m_property;
				}
			};
		}

		public static readonly	DependencyProperty	value_editorProperty		= DependencyProperty.Register( "value_editor", typeof( value_editor_base ), typeof( named_item_editor ), new PropertyMetadata( null ) );
		public					value_editor_base	value_editor				
		{
			get
			{
				return (value_editor_base)GetValue( value_editorProperty );
			}
			set
			{
				SetValue( value_editorProperty, value );
			}
		}

		public override			void				update						( )		
		{
			value_editor.update( );
		}
	}
}
