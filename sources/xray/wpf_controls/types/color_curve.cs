using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls
{
	public class color_curve
		{

		#region |   Events   |


		public event Action curve_changed;
		public event EventHandler edit_completed;


		#endregion

		#region | Initialize |


		public			color_curve			():this(new List<color_curve_key>())	
		{
		
        }
		public			color_curve			(List<color_curve_key> keys_list)		
		{
			keys		= keys_list;
			min_time	= 0;
			max_time	= 100;
			min_value	= 0;
			max_value	= 255;
		}
		protected void on_curve_changed		()										
		{
			if(curve_changed != null)
				curve_changed();
		}


		#endregion

		#region |   Fields   |

		#endregion

		#region | Properties |


		public		List<color_curve_key>		keys		
		{
			get; set; 
		}
		public		Single						min_value	
		{
			get;
			set;
		}
		public		Single						max_value	
		{
			get;
			set;
		}
		public		Single						min_time	
		{
			get;
			set;
		}
		public		Single						max_time	
		{
			get;
			set;
		}


		#endregion

		#region |   Methods  |


		public void			fire_curve_changed		()	
		{
			if(curve_changed != null)
				curve_changed();
		}
		public void			fire_edit_completed		()	
		{
			if(edit_completed != null)
				edit_completed(this, EventArgs.Empty);
		}


		#endregion

	}

}
