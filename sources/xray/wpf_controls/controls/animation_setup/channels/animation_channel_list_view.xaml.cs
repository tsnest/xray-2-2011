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

namespace xray.editor.wpf_controls.animation_setup
{
	public partial class animation_channel_list_view : UserControl
	{
		public animation_channel_list_view()
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				animation_channel first_item = (animation_channel)DataContext;
				if(first_item.type==animation_channel_type.intervals)
					m_channel_views.ItemTemplate = new DataTemplate{VisualTree = new FrameworkElementFactory(typeof(animation_channel_intervals_view))};
				else if(first_item.type==animation_channel_type.partitions)
					m_channel_views.ItemTemplate = new DataTemplate{VisualTree = new FrameworkElementFactory(typeof(animation_channel_partitions_view))};
				else
					m_channel_views.ItemTemplate = new DataTemplate{VisualTree = new FrameworkElementFactory(typeof(animation_channel_events_view))};

				first_item.list_view = this;
				List<animation_channel> lst = new List<animation_channel>();
				Single item_scaled_length = first_item.length/first_item.panel.time_layout_scale;
				Single panel_max_time = first_item.panel.max_time;
				UInt16 channel_count = (UInt16)Math.Ceiling(panel_max_time/item_scaled_length);
				for(; channel_count>0; --channel_count)
					lst.Add(first_item);

				channel_views = lst;
			};
		}

		public	List<animation_channel> channel_views
		{
		    get
		    {
		        return (List<animation_channel>)m_channel_views.ItemsSource;
		    }
		    set
		    {
		        m_channel_views.ItemsSource = value;
		    }
		}

		internal void update(Single panel_max_time)
		{
			if(channel_views!=null)
			{
				animation_channel first_item = channel_views[0];
				List<animation_channel> lst = new List<animation_channel>();
				Single item_scaled_length = first_item.length/first_item.panel.time_layout_scale;
				UInt16 channel_count = (UInt16)Math.Ceiling(panel_max_time/item_scaled_length);
				for(; channel_count>0; --channel_count)
					lst.Add(first_item);

				channel_views = lst;
			}
		}
	}
}
