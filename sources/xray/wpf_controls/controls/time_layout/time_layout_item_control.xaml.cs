using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for time_layout_item_control.xaml
	/// </summary>
	public partial class time_layout_item_control
	{

		public time_layout_item_control()
		{
			InitializeComponent();
			

			DataContextChanged += (o, e) =>{
				// Start time binding!!!
				m_item = (time_layout_item)DataContext;
				m_time_layout = ((time_layout_item)DataContext).parent_time_layout;

				var start_time_text_binding = new Binding("start_time")
				                              	{
				                              		Converter = new time_layout_start_time_converter(this),
				                              		UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged,
				                              		NotifyOnValidationError = true
				                              	};
			    start_time_text_binding.ValidationRules.Add(new time_layout_start_value_validation_rule(this));
				start_time_text_binding.Mode = BindingMode.TwoWay;
				start_time_text.SetBinding(TextBox.TextProperty, start_time_text_binding);

				// Length time binding!!!

				var length_time_text_binding = new Binding("length_time")
				                               	{
				                               		Converter = new time_layout_length_time_converter(this),
				                               		UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged,
				                               		NotifyOnValidationError = true
				                               	};
			    length_time_text_binding.ValidationRules.Add(new time_layout_length_value_validation_rule(this));
				length_time_text_binding.Mode = BindingMode.TwoWay;
				length_time_text.SetBinding(TextBox.TextProperty, length_time_text_binding);

				// End time binding!!!

				var end_time_text_binding = new MultiBinding();
				end_time_text_binding.Bindings.Add(new Binding("start_time"));
				end_time_text_binding.Bindings.Add(new Binding("length_time"));
				end_time_text_binding.Converter = new time_layout_end_time_converter(this);
				end_time_text_binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
				end_time_text_binding.NotifyOnValidationError = true;
				end_time_text_binding.ValidationRules.Add(new time_layout_end_value_validation_rule(this));
				end_time_text_binding.Mode = BindingMode.TwoWay;
				end_time_text.SetBinding(TextBox.TextProperty, end_time_text_binding);

				

				// border_left binding!!!
				var border_left_binding = new MultiBinding();
				border_left_binding.Bindings.Add(new Binding("start_time"));
				border_left_binding.Bindings.Add(new Binding("parent_time_layout.time_layout_scale"));
				border_left_binding.Converter = new time_layout_scale_dependent_converter(m_time_layout);
				border_left_binding.NotifyOnValidationError = true;
				border_left_binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
				border_left_binding.Mode = BindingMode.TwoWay;
				ControlBorder.SetBinding(Canvas.LeftProperty, border_left_binding);

				// border_width binding!!!
				var border_width_binding = new MultiBinding();
				border_width_binding.Bindings.Add(new Binding("length_time"));
				border_width_binding.Bindings.Add(new Binding("parent_time_layout.time_layout_scale"));
				border_width_binding.Converter = new time_layout_scale_dependent_converter(m_time_layout);
				border_width_binding.NotifyOnValidationError = true;
				border_width_binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
				border_width_binding.Mode = BindingMode.TwoWay;
				ControlBorder.SetBinding(WidthProperty, border_width_binding);


			};
		}

		private bool				m_left_edge_mouse_down;
		private bool				m_right_edge_mouse_down;
		private bool				m_center_mouse_down;
		private float				m_last_mouse_down_position;
		private time_layout			m_time_layout;
		private bool				m_multiselection_move_mode;
		private time_layout_item	m_item;

		private void right_edge_mouse_left_button_down		(object sender, MouseButtonEventArgs e)		
		{
			if (Keyboard.IsKeyDown(Key.LeftCtrl)){
				if(!m_time_layout.selected_items.Contains((time_layout_item)DataContext)){
					m_time_layout.selected_items.Add((time_layout_item)DataContext);
					((time_layout_item)DataContext).is_selected = true;
					e.Handled = true;
					return;
				}
				m_time_layout.selected_items.Remove((time_layout_item)DataContext);
				((time_layout_item)DataContext).is_selected = false;
				e.Handled = true;
				return;
			}

			if (Keyboard.IsKeyDown(Key.LeftAlt))
				return;
			if (m_time_layout.multi_selection_mode 
				&& m_time_layout.selected_items.Contains((time_layout_item)DataContext))
				return;
			foreach (var item in m_time_layout.items.Items)
			{
				((time_layout_item)item).is_selected = false;
			}
			m_time_layout.selected_items.Clear();
			m_time_layout.selected_items.Add((time_layout_item)DataContext);
			((time_layout_item)DataContext).is_selected = true;

			e.Handled = true;	
			m_right_edge_mouse_down = true;
			CaptureMouse();

        }
	
		private void left_edge_mouse_left_button_down		(object sender, MouseButtonEventArgs e)		
		{
			if (Keyboard.IsKeyDown(Key.LeftCtrl)){
				if(!m_time_layout.selected_items.Contains((time_layout_item)DataContext)){
					m_time_layout.selected_items.Add((time_layout_item)DataContext);
					((time_layout_item)DataContext).is_selected = true;
					e.Handled = true;
					return;
				}
				m_time_layout.selected_items.Remove((time_layout_item)DataContext);
				((time_layout_item)DataContext).is_selected = false;
				e.Handled = true;
				return;
			}
			if (Keyboard.IsKeyDown(Key.LeftAlt))
				return;
			if (m_time_layout.multi_selection_mode 
				&& m_time_layout.selected_items.Contains((time_layout_item)DataContext))
				return;
			foreach (var item in m_time_layout.items.Items)
			{
				((time_layout_item)item).is_selected = false;
			}
			m_time_layout.selected_items.Clear();
			m_time_layout.selected_items.Add((time_layout_item)DataContext);
			((time_layout_item)DataContext).is_selected = true;
			
			e.Handled = true;	
			m_left_edge_mouse_down = true;
			CaptureMouse();
		}
	

		private void user_control_mouse_move				(object sender, MouseEventArgs e)			
		{
		
			float offset = (float)e.GetPosition(this).X - m_last_mouse_down_position;



			if (m_right_edge_mouse_down)
			{
				var item = ((time_layout_item)DataContext).parent_time_layout;
				if (m_item.start_time + m_item.length_time + offset > item.parent_length_time)
					offset = (item.parent_length_time- (m_item.start_time + m_item.length_time));
				else if(m_item.length_time + offset <= 0)
					offset = (-m_item.length_time + 1);
				m_item.length_time += offset/m_time_layout.time_layout_scale;
			}
			else if (m_left_edge_mouse_down)
			{
				if (m_item.length_time - offset <= 0)
					offset = (m_item.length_time - 1);
				else if (m_item.start_time + offset < 0)
					offset = (-m_item.start_time);
				m_item.start_time += offset/m_time_layout.time_layout_scale;
				m_item.length_time -= offset/m_time_layout.time_layout_scale;
			}
			else if (m_center_mouse_down)
			{
				var item = ((time_layout_item)DataContext).parent_time_layout;
				if (m_item.start_time + m_item.length_time + offset > item.parent_length_time)
					offset = (item.parent_length_time - (m_item.start_time + m_item.length_time));
				else if (m_item.start_time + offset < 0)
					offset = (-m_item.start_time);
				m_item.start_time += offset/m_time_layout.time_layout_scale;
			}
			else if (m_multiselection_move_mode)
			{
				foreach(time_layout_item item in m_time_layout.selected_items){
					if (item.start_time + item.length_time + offset > m_time_layout.parent_length_time)
					offset = (m_time_layout.parent_length_time - (item.start_time + item.length_time));
				else if (item.start_time + offset < 0)
					offset = (-item.start_time);
				
				}
				foreach(time_layout_item item in m_time_layout.selected_items){
					item.start_time+=offset/m_time_layout.time_layout_scale;
				}
			}


			m_last_mouse_down_position = (float)e.GetPosition(this).X;
		}


		private void user_control_mouse_left_button_down	(object sender, MouseButtonEventArgs e)		
		{
			m_last_mouse_down_position = (float)e.GetPosition(this).X;
		}
		private void user_control_mouse_left_button_up		(object sender, MouseButtonEventArgs e)		
		{
			ReleaseMouseCapture();
			
			m_right_edge_mouse_down = false;
			m_left_edge_mouse_down = false;
			m_center_mouse_down = false;
			m_multiselection_move_mode = false;
		}

		private void dock_panel_mouse_left_button_down(object sender, MouseButtonEventArgs e)
		{
			if (Keyboard.IsKeyDown(Key.LeftCtrl)){
				if(!m_time_layout.selected_items.Contains((time_layout_item)DataContext)){
					m_time_layout.selected_items.Add((time_layout_item)DataContext);
					((time_layout_item)DataContext).is_selected = true;
					e.Handled = true;
					return;
				}
				m_time_layout.selected_items.Remove((time_layout_item)DataContext);
				((time_layout_item)DataContext).is_selected = false;
				e.Handled = true;
				return;
			}

			if (Keyboard.IsKeyDown(Key.LeftAlt))
				return;
			e.Handled = true;	

			if (m_time_layout.multi_selection_mode
				&& m_time_layout.selected_items.Contains((time_layout_item)DataContext)){
				m_multiselection_move_mode = true;  
			}
			else{
				foreach (var item in m_time_layout.items.Items)
				{
					((time_layout_item)item).is_selected = false;
				}
				m_time_layout.selected_items.Clear();
				m_time_layout.selected_items.Add((time_layout_item)DataContext);
				((time_layout_item)DataContext).is_selected = true;
			
				m_center_mouse_down = true;
			}
			CaptureMouse();
		}

		private void text_mouse_double_click(object sender, MouseButtonEventArgs e)
		{
			if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.LeftAlt))
				return;
			var txt_box = ((TextBox)sender);
			txt_box.Focusable = true;
			txt_box.Focus();
			txt_box.Select(0, txt_box.Text.Length);
		}
		
		private void text_lost_focus(object sender, System.Windows.RoutedEventArgs e)
		{
			((TextBox)sender).Focusable = false;
			((time_layout_item)DataContext).reset_property("start_time");
			((time_layout_item)DataContext).reset_property("length_time");
		//	((time_layout_item)DataContext).start_time = ((time_layout_item)DataContext).start_time;
		//	((time_layout_item)DataContext).length_time = ((time_layout_item)DataContext).length_time;
		}
		private void text_key_down(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter){
				((TextBox)sender).Focusable = false;
				((time_layout_item)DataContext).reset_property("start_time");
				((time_layout_item)DataContext).reset_property("length_time");
			//	((time_layout_item)DataContext).start_time = ((time_layout_item)DataContext).start_time;
			//	((time_layout_item)DataContext).length_time = ((time_layout_item)DataContext).length_time;
			}
		}

	}
}
