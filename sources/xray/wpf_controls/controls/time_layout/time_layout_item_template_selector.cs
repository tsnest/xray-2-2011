using System.Drawing;
using System.Windows;
using System.Windows.Controls;

namespace xray.editor.wpf_controls
{
	public class time_layout_item_template_selector: DataTemplateSelector
	{
		private int m_i = 1;

		public override DataTemplate SelectTemplate(object item, DependencyObject container)
		{
			ItemsControl ctrl = ItemsControl.ItemsControlFromItemContainer(container);
			if(ctrl.Items.IndexOf(item) == 0)
				m_i = 1;
			if(m_i++%2==0)
			{
				return (DataTemplate)((FrameworkElement)container).FindResource("time_layout_item_template_even");
			}

			return (DataTemplate)((FrameworkElement)container).FindResource("time_layout_item_template_odd");
		
		}

	}

}
