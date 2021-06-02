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
using System.Windows.Controls.Primitives;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for color_ramp.xaml
	/// </summary>
	public partial class color_ramp : UserControl
	{
		public color_ramp()
		{
			InitializeComponent();

			m_gradient = (LinearGradientBrush)m_gradient_box.Background;
		}

		LinearGradientBrush m_gradient;

		private void m_gradient_box_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
		{
			var point = Mouse.GetPosition(m_grid);

			var thumb = new Thumb();
			thumb.Width = 8;
			thumb.Height = m_canvas.ActualHeight;
			thumb.BorderThickness = new Thickness(0);
			thumb.VerticalAlignment = VerticalAlignment.Stretch;
			thumb.HorizontalAlignment = HorizontalAlignment.Left;
			thumb.SetValue(Canvas.LeftProperty, point.X-4);
			thumb.Background = new SolidColorBrush(Colors.Black);

			m_canvas.Children.Add(thumb);
		}
	}
}
