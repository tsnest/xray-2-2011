using System.Collections.Generic;
using System.Windows.Controls;
using xray.editor.wpf_controls.property_editors;

namespace xray.editor.wpf_controls.data_sources
{
	/// <summary>
	/// Interaction logic for data_source_item.xaml
	/// </summary>
	public partial class data_source_item : UserControl
	{
		public data_source_item()
		{
			InitializeComponent();

			DataContextChanged += delegate 
			{
				if( DataContext == null )
					return;

				var prop = property_extractor.extract<List<property>>( DataContext );
				m_data_source_editor.Content = prop[2];
			};
		}
	}
}
