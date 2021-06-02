using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace xray.editor.wpf_controls
{
	public class time_layout_parent_length_value_validation_rule: ValidationRule
	{
		private time_layout m_parent;

		public time_layout_parent_length_value_validation_rule(time_layout parent)
		{
			m_parent = parent;	
		}
		public override ValidationResult Validate(object value, CultureInfo cultureInfo)
		{
			float result;
			var is_number = float.TryParse(value.ToString(), NumberStyles.Any, cultureInfo,out result);
			if (!is_number)
				return new ValidationResult(false,"Not Number Value");

			float max_children_end_time = 0;

			foreach (time_layout_item item in m_parent.items.Items)
			{
				if (item.start_time + item.length_time > max_children_end_time)
					max_children_end_time = item.start_time + item.length_time;
			}

			if (result < max_children_end_time)
				return new ValidationResult(false, "Parent length can't be lesser than max children end time!");
            
			if (result > 7200)
				return new ValidationResult(false, "Parent length can't be greater than 7200!");

			return new ValidationResult(true,"");
		}
	}
}
