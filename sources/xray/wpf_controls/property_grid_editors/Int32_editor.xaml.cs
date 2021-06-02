using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;

namespace xray.editor.wpf_controls.property_grid_editors
{
	partial class Int32_editor : UserControl
	{
		public Int32_editor()
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

					m_min_value = (Int32)attribute.min_value;
					m_max_value = (Int32)attribute.max_value;
					m_step_size = (Int32)attribute.step_size;
					if (m_step_size == 0)
						m_step_size = 1;
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
		Int32			m_min_value = Int32.MinValue;
		Int32			m_max_value = Int32.MaxValue;
		Int32			m_step_size = 1;

		Boolean			m_drag_started;
		Boolean			m_drag_initialized;
		Boolean			m_mouse_captured;
		Int32			m_initial_mouse_x;
		Int32			m_initial_value;

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
					m_initial_value = (Int32)property.value;
					m_initial_mouse_x = (Int32)e.MouseDevice.GetPosition(e.MouseDevice.DirectlyOver).X;
					m_drag_initialized = true;
					return;
				}
				
				Int32 mouse_delta = (Int32)e.MouseDevice.GetPosition(e.MouseDevice.DirectlyOver).X - m_initial_mouse_x;

				if(!m_mouse_captured && Math.Abs(mouse_delta) >= 1)
				{
					m_mouse_captured = true;
					text_box.CaptureMouse();
					text_box.Cursor = Cursors.SizeWE;
				}

				Int32 value = m_initial_value
					+ (mouse_delta)*m_step_size;

				text_box.Text = ((value < m_min_value)?m_min_value:((value>m_max_value)?m_max_value:value)).ToString();
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
			public validation_rule(Int32_editor editor)
			{
				m_editor = editor;
			}

			private Int32_editor m_editor;

			public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
			{
				Int32 val;

				if(!Int32.TryParse((String)value, out val))
					return new ValidationResult(false, "Can not parse to Int32");

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
