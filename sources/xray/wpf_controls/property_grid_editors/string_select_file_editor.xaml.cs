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

namespace xray.editor.wpf_controls.property_grid_editors
{
	/// <summary>
	/// Interaction logic for string_select_file_editor.xaml
	/// </summary>
	public partial class string_select_file_editor : UserControl
	{
		public string_select_file_editor()
		{
			InitializeComponent();
		}

		private void Browse_Click(object sender, RoutedEventArgs e)
		{
			var obj = (property_grid_property)((Button)sender).DataContext;
			var attributes = obj.descriptors[0].Attributes;
			foreach (var attribute in attributes.OfType<string_select_file_editor_attribute>())
			{
				// Configure open file dialog box
				Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
				dlg.Title = attribute.caption;
				dlg.InitialDirectory = attribute.default_folder;
				dlg.DefaultExt = attribute.default_extension;
				dlg.Filter = attribute.file_mask;

				// Show open file dialog box
				Nullable<bool> result = dlg.ShowDialog();

				// Process open file dialog box results
				if (result == true)
					obj.value = dlg.FileName;

				break;
			}

			file_path.Text = (String)obj.value;
		}
	}

	/// <summary>
	/// Describes that this property will be editied external editor
	/// </summary>
	[AttributeUsage(AttributeTargets.Property)]
	public class string_select_file_editor_attribute : Attribute
	{
		public string_select_file_editor_attribute()
		{

		}

		/// <summary>
		/// Constructs an instance of ExternalEditorAttribute
		/// </summary>
		/// <param name="externalEditorDelegete"> Delegate that will be raised when external editor edit button down </param>
		public string_select_file_editor_attribute(String default_extension, String file_mask, String default_folder, String caption)
		{
			this.caption = caption;
			this.default_extension = default_extension;
			this.file_mask = file_mask;
			this.default_folder = default_folder;
		}

		public String default_extension;
		public String file_mask;
		public String default_folder;
		public String caption;
	}
}
