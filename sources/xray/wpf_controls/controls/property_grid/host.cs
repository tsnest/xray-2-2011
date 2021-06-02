////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.ComponentModel.Design;
using System.Windows.Data;
using System.Windows.Forms.Integration;

namespace xray.editor.wpf_controls.property_grid
{
	public partial class host : ElementHost
	{

		#region |   Event    |


		public event value_changed_event_handler property_value_changed
		{
			add		{ m_property_grid.property_value_changed += value; }
			remove	{ m_property_grid.property_value_changed -= value; }
		}

        public event EventHandler refresh_property_changed
        {
            add { m_property_grid.refresh_property_changed += value; }
            remove { m_property_grid.refresh_property_changed -= value; }
        }

		#endregion

		#region | Initialize |


		public host()
		{
			InitializeComponent();

			in_constructor();
		}


		#endregion

		#region |   Fields   |


		private property_grid_control m_property_grid;
		public new String Child;


		#endregion

		#region | Properties |


		public property_grid_control		property_grid			
		{
			get { return m_property_grid; }
		}
		public Object						selected_object			
		{
			get { return m_property_grid.selected_object; }
			set { m_property_grid.selected_object = value;}
		}
		public Object[]						selected_objects		
		{
			get { return m_property_grid.selected_objects; }
			set { m_property_grid.selected_objects = value;}
		}
		public ListCollectionView			data_view				
		{
			get { return m_property_grid.data_view; }
		}
		public List<Attribute>				browsable_attributes	
		{
			get { return m_property_grid.browsable_attributes; }
		}
		public property_editors.browsable_state browsable_attribute_state
		{
			get { return m_property_grid.browsable_attribute_state;}
			set { m_property_grid.browsable_attribute_state = value;}
		}
		public Boolean						auto_update				
		{
			get { return m_property_grid.auto_update; }
			set	{ m_property_grid.auto_update = value;	}
		}
		public Boolean						read_only_mode			
		{
			get 
			{
				return m_property_grid.read_only_mode; 
			}
			set 
			{
				m_property_grid.read_only_mode = value;
			}
		}


		#endregion

		#region |  Methods   |


		private void	in_constructor	()	
		{
			if (!is_design_mode())
			{
				m_property_grid = new property_grid_control();
				base.Child = m_property_grid;
			}
		}
		private bool	is_design_mode	()	
		{
			if (Site != null)
				return Site.DesignMode;

			var stack_trace = new StackTrace();
			var frame_count = stack_trace.FrameCount - 1;

			for (int frame = 0; frame < frame_count; frame++)
			{
				Type type = stack_trace.GetFrame(frame).GetMethod().DeclaringType;
				if (typeof(IDesignerHost).IsAssignableFrom(type))
					return true;
			}
			return false;
		}

		public void		reset			()	
		{
			m_property_grid.reset();
		}
		public void		update			()	
		{
			m_property_grid.update();
		}


		#endregion

	}
}
