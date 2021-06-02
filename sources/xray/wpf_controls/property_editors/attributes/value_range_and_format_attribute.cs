////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class value_range_and_format_attribute: Attribute
	{
		public value_range_and_format_attribute(Double min_value, Double max_value)
		{
			this.min_value = min_value;
			this.max_value = max_value;
            this.precision = 2;
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

		public value_range_and_format_attribute(Func<Double> min_value_func, Func<Double> max_value_func, Double step_size, Int32 precision)
		{
			this.min_value_func = min_value_func;
			this.max_value_func = max_value_func;
			this.step_size = step_size;
			this.precision = precision;
		}

		public Func<Double> min_value_func { get; set; }
		public Func<Double> max_value_func { get; set; }
		
		public Double	min_value { get; set; }
		public Double	max_value { get; set; }
		public Double	step_size { get; set; }
		public Int32	precision { get; set; }

		public Boolean	update_on_edit_complete {get;set;}
	}
}
