using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;

namespace xray.editor.wpf_controls.logic_view
{
    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
	/// 
    public partial class logic_entity_view : INotifyPropertyChanged
    {
        public logic_entity_view()
        {
            InitializeComponent();

			start_event_combobox.Items.Add("#StartScene");
			start_event_combobox.SelectedIndex = 0;

			logic_view_height = Height;
        }

		public Action select_object_button_clicked;
		public Action add_behaviour_button_clicked;


		public Func<List<String>>	get_events_list;

		public Action<String>		set_start_event;
		
		private	const		Double	min_view_size = 100;
		private bool m_is_selected = false;

    	private logic_view m_parent_view;
    	private String m_last_set_item;

		public Double logic_view_height 
		{
			get
			{
				return ActualHeight;
			}
			set
			{
				var static_offset = bottom_grip.ActualHeight + BorderThickness.Top + BorderThickness.Bottom;
				Height = value;
				logic_hypergraph.bottom_limit = value - static_offset;
			}
		} 

    	public bool is_selected 
		{
			get
			{
				return m_is_selected;
			}
			set
			{
				if ( m_is_selected == value )
					return;

				m_is_selected = value;
				
				if( PropertyChanged != null )
					PropertyChanged( this, new PropertyChangedEventArgs( "is_selected" ) );
			}
		}
		
		public void set_parent_view ( logic_view parent_view )
		{
			m_parent_view = parent_view;
		}

    	private void on_bottom_drag(object sender, DragDeltaEventArgs e)
    	{
			var new_height = Height + e.VerticalChange;
			var static_offset = bottom_grip.ActualHeight + BorderThickness.Top + BorderThickness.Bottom;
			
			if ( new_height < logic_hypergraph.field.ActualHeight + static_offset )
			{
				new_height = logic_hypergraph.field.ActualHeight + static_offset;
			}
			
			if ( new_height < min_view_size )
			{
				new_height = min_view_size;
			}
			

    		logic_view_height = new_height;
    	}

		private void parent_control_mouse_left_button_down(object sender, MouseButtonEventArgs e)
		{
			is_selected = true;
		}

    	public event PropertyChangedEventHandler PropertyChanged;

    	private void on_select_object_button_clicked(object sender, RoutedEventArgs e)
    	{
			if ( select_object_button_clicked != null )
    			select_object_button_clicked();
    	}

    	private void on_start_event_combo_box_down(object sender, MouseButtonEventArgs e)
    	{
    		if (start_event_combobox.IsDropDownOpen)
				return;

			if (get_events_list == null)
				return;

			m_last_set_item	= (String)start_event_combobox.SelectedItem;

			start_event_combobox.Items.Clear();
			List<String> combobox_items = get_events_list();
    		foreach (String item in combobox_items)
    		{
    			start_event_combobox.Items.Add(item);
    		}


    	}

    	private void start_event_combo_box_drop_down_closed(object sender, EventArgs e)
    	{
    		if( start_event_combobox.SelectedItem == null )
			{
				start_event_combobox.SelectedItem = m_last_set_item;
			}
    	}

    	private void on_start_event_combobox_selection_changed(object sender, SelectionChangedEventArgs e)
    	{
    		if (set_start_event != null)
				set_start_event(start_event_combobox.SelectedItem as String);
    	}

    	private void on_add_behaviour_button_clicked(object sender, RoutedEventArgs e)
    	{
			if ( add_behaviour_button_clicked != null )
    			add_behaviour_button_clicked();
    	}
    }
}
