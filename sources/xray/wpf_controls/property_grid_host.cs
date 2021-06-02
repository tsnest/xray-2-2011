using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.ComponentModel.Design;
using System.Windows.Forms.Integration;
using System.Windows.Data;

namespace xray.editor.wpf_controls
{
	public partial class property_grid_host : ElementHost
	{
		#region |   Event    |


		public event property_grid_value_changed_event_handler property_value_changed
		{
			add		{ m_property_grid.property_value_changed += value; }
			remove	{ m_property_grid.property_value_changed -= value; }
		}


		#endregion

		#region | Initialize |


		public property_grid_host()
		{
			InitializeComponent();

			in_constructor();
		}


		#endregion

		#region |   Fields   |


		private		property_grid	m_property_grid;
		public new	String			Child;


		#endregion

		#region | Properties |


		public property_grid		property_grid
		{
			get { return m_property_grid; }
		}

		public Object				selected_object
		{
			get { return m_property_grid.selected_object; }
			set { m_property_grid.selected_object = value;}
		}

		public Object[]				selected_objects
		{
			get { return m_property_grid.selected_objects; }
			set { m_property_grid.selected_objects = value;}
		}

		public ListCollectionView	data_view
		{
			get { return m_property_grid.data_view; }
		}

		public List<Attribute>		browsable_attributes
		{
			get { return m_property_grid.browsable_attributes; }
		}

		public property_grid.browsable_state browsable_attribute_state
		{
			get { return m_property_grid.browsable_attribute_state;}
			set { m_property_grid.browsable_attribute_state = value;}
		}

		public Boolean				auto_update
		{
			get { return m_property_grid.auto_update; }
			set	{ m_property_grid.auto_update = value;	}
		}


		#endregion

		#region |  Methods   |


		private void	in_constructor()
		{
			if (!IsDesignMode())
			{
				m_property_grid = new property_grid();
				base.Child = m_property_grid;
			}
		}

		private bool	IsDesignMode()
		{
			if (Site != null)
				return Site.DesignMode;

			StackTrace stackTrace = new StackTrace();
			int frameCount = stackTrace.FrameCount - 1;

			for (int frame = 0; frame < frameCount; frame++)
			{
				Type type = stackTrace.GetFrame(frame).GetMethod().DeclaringType;
				if (typeof(IDesignerHost).IsAssignableFrom(type))
					return true;
			}
			return false;
		}

		public void		reset()
		{
			m_property_grid.reset();
		}


		#endregion
	}
}
