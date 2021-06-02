////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Data;
using System.Collections;
using System.Windows.Controls;
using System.Windows.Shapes;
using System.Windows.Media;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for collection_editor.xaml
	/// </summary>
	public partial class IList_collection_editor
	{
		public				IList_collection_editor			( )										
		{
			InitializeComponent();

			DataContextChanged += delegate
			{
				if( DataContext == null )
					return;

				m_property						= ((property)DataContext);
				m_property.owner_editor			= this;
				m_property.is_collection		= true;
				m_property.is_expandable_item	= true;

				if( item_editor.parent_container != null )
				{
					item_editor.parent_container.expand_visibility		= Visibility.Visible;
					item_editor.parent_container.fill_sub_properties	= fill_sub_properties;
					if(!m_property.is_read_only)
						item_editor.parent_container.decorate_container	= decorate_container;
				}
			};
		}

		private		Int32	m_last_sub_properties_count;

		private				void	add_click				( Object sender, RoutedEventArgs e )	
		{
			var collection_property		= ((property)((FrameworkElement)sender).DataContext);
			var property				= new property
			{
				name = "item",
				is_collection_item = true,
				property_parent = collection_property
			};

			foreach ( IList list in collection_property.values )
			{
				var generic_types  = list.GetType( ).GetGenericArguments( );
				if( generic_types != null && generic_types.Length > 0 )
					list.Add( Activator.CreateInstance( generic_types[0] ) );
				else
					list.Add( new Object( ) );
				
				property.descriptors.Add( new item_property_descriptor( property.name, list.Count-1, list));
				property.property_owners.Add(collection_property.value);
			}

			collection_property.sub_properties.Add(property);
		}
		private				void	fill_sub_properties		( )										
		{
			if( m_property.sub_properties == null || m_property.sub_properties.Count == 0 )
			{
				m_property.sub_properties = property_extractor.extract( m_property.values, m_property, m_property.extract_settings );
				m_last_sub_properties_count = m_property.sub_properties.Count;
			}
		}
		private				void	decorate_container		( expandable_items_control container )	
		{
			var button		= new Button {
  		  		Width		= 11,
  		  		Height		= 11,
  		  		HorizontalContentAlignment	= HorizontalAlignment.Center,
  		  		VerticalContentAlignment	= VerticalAlignment.Center,
  		  		Content		= new Path {
					Data				= (Geometry)FindResource( "remove_button_path_geometry" ),
					Stroke				= new SolidColorBrush( Colors.Black ),
					SnapsToDevicePixels	= false
				}
  		  	};

			button.Click	+= remove_item_click;
			container.add_right_pocket_inner( button );
		}
		private				void	remove_item_click		( Object sender, RoutedEventArgs e )	
		{
			remove_sub_property( (property)( (Button)sender ).DataContext );
		}

		internal override	void	remove_sub_property		( property sub_property )				
		{
			property to_delete_property = sub_property;

			int count = to_delete_property.property_owners.Count;
			for (int i = 0; i < count; ++i)
			{
				((IList)to_delete_property.property_owners[i]).RemoveAt(
					((item_property_descriptor)to_delete_property.descriptors[i]).item_index
				);
			}

			var properties = to_delete_property.property_parent.sub_properties;
			Int32 index = properties.IndexOf(to_delete_property);
			
			for (int i = index + 1; i < properties.Count; ++i)
			{
				foreach (item_property_descriptor desc in properties[i].descriptors)
				{
					desc.decrease_item_index();
				}
			}

			properties.RemoveAt(index);
			update( );
		}
		public override		void	update					()										
		{
			if( item_editor.parent_container != null && m_property.value != null && 
				m_last_sub_properties_count != ((IList)m_property.value).Count )
				item_editor.parent_container.reset_sub_properties	( );
		}
	}

	internal class collection_editor_is_fixed_size_checker : IValueConverter	
	{
		#region IValueConverter Members

		public object Convert(object value, Type target_type, object parameter, System.Globalization.CultureInfo culture)
		{
			if( value == null )
				return Visibility.Hidden;
			if( value.GetType( ).IsAssignableFrom( typeof(IList) ) )
				return (((IList)value).IsFixedSize)?Visibility.Hidden:Visibility.Visible;
			return Visibility.Hidden;
		}

		public object ConvertBack(object value, Type target_type, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}

		#endregion
	}
}
