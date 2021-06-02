//#define TEST_MODE
using System;
using System.ComponentModel;

namespace xray.editor.wpf_controls
{
	public class time_layout_item: INotifyPropertyChanged
	{
		public time_layout_item	( time_layout parent_time_layout, Action show_props,	String item_name, 
								Action<float> set_start_time, Func<float> get_start_time, 
								Action<float> set_length_time, Func<float> get_length_time
							)
		{
			m_parent_time_layout	= parent_time_layout;
			item_text				= item_name;
			m_set_start_time		= set_start_time;
			m_get_start_time		= get_start_time;
			m_set_length_time		= set_length_time;
			m_get_length_time		= get_length_time;
			m_show_props			= show_props;
			
#if TEST_MODE
//Only for TESTS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 			start_time = (new Random((int)get_start_time())).Next(0, (int)m_parent_time_layout.parent_length_time);
 			length_time = (new Random((int)get_start_time())).Next(0, (int)m_parent_time_layout.parent_length_time - (int)start_time);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#endif

		}

	
		private Action<float>	m_set_start_time;
		private Func<float>		m_get_start_time;
		private Action<float>	m_set_length_time;
		private Func<float>		m_get_length_time;
		private Action			m_show_props;

		private time_layout		m_parent_time_layout;

		private bool			m_is_selected;
		public bool				is_selected
		{
			get { return m_is_selected;}
			set { 
				m_is_selected = value;
				if (value)
					m_show_props();
				on_property_changed("is_selected");
			}
		}

		public bool is_in_multiselection{
			get
			{
				return is_selected&&m_parent_time_layout.multi_selection_mode;
			}
			set{}}

		public	String	item_text{ get; set;}               

#if TEST_MODE
// Only for TESTS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 		private float m_start_time;
 		public float start_time
 		{
 			set { m_start_time = value; on_property_changed("start_time");}
 			get { return m_start_time; }
 		}
 
 		private float m_length_time;
		public float length_time
 		{
 			set { m_length_time = value; on_property_changed("length_time"); }
 			get { return m_length_time; }
 		}
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#else
		public	float	start_time
 		{
 			get { return m_get_start_time(); }
 			set	{ m_set_start_time(value);on_property_changed("start_time");}
 		}
 		public float	length_time
 		{
 			get { return m_get_length_time(); }
 			set { m_set_length_time(value);on_property_changed("length_time");}
 		}

#endif
		public time_layout parent_time_layout {
			get{
				return m_parent_time_layout;
			}
		}
		public void reset_property(String property_name){
			on_property_changed(property_name);
		}
		private void on_property_changed(String property_name){
			if (PropertyChanged != null){
				PropertyChanged(this, new PropertyChangedEventArgs(property_name));
			}
		}

		public event PropertyChangedEventHandler PropertyChanged;
	}

	
}
