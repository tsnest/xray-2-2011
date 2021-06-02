using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;


namespace xray.editor.wpf_controls.logic_view
{
    /// <summary>
    /// Interaction logic for event_control.xaml
    /// </summary>
    public partial class event_control :  INotifyPropertyChanged
    {
    	public event_control()
        {
            InitializeComponent();
        }


		private	const		Double				default_event_offset = 141;
		private				Boolean				m_is_event_name_changing;
		internal static		DependencyProperty	event_nameProperty = DependencyProperty.Register( "event_name", typeof(String), typeof(event_control), new PropertyMetadata( event_name_property_changed ) );


		public				String				event_name			
		{
			get
			{
				return (String)GetValue( event_nameProperty );
			}
			set
			{
				if ( value == "#StartScene" )
					Visibility = Visibility.Collapsed;

				m_is_event_name_changing = true;
				SetValue( event_nameProperty, value );
				m_is_event_name_changing = false;
			}
		}

		private				Double				m_scale								= 1.0f;
		private				Double				m_position;
		private				Double				m_parent_control_scroller_offset	= 0;
		private				logic_view			m_parent_logic_view;
		private				Boolean				m_is_selected;

		public				Action				on_delete_event;
		public				Action				on_view_event_properties;

		public				Boolean				is_selected
		{
			get
			{
				return m_is_selected;
			}
			set
			{
				m_is_selected = value;

				if( PropertyChanged != null )
					PropertyChanged( this, new PropertyChangedEventArgs( "is_selected" ) );
			}
		}

		public				logic_view			parent_logic_view
		{
			get
			{
				return m_parent_logic_view;
			}
			set
			{
				m_parent_logic_view = value;
				m_scale = m_parent_logic_view.logic_panel_scale.X;
			}
		}

		public				Double				position_scale
		{
			get
			{
				if ( parent_logic_view == null )
					return m_scale;

				if ( parent_logic_view.ActualWidth == 0 || parent_logic_view.field_scroller.ActualWidth == 0 )
					return m_scale;

				return m_scale / ( parent_logic_view.ActualWidth / parent_logic_view.field_scroller.ActualWidth );
			}
			set
			{
				m_scale = value;
				SetValue( Canvas.LeftProperty, visual_position );
			}
		}

		public				Double				parent_control_scroller_offset {
			get
			{
				return m_parent_control_scroller_offset;
			}
			set
			{	
				m_parent_control_scroller_offset = value;
				SetValue(Canvas.LeftProperty, visual_position);
			}
		}

		public				Double				logical_position			
		{
			get
			{
				return m_position;
			}
			set
			{	
				m_position = value;
				SetValue( Canvas.LeftProperty, visual_position );
			}
		}

		public				Double				visual_position			
		{
			get
			{
				return logical_position * position_scale - parent_control_scroller_offset ;
			}
			set
			{	
				SetValue(Canvas.LeftProperty, value);
				m_position = ( value + parent_control_scroller_offset ) / position_scale;
			}
		}

		public event		Action<String>		event_name_changed;

		private static		void	event_name_property_changed	( DependencyObject d, DependencyPropertyChangedEventArgs e )
    	{
    		var event_control = (event_control)d;
    		
			if( event_control.event_name_changed != null && !event_control.m_is_event_name_changing )
				event_control.event_name_changed( (String)e.NewValue );
    	}

        private				void	thumb_drag_delta	( Object sender, DragDeltaEventArgs e )
        { 
			foreach ( event_control event_ctrl in m_parent_logic_view.events_strip.Children ){
				if ( event_ctrl.is_selected )
					event_ctrl.set_position_offset( e.HorizontalChange );
			}
        }

		public				void	set_position_offset	( Double offset )
        {
			var new_position = Math.Max( default_event_offset * m_scale - parent_control_scroller_offset,( visual_position + offset ) );

			visual_position = new_position;
        }
		
		public void		set_default_position( )
		{
			visual_position = default_event_offset * position_scale - parent_control_scroller_offset;
		}

    	private void on_event_control_preview_mouse_down(object sender, MouseButtonEventArgs e)
    	{
			if ( Mouse.PrimaryDevice.RightButton == MouseButtonState.Pressed )
			return;


			if ( !Keyboard.IsKeyDown( Key.LeftCtrl ) )
			{
				foreach( event_control event_ctrl in m_parent_logic_view.events_strip.Children )
				{
					event_ctrl.is_selected = false;
				}
			}

    		is_selected = true;
    	}

    	public event PropertyChangedEventHandler PropertyChanged;

    	private void delete_selected_click( object sender, RoutedEventArgs e )
    	{
			is_selected = true;
    		m_parent_logic_view.delete_selected_events();
    	}
		private void text_key_down( object sender, KeyEventArgs e )
		{
			if (e.Key == Key.Enter){
				m_parent_logic_view.scenes_view.Focus();
			}
		}

    	private void view_event_properties_click(object sender, RoutedEventArgs e)
    	{
    		on_view_event_properties( );
    	}
    }
}
