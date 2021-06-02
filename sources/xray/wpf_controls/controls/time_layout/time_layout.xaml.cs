
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows;
using System.Windows.Media;

namespace xray.editor.wpf_controls
{
	/// <summary>
	/// Interaction logic for time_layout.xaml
	/// </summary>
	public partial class time_layout: INotifyPropertyChanged
	{
		public time_layout()
		{

			m_set_length_time	= delegate {};
			m_get_length_time	= ()=>600;
			m_selection_rect	= new RectangleGeometry();
			
			InitializeComponent();

			time_layout_ruler.layout_scale = () => time_layout_scale;
			time_layout_ruler.layout_width = () => (float)time_layout_control.ActualWidth;
			time_layout_ruler.layout_offset = () => (float)time_layout_control.horizontal_scroll.HorizontalOffset;
			
			m_moving_scale = 0;

			m_scale_multiplier = 1.1f;

			parent_length.Focusable = false;

			Binding visibility_binding = new Binding("Items.Count");
			visibility_binding.ElementName = "items";
			visibility_binding.Converter = new time_layout_visibility_converter();
			visibility_binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
			visibility_binding.Mode = BindingMode.OneWay;
			this.SetBinding(Control.VisibilityProperty, visibility_binding);

		}

		private Action<float>		m_set_length_time;
		private Func<float>			m_get_length_time;
		private float				m_last_mouse_down_position;
		private bool				m_right_edge_mouse_down;
		private bool				m_pan_mode;
		private string				m_parent_text;
		private bool				m_selection_mode;
		private Point				m_selection_start_position;
		private readonly RectangleGeometry	m_selection_rect;
		private float				m_moving_scale;
		private float				m_last_parent_length_time;


		private readonly List<Point>	m_last_items_time_parameters = new List<Point>();
		public List<time_layout_item>	selected_items = new List<time_layout_item>();
        public bool						multi_selection_mode		{
			get
			{
				return selected_items.Count > 1;
			}
        }

		public String					parent_text
		{
			get {	
				return m_parent_text; 
			}
			set {
				m_parent_text = value;
				on_property_changed("parent_text");
			}
		}

		public float					parent_length_time{
			get
			{
				return m_get_length_time();
			}
			set{
				m_set_length_time(value);
				on_property_changed("parent_length_time");
			}
		}

		private float m_time_layout_scale = 1.0f;
		private float m_scale_multiplier;

		public float					time_layout_scale
		{
			get { return m_time_layout_scale; }
			set { m_time_layout_scale = value; on_property_changed("time_layout_scale"); time_layout_ruler.InvalidateVisual(); }
		}

		public void set_new_time_layout_params(String parent_node_name, Action<float> set_length_time, Func<float> get_length_time, List<time_layout_item> items_list){
			parent_text = parent_node_name;
			m_set_length_time = set_length_time;
			m_get_length_time = get_length_time;
			parent_length_time = get_length_time();

			Binding parent_length_time_binding		= new Binding ("parent_length_time");
			parent_length_time_binding.ElementName	= "time_layout_control";
			parent_length_time_binding.Converter = new time_layout_parent_length_time_converter(this);
			parent_length_time_binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
			parent_length_time_binding.NotifyOnValidationError = true;
			parent_length_time_binding.ValidationRules.Add(new time_layout_parent_length_value_validation_rule(this));
			parent_length_time_binding.Mode = BindingMode.TwoWay;
			parent_length.SetBinding(TextBox.TextProperty, parent_length_time_binding);

			// panel_width binding!!!
			MultiBinding panel_width_binding = new MultiBinding();
			
			var time_binding = new Binding("parent_length_time");
			time_binding.ElementName = "time_layout_control";
			
			panel_width_binding.Bindings.Add(time_binding);
			
			time_binding = new Binding("time_layout_scale");
			time_binding.ElementName = "time_layout_control";

			panel_width_binding.Bindings.Add(time_binding);
			
			panel_width_binding.Converter = new time_layout_scale_dependent_converter(this);
			panel_width_binding.NotifyOnValidationError = true;
			panel_width_binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
			panel_width_binding.Mode = BindingMode.OneWay;
			parent_node_panel.SetBinding(WidthProperty, panel_width_binding);


			//Text="{Binding ElementName=time_layout_control, Path=parent_length_time, Mode=TwoWay}"

			items.ItemsSource = items_list;
			time_layout_ruler.InvalidateVisual();

		}

