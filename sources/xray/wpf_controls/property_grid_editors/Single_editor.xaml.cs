using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace xray.editor.wpf_controls.property_grid_editors
{
	partial class Single_editor: UserControl
	{
		public Single_editor()
		{
			InitializeComponent();
			
			EditorToolTip.SetValue(ToolTipService.PlacementTargetProperty, text_box);

			DataContextChanged += (o, e) =>
			{
				property = (property_grid_property)e.NewValue;
				text_box.GetBindingExpression(TextBox.TextProperty).ParentBinding.ValidationRules.Add(new validation_rule(this));

				var attribute = (value_range_and_format_attribute)property.descriptors[0].Attributes[typeof(value_range_and_format_attribute)];
				if (attribute != null)
				{
					m_min_value_func = attribute.min_value_funk;
					m_max_value_func = attribute.max_value_funk;

					m_min_value = (Single)attribute.min_value;
					m_max_value = (Single)attribute.max_value;
					m_step_size = (Single)attribute.step_size;
					m_precision = (Int32)attribute.precision;
					if (m_step_size == 0)
						m_step_size = 0.1f;
				}
				else
				{
					progress_bar.Visibility = Visibility.Collapsed;
				}

				if (m_min_value_func != null && m_max_value_func != null)
				{
					progress_bar.Maximum = m_max_value_func();
					progress_bar.Minimum = m_min_value_func();
				}
				else
				{
					progress_bar.Maximum = m_max_value;
					progress_bar.Minimum = m_min_value;
				}
			};
		}

		property_grid_property property;

		Func<Double>	m_min_value_func;
		Func<Double>	m_max_value_func;
		Single			m_min_value =  Single.MinValue;
		Single			m_max_value =  Single.MaxValue;
		Single			m_step_size = 0.1f;
		Int32			m_precision = 4;

		Boolean			m_drag_started;
		Boolean			m_drag_initialized;
		Boolean			m_mouse_captured;
		Single			m_initial_mouse_x;
		Single			m_initial_value;

		private void TextBox_PreviewMouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			TextBox textBox = (TextBox)sender;
			Point pt = e.MouseDevice.GetPosition(e.MouseDevice.DirectlyOver);

			if (pt.X > textBox.GetRectFromCharacterIndex(textBox.Text.Length, true).X)
			{
				m_drag_started = true;
				e.Handled = true;
			}
		}

		private void TextBox_PreviewMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
		{
			if (m_drag_started)
			{
				if (e.LeftButton == System.Windows.Input.MouseButtonState.Released)
				{
					m_drag_started = false;
					m_drag_initialized = false;
					m_mouse_captured = false;
					text_box.ReleaseMouseCapture();
					text_box.Cursor = null;
					return;
				}

				if (!m_drag_initialized)
				{
					m_initial_value = (Single)property.value;
					m_initial_mouse_x = (Single)e.MouseDevice.GetPosition(e.MouseDevice.DirectlyOver).X;
					m_drag_initialized = true;
					return;
				}

				Single mouse_delta = (Single)e.MouseDevice.GetPosition(e.MouseDevice.DirectlyOver).X - m_initial_mouse_x;

				if (!m_mouse_captured && Math.Abs(mouse_delta) >= 1)
				{
					m_mouse_captured = true;
					text_box.CaptureMouse();
					text_box.Cursor = Cursors.SizeWE;
				}

				Single value = m_initial_value
					+ (mouse_delta)*m_step_size;

				text_box.Text =  String.Format("{0:F"+m_precision+"}",((value < m_min_value)?m_min_value:((value>m_max_value)?m_max_value:value)));
			}
		}

		private void text_box_PreviewMouseDoubleClick(object sender, MouseButtonEventArgs e)
		{
			text_box.Focus();
			text_box.SelectAll();
		}

		private void TextBox_Error(object sender, ValidationErrorEventArgs e)
		{
			if (e.Action == ValidationErrorEventAction.Added)
			{
				EditorToolTip.IsOpen = true;
				EditorToolTip.Content = e.Error.ErrorContent;
				EditorToolTip.Visibility = Visibility.Visible;
			}
			else
			{
				EditorToolTip.IsOpen = false;
				EditorToolTip.Visibility = Visibility.Collapsed;
			}
		}

		#region | InnerTypes |


		class validation_rule : ValidationRule
		{
			public validation_rule(Single_editor editor)
			{
				m_editor = editor;
			}

			private Single_editor m_editor;

			public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
			{
				Single val;

				if(!Single.TryParse((String)value, out val))
					return new ValidationResult(false, "Can not parse to Single");

				if (m_editor.m_min_value_func != null && m_editor.m_max_value_func != null)
				{
					m_editor.progress_bar.Maximum = m_editor.m_max_value_func();
					m_editor.progress_bar.Minimum = m_editor.m_min_value_func();

					if (val >= m_editor.m_min_value_func() && val <= m_editor.m_max_value_func())
						return new ValidationResult(true, "");
				}
				else
				{
					if (val >= m_editor.m_min_value && val <= m_editor.m_max_value)
						return new ValidationResult(true, "");
				}

				return new ValidationResult(false, "value not in the specified range");
			}
		}


		#endregion
	}
}
