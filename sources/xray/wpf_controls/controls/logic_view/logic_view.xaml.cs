using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;

namespace xray{
namespace editor{
namespace wpf_controls{
namespace logic_view{

/// <summary>
/// Interaction logic for logic_view.xaml
/// </summary>
public partial class logic_view
{
    public logic_view()
    {
        InitializeComponent();

		field_scroller.ScrollChanged += m_field_scroller_ScrollChanged;

		m_scenes_view = new logic_scenes_view();

		logic_entities_list.Children.Add( m_scenes_view );
    }

	private void m_field_scroller_ScrollChanged(object sender, ScrollChangedEventArgs e)
	{
		foreach( event_control event_view in events_strip.Children )
		{	
			event_view.parent_control_scroller_offset += e.HorizontalChange;
		}
	}

    private logic_scenes_view m_scenes_view;

	public logic_scenes_view scenes_view
	{
		get
		{
			return m_scenes_view;
		}
	}

	private			Point		m_scale		= new Point(1,1);
	private const	Double		c_epsilon	= 0.1;
	private			Boolean		m_pan_started;
	private			Boolean		m_zoom_started;
	private			Point		m_mouse_position;

	public			Point		logic_panel_scale			
	{
		get
		{
			return m_scale;
		}
		set
		{
			m_scale = value;
			if( m_scale.X < c_epsilon )	m_scale.X = c_epsilon;
			if( m_scale.Y < c_epsilon )	m_scale.Y = c_epsilon;
			logic_entities_list.LayoutTransform = new ScaleTransform( m_scale.X, m_scale.Y );
			foreach( var event_view in events_strip.Children )
			{
				((event_control)event_view).position_scale = m_scale.X;
			}

		}
	}

	public		Point						viewport_position		
	{
		get
		{
			return new Point( field_scroller.HorizontalOffset, field_scroller.VerticalOffset );
		}
		set
		{
            field_scroller.ScrollToHorizontalOffset	(((Vector)value).X);
			field_scroller.ScrollToVerticalOffset		(((Vector)value).Y);
		}
	}

	public List<logic_entity_view> get_selected_views()
	{
		List<logic_entity_view> ret_lst = new List<logic_entity_view>();
		foreach ( var view in logic_entities_list.Children )
		{
			if ( view is logic_entity_view && ((logic_entity_view)view).is_selected )
				ret_lst.Add(((logic_entity_view)view));
		}
		return ret_lst;
	}

    public void add_logic ( logic_entity_view view )
    {
		view.set_parent_view(this);
        logic_entities_list.Children.Add( view );
    }
    public void remove_logic ( logic_entity_view view )
    {
        logic_entities_list.Children.Remove( view );
    }
    public void clear_logics()
    {
        logic_entities_list.Children.Clear( );
    }

    public void add_event( event_control event_ctrl )
    {
        events_strip.Children.Add(event_ctrl);
		event_ctrl.parent_logic_view = this;
		
        var event_ctrl_height_binding = new Binding("ActualHeight");
        event_ctrl_height_binding.Source = this;
        event_ctrl_height_binding.UpdateSourceTrigger = UpdateSourceTrigger.PropertyChanged;
        event_ctrl.SetBinding( Control.HeightProperty, event_ctrl_height_binding );

    }

	public void remove_event ( event_control event_ctrl )
    {
        events_strip.Children.Remove( event_ctrl );
    }

	private void handle_input(object sender, InputCommandEventArgs e)
	{
		if( e.command_string == "F" || e.command_string == "Del" )
				e.Handled = true;
	}

