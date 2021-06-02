using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xray.editor.wpf_controls.animation_setup
{
	public class setup_panel_event_args: EventArgs
	{
		public setup_panel_event_args(String ch_n, String i_n, String pr_n, Object new_val)
		{
			channel_name = ch_n;
			item_names = new List<String>();
			property_names = new List<String>();
			new_values = new List<Object>();
			item_names.Add(i_n);
			property_names.Add(pr_n);
			new_values.Add(new_val);
		}
		public setup_panel_event_args(String ch_n, List<String> i_n, List<String> pr_n, List<Object> new_vals)
		{
			channel_name = ch_n;
			item_names = i_n;
			property_names = pr_n;
			new_values = new_vals;
		}
		
		public String		channel_name;
		public List<String>	item_names;
		public List<String>	property_names;
		public List<Object>	new_values;
	}


	public class setup_panel_copy_event_args: EventArgs
	{
		public setup_panel_copy_event_args(String ch_n)
		{
			channel_name = ch_n;
		}
		
		public String		channel_name;
	}
}
