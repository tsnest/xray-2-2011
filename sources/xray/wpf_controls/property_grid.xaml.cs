using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Collections;
using System.Timers;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for PropertyGrid.xaml
	/// </summary>
	public partial class property_grid : ContentControl
	{

		#region |   Events   |


		public event property_grid_value_changed_event_handler property_value_changed;
		public event property_grid_value_changed_event_handler sub_property_value_changed;


		#endregion

		#region | Initialize |


		public property_grid()
		{
			InitializeComponent();

			m_properties			= new ObservableCollection<property_grid_property>();
			m_editors				= new List<property_grid_editor>();
			m_item_editors			= new List<property_grid_editor>();
			m_editor_selector		= new property_grid_editor_selector(this);
			m_item_editor_selector	= new property_grid_item_editor_selector(this);
			m_browsable_attributes	= new List<Attribute>();

			//Add Standart Item Editors
			m_item_editors.Add(new property_grid_type_editor(typeof(TimeSpan), (DataTemplate)this.FindResource("color_range_editor")));
			
			//Add Standart Editors
			m_editors.Add(new property_grid_assignable_type_editor(typeof(ICollection), (DataTemplate)this.FindResource("collection_editor")));
			m_editors.Add(new property_grid_attribute_editor(typeof(property_grid_editors.combo_box_items_attribute), (DataTemplate)this.FindResource("combo_box_editor")));
			m_editors.Add(new property_grid_attribute_editor(typeof(property_grid_editors.external_editor_attribute), (DataTemplate)this.FindResource("external_editor")));
			m_editors.Add(new property_grid_attribute_editor(typeof(property_grid_editors.string_select_file_editor_attribute), (DataTemplate)this.FindResource("string_select_file_editor")));
			m_editors.Add(new property_grid_type_editor(typeof(Boolean), (DataTemplate)this.FindResource("Boolean_editor")));
			m_editors.Add(new property_grid_type_editor(typeof(Int32), (DataTemplate)this.FindResource("Int32_editor")));
			m_editors.Add(new property_grid_type_editor(typeof(String), (DataTemplate)this.FindResource("String_editor")));
			m_editors.Add(new property_grid_type_editor(typeof(Single), (DataTemplate)this.FindResource("Single_editor")));
			m_editors.Add(new property_grid_type_editor(typeof(Double), (DataTemplate)this.FindResource("Single_editor")));

			m_timer = new Timer();
			m_timer.Interval = 50;
			m_timer.Elapsed += m_timer_Elapsed;

		}

		static property_grid()
		{
			FrameworkPropertyMetadata metadata = new FrameworkPropertyMetadata(0, FrameworkPropertyMetadataOptions.Inherits);
			ItemIndentProperty = DependencyProperty.RegisterAttached("ItemIndent", typeof(Int32), typeof(property_grid), metadata);
		}

		[AttachedPropertyBrowsableForChildren]
		public static Int32 GetItemIndent(UIElement element)
		{
			return (Int32)element.GetValue(property_grid.ItemIndentProperty);
		}
		[AttachedPropertyBrowsableForChildren]
		public static void SetItemIndent(UIElement element, Int32 value)
		{
			element.SetValue(property_grid.ItemIndentProperty, value);
		}


		#endregion

		#region |   Fields   |


		private		Object[]						m_selected_objects;
		private		List<property_grid_editor>		m_editors;
		private		List<property_grid_editor>		m_item_editors;
		internal	property_grid_editor_selector	m_editor_selector;
		internal	property_grid_item_editor_selector	m_item_editor_selector;

		private		ObservableCollection<property_grid_property> m_properties;
		private		List<Attribute>					m_browsable_attributes;

		private		Timer							m_timer;
		private		Boolean							m_auto_update;
		private		Boolean							m_is_focused;

		private		Object							m_synk_object = new Object();
		

		#endregion

		#region | Properties |

		public static readonly DependencyProperty	ItemIndentProperty;
		public static IValueConverter				item_indent_increaser
		{
			get { return new item_indent_converter_class();}
		}


		public List<property_grid_editor>		editors					
		{
			get
			{
				return m_editors;
			}
		}
		public List<property_grid_editor>		item_editors			
		{
			get { return m_item_editors; }
		}
		public ListCollectionView				data_view				
		{
			get{return (ListCollectionView)CollectionViewSource.GetDefaultView(PropertiesPanel.ItemsSource);}
		}
		public ObservableCollection<property_grid_property>	properties	
		{
			get { return m_properties; }
			set { m_properties = value; }
		}
		public Object							selected_object			
		{
			get { return m_selected_objects[0]; }
			set { selected_objects = (value != null) ? new[] { value }: null; }
		}
		public Object[]							selected_objects		
		{
			get { return m_selected_objects; }
			set
			{
				lock (m_synk_object)
				{
					m_selected_objects = value;
					fill_properties_from_objects();
					if(data_view != null)
						data_view.GroupDescriptions.Add(new category_group_description());
				}
			}
		}
		public List<Attribute>					browsable_attributes	
		{
			get { return m_browsable_attributes;}
		}
		public Boolean							auto_update				
		{
			get { return m_auto_update; }
			set
			{
				m_auto_update = value;
				if (value)
					m_timer.Start();
				else
					m_timer.Stop();
			}
		}

		public browsable_state					browsable_attribute_state { get; set; }


		#endregion

		#region |  Methods   |

		private		void	m_timer_Elapsed					(object sender, ElapsedEventArgs e)
		{
			if (m_is_focused || m_properties == null)
				return;

			lock (m_synk_object)
			{
				foreach (var property in m_properties)
				{
					property.update();
				}
			}
		}
		private		Boolean	is_match_for_attributes			(PropertyDescriptor descriptor)					
		{
			switch(browsable_attribute_state)
			{
				case browsable_state.and:
					return descriptor.Attributes.Matches(m_browsable_attributes.ToArray());

				case browsable_state.or:
					foreach (var attribute in browsable_attributes)
					{
						if(descriptor.Attributes.Matches(attribute))
							return true;
					}
					break;
			}
			return false;
		}

		private		void	fill_properties_from_objects	()												
		{
			PropertiesPanel.ItemsSource = null;
			if ( m_properties != null )
				m_properties.Clear();
			m_properties = (selected_objects != null && selected_objects.Length != 0) ? new ObservableCollection<property_grid_property>() : null;
			if ( m_properties == null )
				return;

			m_properties = get_properties_from_objects(selected_objects);

			PropertiesPanel.ItemsSource = m_properties;
		}
		internal	void	on_property_changed				(property_grid_property prop, Object old_value)	
		{
			if(property_value_changed != null)
				property_value_changed(this, new property_grid_value_changed_event_args(prop, old_value));
		}
		internal	void	on_sub_property_changed			(property_grid_property prop, Object old_value)	
		{
			if (sub_property_value_changed != null)
				sub_property_value_changed(this, new property_grid_value_changed_event_args(prop, old_value));
		}
		public		void	reset							()												
		{
			var obj = PropertiesPanel.ItemsSource;
			PropertiesPanel.ItemsSource = null;
			PropertiesPanel.ItemsSource = obj;
		}

		private		void	expand_collapse_Click			(object sender, RoutedEventArgs e)				
		{
			Grid grid = (Grid)(sender as FrameworkElement).Parent;
			var ctrl_2 = (grid as FrameworkElement).Parent;
			var ctrl_3 = (ctrl_2 as FrameworkElement).Parent;
			var ctrl_4 = (ctrl_3 as FrameworkElement).Parent;
			var stack_panel = (StackPanel)(ctrl_4 as FrameworkElement).Parent;

			var border = (Border)stack_panel.Children[1];
			var sub_items = (ItemsControl)((Grid)border.Child).Children[0];
			var property_item = (property_grid_property)stack_panel.DataContext;

			if (sub_items.ItemsSource == null)
			{
				property_item.sub_properties = get_properties_from_objects(property_item.values);

				foreach (var prop in property_item.sub_properties)
					prop.property_parent = property_item;

				sub_items.ItemTemplate = (DataTemplate)FindResource("PropertyItemTemplate");
				sub_items.ItemsSource = property_item.sub_properties;
			}

			if (border.Visibility == Visibility.Collapsed)
			{
				property_item.is_expanded				= true;
				((Button)grid.Children[1]).Visibility	= Visibility.Visible;
				((Button)grid.Children[0]).Visibility	= Visibility.Collapsed;
				border.Visibility						= Visibility.Visible;
			}
			else
			{
				property_item.is_expanded				= false;
				((Button)grid.Children[1]).Visibility	= Visibility.Collapsed;
				((Button)grid.Children[0]).Visibility	= Visibility.Visible;
				border.Visibility						= Visibility.Collapsed;
			}
		}
		private		void	expand_collapse_group_Click		(object sender, RoutedEventArgs e)				
		{
			Grid grid = (Grid)(sender as FrameworkElement).Parent;
			var ctrl_2 = (grid as FrameworkElement).Parent;
			var ctrl_3 = (ctrl_2 as FrameworkElement).Parent;
			var stack_panel = (StackPanel)(ctrl_3 as FrameworkElement).Parent;
			
			var items_presenter = (ItemsPresenter)stack_panel.Children[1];
			
			if (items_presenter.Visibility == Visibility.Collapsed)
			{
				((Button)grid.Children[1]).Visibility	= Visibility.Visible;
				((Button)grid.Children[0]).Visibility	= Visibility.Collapsed;
				items_presenter.Visibility				= Visibility.Visible;
			}
			else
			{
				((Button)grid.Children[1]).Visibility	= Visibility.Collapsed;
				((Button)grid.Children[0]).Visibility	= Visibility.Visible;
				items_presenter.Visibility				= Visibility.Collapsed;
			}
		}
		private		void	delete_item_Click				(object sender, RoutedEventArgs e)				
		{
			property_grid_property to_delete_property = ((property_grid_property)((FrameworkElement)sender).DataContext);

			int count = to_delete_property.property_owners.Count;
			for (int i = 0; i < count; ++i)
			{
				((IList)to_delete_property.property_owners[i]).RemoveAt(
					((property_grid_item_property_descriptor)to_delete_property.descriptors[i]).item_index
				);
			}

			Int32 index = to_delete_property.property_parent.sub_properties.IndexOf(to_delete_property);
			var properties = to_delete_property.property_parent.sub_properties;
			for (int i = index+1; i < properties.Count; ++i)
			{
				foreach (property_grid_item_property_descriptor desc in properties[i].descriptors)
				{
					desc.decrease_item_index();
				}
			}

			to_delete_property.property_parent.sub_properties.RemoveAt(index);
		}
		private		void	this_ctrl_GotFocus				(object sender, RoutedEventArgs e)				
		{
			m_is_focused = true;
		}
		private		void	this_ctrl_LostFocus				(object sender, RoutedEventArgs e)				
		{
			m_is_focused = false;
		}


		internal	ObservableCollection<property_grid_property>	get_properties_from_objects				(Object[] objects)	
		{
			if (objects[0] is IList)
				return get_properties_from_collection_objects(objects);
			return get_properties_from_class_objects(objects);
		}
		internal	ObservableCollection<property_grid_property>	get_properties_from_class_objects		(Object[] objects)	
		{
			var properties = new ObservableCollection<property_grid_property>();

			Boolean is_first_object		= true;
			var properties_set			= new Dictionary<String, Int32>();
			var new_properties_set		= new HashSet<String>();
			var to_remove_keys			= new List<Int32>();

			foreach (Object obj in objects)
			{
				new_properties_set.Clear();
				to_remove_keys.Clear();
				var collection = TypeDescriptor.GetProperties(obj);
				foreach (PropertyDescriptor descriptor in collection)
				{
					if ( m_browsable_attributes.Count > 0 && !is_match_for_attributes(descriptor))
						continue;

					if (!is_first_object && !properties_set.ContainsKey(descriptor.DisplayName))
						continue;

					property_grid_property prop;
					if (is_first_object)
					{
						prop = new property_grid_property(this);
						properties.Add(prop);
						properties_set.Add(descriptor.DisplayName, properties.Count-1);
					}
					else
						prop = properties[properties_set[descriptor.DisplayName]];

					if (!is_first_object && prop.descriptors[0].PropertyType != descriptor.PropertyType)
						continue;

					prop.name					= descriptor.DisplayName;
					prop.descriptors.Add			(descriptor);
					prop.property_owners.Add	(obj);
					
					prop.default_values.Add(prop.get_property_value(descriptor, obj));

					Object old_val = prop.get_property_value(prop.descriptors[0], prop.property_owners[0]);
					Object new_val = prop.get_property_value(descriptor, obj);

					if ((old_val != null && new_val != null) && !old_val.Equals(new_val))
						prop.is_multiple_values = true;
					
					new_properties_set.Add		(descriptor.DisplayName);
				}
				if (!is_first_object)
				{
					foreach (var entry in properties_set)
					{
						if (!new_properties_set.Contains(entry.Key))
							to_remove_keys.Add(entry.Value);
					}
					for (int i = to_remove_keys.Count-1; i >= 0; --i)
					{
						properties_set.Remove(properties[to_remove_keys[i]].name);
						properties.RemoveAt(to_remove_keys[i]);
					}
				}
				is_first_object = false;
			}

			return properties;
		}
		internal	ObservableCollection<property_grid_property>	get_properties_from_collection_objects	(Object[] objects)	
		{
			var properties = new ObservableCollection<property_grid_property>();

			Boolean is_first_object		= true;

			foreach (Object obj in objects)
			{
				IList collection = (IList)obj;

				for (int i = 0; i < collection.Count; ++i)
				{
					String item_name = "item";
					property_grid_property prop;
					if (is_first_object)
					{
						prop = new property_grid_property(this);
						properties.Add(prop);
					}
					else
						prop = properties[i];

					if (!is_first_object && prop.descriptors[0].PropertyType != collection[i].GetType())
						continue;

					property_grid_item_property_descriptor descriptor = new property_grid_item_property_descriptor(item_name, i, collection);

					prop.name					= item_name;
					prop.descriptors.Add			(descriptor);
					prop.property_owners.Add	(collection);
					prop.is_collection_item		= true;
					
					prop.default_values.Add(prop.get_property_value(descriptor, obj));

					Object old_val = prop.get_property_value(prop.descriptors[0], prop.property_owners[0]);
					Object new_val = prop.get_property_value(descriptor, obj);

					if ((old_val != null && new_val != null) && !old_val.Equals(new_val))
						prop.is_multiple_values = true;
					
				}
				is_first_object = false;
			}

			return properties;
		}


		#endregion

		#region | InnerTypes |


		public enum browsable_state
		{
			and,
			or
		}

		private class read_only_attribute_converter : IValueConverter
		{
			#region IValueConverter Members

			public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
			{
				return ((ReadOnlyAttribute)value).IsReadOnly;
			}

			public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
			{
				throw new NotImplementedException();
			}

			#endregion
		}

		private class item_indent_converter_class : IValueConverter
		{
			#region IValueConverter Members

			public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
			{
				
				return ((Int32)value)+10;
			}

			public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
			{
				return ((Int32)value)-10;
			}

			#endregion
		}

		#endregion

	}
}
