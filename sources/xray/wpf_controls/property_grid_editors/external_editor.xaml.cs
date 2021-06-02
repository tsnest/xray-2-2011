using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.property_grid_editors
{
	partial class external_editor: ResourceDictionary
	{
		public external_editor()
		{
			InitializeComponent();
		}

		private void Panel_PreviewKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
		{
			if (e.Key == Key.Delete || e.Key == Key.Back)
				Clear_Click(((DockPanel)sender).Children[1], new RoutedEventArgs());
		}

		private void run_editor_button_Click(object sender, RoutedEventArgs e)
		{
			var obj = (property_grid_property)((Button)sender).DataContext;
			var attributes = obj.descriptors[0].Attributes;
			foreach (var attribute in attributes.OfType<external_editor_attribute>())
			{
				attribute.external_editor_delegate(obj, e);
				break;
			}
			((TextBox)((DockPanel)((Button)sender).Parent).Children[2]).Text = (String)obj.value;
		}

		private void Clear_Click(object sender, RoutedEventArgs e)
		{
			var obj = (property_grid_property)((Button)sender).DataContext;
			obj.value = "";
			((TextBox)((Button)sender).Tag).Text = "";
		}
	}

	/// <summary>
	/// Describes that this property will be editied external editor
	/// </summary>
	[AttributeUsage(AttributeTargets.Property)]
	public class external_editor_attribute : Attribute
	{
		public external_editor_attribute()
		{

		}

		/// <summary>
		/// Constructs an instance of ExternalEditorAttribute
		/// </summary>
		/// <param name="externalEditorDelegete"> Delegate that will be raised when external editor edit button down </param>
		public external_editor_attribute(external_editor_event_handler external_editor_delegete)
		{
			if (external_editor_delegete == null)
				throw new NullReferenceException("externalEditorDelegete parameter can not be null");
			external_editor_delegate = external_editor_delegete;
		}

		/// <summary>
		/// Type of External Editor that will be modify specified proprty
		/// </summary>
		public external_editor_event_handler external_editor_delegate;
	}

	/// <summary>
	/// Represents a Delegate that will be raised when external editor edit button down
	/// </summary>
	/// <param name="sender"> object that wraps source property </param>
	/// <param name="e"> event that raised </param>
	public delegate void external_editor_event_handler(Object sender, RoutedEventArgs e);
}
