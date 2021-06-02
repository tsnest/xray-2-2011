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
	public partial class animation_channel_partitions_view : UserControl
	{
		public animation_channel_partitions_view()
		{
			InitializeComponent();
			DataContextChanged += delegate
			{
				m_channel = (animation_channel)DataContext;
				m_partitions_view.ItemsSource = m_channel.objects;
			};
		}

		private animation_channel m_channel;
	}
}
