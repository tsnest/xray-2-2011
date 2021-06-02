////////////////////////////////////////////////////////////////////////////
//	Created		: 07.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	public class float_curve_editor_attribute: Attribute
	{
		internal	Action<Double>		left_limit_setter;
		internal	Action<Double>		top_limit_setter;
		internal	Action<Double>		right_limit_setter;
		internal	Action<Double>		bottom_limit_setter;

		public		Double				initial_left_limit		= Double.NaN;	
		public		Double				initial_top_limit		= Double.NaN;	
		public		Double				initial_right_limit		= Double.NaN;		
		public		Double				initial_bottom_limit	= Double.NaN;	

		public		List<float_curve>	templates = new List<float_curve>() ;

		public		void				set_left_limit		( Double limit )	
		{
			left_limit_setter	( limit );
		}
		public		void				set_top_limit		( Double limit )	
		{
			top_limit_setter	( limit );
		}
		public		void				set_right_limit		( Double limit )	
		{
			right_limit_setter	( limit );
		}
		public		void				set_bottm_limit		( Double limit )	
		{
			bottom_limit_setter	( limit );
		}
	}
}