	private				void			preview_mouse_button_down		( Object sender, MouseButtonEventArgs e )			
	{
		//Focus( );
		m_mouse_position	= Mouse.GetPosition( this );

		if (e.ChangedButton == MouseButton.Middle && Keyboard.PrimaryDevice.IsKeyDown(Key.LeftAlt))
		{
			m_pan_started		= true;
			zoomable_panel.CaptureMouse		( );
			Cursor				= Cursors.SizeAll;
			e.Handled			= true;
		}
		else if(e.ChangedButton == MouseButton.Right && Keyboard.PrimaryDevice.IsKeyDown(Key.LeftAlt))
		{
			m_zoom_started		= true;
			zoomable_panel.CaptureMouse		( );
			Cursor				= Cursors.SizeAll;
			e.Handled			= true;
		}
	}
	private				void			preview_mouse_move				( Object sender, MouseEventArgs e )					
	{
		if ( m_pan_started )
		{
			var new_mouse_position	= Mouse.GetPosition( this );
			var mouse_offset		= (Point)( new_mouse_position - m_mouse_position );
			m_mouse_position		= new_mouse_position;

			viewport_position		-= (Vector)mouse_offset;
			e.Handled				= true;
			return;
		}
		if( m_zoom_started )
		{
			var new_mouse_position	= Mouse.GetPosition( this );
			var mouse_offset		= new_mouse_position - m_mouse_position;
			m_mouse_position		= new_mouse_position;

			var mouse_start			= (Vector)Mouse.GetPosition( logic_entities_list );

			mouse_offset			/= 100;
			mouse_offset.Y			= mouse_offset.X;
			logic_panel_scale		+= mouse_offset;

			var mouse_end			= new Vector( mouse_start.X * ( 1+mouse_offset.X), mouse_start.Y * (1+mouse_offset.Y) );
            
			viewport_position		+= (mouse_end - mouse_start);

			e.Handled				= true;

			return;
		}
		
	}

	private				void			preview_mouse_button_up			( Object sender, MouseButtonEventArgs e )			
	{
		if( m_pan_started || m_zoom_started )
		{
			m_pan_started		= false;
			m_zoom_started		= false;
			zoomable_panel.ReleaseMouseCapture	( );
			Cursor				= null;
			e.Handled			= true;
		}
	}

	private void zoomable_panel_mouse_wheel(object sender, MouseWheelEventArgs e)
	{
		if ( Keyboard.IsKeyDown( Key.LeftCtrl ) ) 
		{
			Double offset = (Double)e.Delta / 500;
			e.Handled = true;
			var new_mouse_position	= Mouse.GetPosition( this );
			var mouse_offset		= new_mouse_position - m_mouse_position;
			m_mouse_position		= new_mouse_position;

			var mouse_start			= (Vector)Mouse.GetPosition( logic_entities_list );

			mouse_offset			/= 100;
			mouse_offset.Y			= offset;
			mouse_offset.X			= offset;
			logic_panel_scale		+= mouse_offset;

			var mouse_end			= new Vector( mouse_start.X * ( 1 + mouse_offset.X ), mouse_start.Y * ( 1 + mouse_offset.Y ) );
            
			viewport_position		+= ( mouse_end - mouse_start );

			e.Handled				= true;

			return;
		}

		if ( Keyboard.IsKeyDown(Key.LeftShift) )
		{
			var mouse_offset		= new Vector( e.Delta, 0 );
			viewport_position		-= mouse_offset;
			e.Handled				= true;
			return;
		}
	}

	private void on_events_strip_preview_mouse_down(object sender, MouseButtonEventArgs e)
	{
		if ( Mouse.PrimaryDevice.RightButton == MouseButtonState.Pressed )
			return;

		if ( Keyboard.IsKeyDown( Key.LeftCtrl ) )
			return;

		foreach ( event_control event_ctrl in events_strip.Children)
		{
			event_ctrl.is_selected = false;
		}
	}

	public void delete_selected_events( )
	{
		for ( int i = 0; i<events_strip.Children.Count; )
		{
			var event_ctrl = (event_control)events_strip.Children[i];
			if ( event_ctrl.is_selected )
			{	
				if ( event_ctrl.on_delete_event != null )
					event_ctrl.on_delete_event();
				remove_event(event_ctrl);
			}
			else
			{
				i++;
			}
		}
	}

	private void delete_selected_click(object sender, RoutedEventArgs e)
	{
		delete_selected_events();
	}
}


}
}
}
}