		private void parent_node_time_layout_right_edge_mouse_left_button_down(object sender, MouseButtonEventArgs e)
		{
			if (Keyboard.IsKeyDown(Key.LeftAlt))
				return;
			e.Handled = true;
			m_right_edge_mouse_down = true;
			m_moving_scale = 1;
			m_last_parent_length_time = (float)parent_node_panel.Width;

			foreach (time_layout_item item in items.Items)
			{
				Point point = new Point(item.start_time, item.length_time);
				m_last_items_time_parameters.Add(point);
			}

			main_dock_panel.CaptureMouse();
		}
		private void dock_panel_mouse_move(object sender, MouseEventArgs e)
		{
			float offset = (float)e.GetPosition(this).X - m_last_mouse_down_position ;
			if (m_right_edge_mouse_down){
				if (parent_node_panel.Width + offset <= 0)
					offset = -(float)parent_node_panel.Width + 1;
				parent_length_time += offset/time_layout_scale;
			}
			else if (m_pan_mode)
				horizontal_scroll.ScrollToHorizontalOffset(horizontal_scroll.HorizontalOffset - offset);
			
			m_last_mouse_down_position = (float)e.GetPosition(this).X;

			if (m_last_parent_length_time == 0)
				return;

			m_moving_scale = (float)(parent_node_panel.Width) / m_last_parent_length_time;
			
			if (m_moving_scale == 1)
				return;
			var i = 0;

			foreach (time_layout_item item in items.Items)
			{
				
				item.start_time = (float) (m_last_items_time_parameters[i].X*m_moving_scale);
				item.length_time = (float) (m_last_items_time_parameters[i].Y*m_moving_scale);
				i++;
			}
		}
		private void dock_panel_mouse_left_button_up(object sender, MouseButtonEventArgs e)
		{
			main_dock_panel.ReleaseMouseCapture();
			m_right_edge_mouse_down = false;
			m_last_parent_length_time = 0;
			m_moving_scale = 0;
			m_last_items_time_parameters.Clear();
			m_pan_mode = false;
		}
		private void dock_panel_mouse_left_button_down(object sender, MouseButtonEventArgs e)
		{
			if (!Keyboard.IsKeyDown(Key.LeftAlt)) return;
			e.Handled = true;
			m_pan_mode = true;
			main_dock_panel.CaptureMouse();
		}

		public event PropertyChangedEventHandler PropertyChanged;

		private void on_property_changed					(String property_name)						
		{
			if(PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(property_name));
		}

		private void items_grid_panel_preview_mouse_left_button_down(object sender, MouseButtonEventArgs e)
		{
			m_selection_mode			= true;
			selection_path.Visibility	= Visibility.Visible;
			m_selection_start_position	= Mouse.GetPosition(items_grid_panel);
			m_selection_rect.Rect		= new Rect(m_selection_start_position, m_selection_start_position+new Vector(1,1));
			selection_path.Data			= m_selection_rect;

			items_grid_panel.CaptureMouse();
		}

		private void items_grid_panel_preview_mouse_move(object sender, MouseEventArgs e)
		{
			if (m_selection_mode){	
				var new_position		= Mouse.GetPosition(items_grid_panel);
				m_selection_rect.Rect	= new Rect(m_selection_start_position, new_position);
			}

		}

		private void items_grid_panel_preview_mouse_left_button_up(object sender, MouseButtonEventArgs e)
		{
			if (!m_selection_mode)
				return;

			if (!Keyboard.IsKeyDown(Key.LeftCtrl)){
				selected_items.Clear();
				foreach (time_layout_item item in items.Items)
				{
					item.is_selected = false;
				}
			}

			VisualTreeHelper.HitTest	(items_grid_panel, null, selecting_visual, new GeometryHitTestParameters(m_selection_rect));

			m_selection_mode			= false;
			selection_path.Visibility	= Visibility.Hidden;
			items_grid_panel.ReleaseMouseCapture();
		}

		private HitTestResultBehavior selecting_visual(HitTestResult result)
		{
            if (result.VisualHit is Border)
			{
				var ctrl = (Border)(result.VisualHit);
				if(ctrl.Name == "ControlBorder")
				{
					var item = ((time_layout_item)(ctrl.DataContext));
					if (!selected_items.Contains(item)){
						selected_items.Add(item);
						item.is_selected = true;
					}
					else {
						selected_items.Remove(item);
						item.is_selected = false;
					}
				}

				
			}
			return HitTestResultBehavior.Continue;
		}

		private void time_layout_control_mouse_wheel(object sender, MouseWheelEventArgs e)
		{
			if (Keyboard.IsKeyDown(Key.LeftAlt)){
				e.Handled = true;
				if (e.Delta > 0){
					if (time_layout_scale < 25){
						time_layout_scale *= m_scale_multiplier;
						horizontal_scroll.ScrollToHorizontalOffset(horizontal_scroll.HorizontalOffset + main_dock_panel.ActualWidth*(m_scale_multiplier-1)/(main_dock_panel.ActualWidth/(horizontal_scroll.HorizontalOffset+time_layout_control.ActualWidth/2)));
					}
				}
				else
				{
					if (time_layout_scale > 0.04){
						time_layout_scale /= m_scale_multiplier;
						horizontal_scroll.ScrollToHorizontalOffset(horizontal_scroll.HorizontalOffset - main_dock_panel.ActualWidth*(1 - 1/m_scale_multiplier)/(main_dock_panel.ActualWidth/(horizontal_scroll.HorizontalOffset+time_layout_control.ActualWidth/2)));
					}
				}
			}
		}

		private void parent_length_key_down(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter)
			{
				parent_length_time = parent_length_time;
				((TextBox)sender).Focusable = false;
			}
		}

		private void parent_length_lost_focus(object sender, RoutedEventArgs e)
		{
			parent_length_time = parent_length_time;
			((TextBox)sender).Focusable = false;
		}

		private void parent_length_mouse_double_click(object sender, MouseButtonEventArgs e)
		{
			e.Handled = true;
			var txt_box = ((TextBox)sender);
			txt_box.Focusable = true;
			txt_box.Focus();
			txt_box.Select(0, txt_box.Text.Length);
		}

		private void horizontal_scroll_scroll_changed(object sender, ScrollChangedEventArgs e)
		{
			time_layout_ruler.InvalidateVisual();
		}
	}
}
