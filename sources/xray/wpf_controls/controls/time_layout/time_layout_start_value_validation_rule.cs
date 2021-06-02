using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace xray.editor.wpf_controls
{
	public class time_layout_start_value_validation_rule: ValidationRule
	{
		private time_layout_item m_parent;

		public time_layout_start_value_validation_rule(time_layout_item_control parent)
		{
			m_parent = (time_layout_item)parent.DataContext;	
		}
		public override ValidationResult Validate(object value, CultureInfo cultureInfo)
		{
			float result;
			var is_number = float.TryParse(value.ToString(), NumberStyles.Any, cultureInfo,out result);
			if (!is_number)
				return new ValidationResult(false,"Not Number Value");

				if (m_parent != null && (result < 0 || result > m_parent.parent_time_layout.parent_length_time - m_parent.length_time))
				return new ValidationResult(false,"Value can't be neither greater then parent length time nor lesser then start time!");


			return new ValidationResult(true,"");
		}
	}
}
