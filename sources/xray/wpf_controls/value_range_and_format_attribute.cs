using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xray.editor.wpf_controls
{
	public class value_range_and_format_attribute: Attribute
	{
		public value_range_and_format_attribute(Double min_value, Double max_value)
		{
			this.min_value = min_value;
			this.max_value = max_value;
		}

		public value_range_and_format_attribute(Double min_value, Double max_value, Double step_size):
			this(min_value, max_value)
		{
			this.step_size = step_size;
		}

		public value_range_and_format_attribute(Double min_value, Double max_value, Double step_size, Int32 precision):
			this(min_value, max_value, step_size)
		{
			this.precision = precision;
		}

		public value_range_and_format_attribute(Func<Double> min_value_funk, Func<Double> max_value_funk, Double step_size, Int32 precision)
		{
			this.min_value_funk = min_value_funk;
			this.max_value_funk = max_value_funk;
			this.step_size = step_size;
			this.precision = precision;
		}

		public Func<Double> min_value_funk { get; set; }
		public Func<Double> max_value_funk { get; set; }
		
		public Double	min_value { get; set; }
		public Double	max_value { get; set; }
		public Double	step_size { get; set; }
		public Int32	precision { get; set; }
	}
}
