using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using xray.editor.wpf_controls.property_editors.attributes;

namespace xray.editor.wpf_controls.property_editors.value
{
	/// <summary>
	/// Interaction logic for color_curve_editor.xaml
	/// </summary>
	public partial class color_curve_editor
	{
		public		color_curve_editor		( )		
		{
			InitializeComponent();

			m_color_ramp.key_added				+= color_key_added;
			m_color_ramp.key_modified			+= color_key_modified;
			m_color_ramp.key_removed			+= color_key_removed;
			m_color_ramp.key_modification_done	+= color_key_modification_done;

			m_ruler.layout_width				+= ( ) => (Single)m_ruler.ActualWidth;
			m_ruler.layout_scale				+= ( ) => (Single)( ( m_ruler.ActualWidth - 10 ) / ( m_max_color_offset - m_min_color_offset ) );
			m_ruler.value_offset				+= ( ) => 5;

			DataContextChanged					+= data_context_changed;
		}

		private				Double			m_min_color_offset;
		private				Double			m_max_color_offset;
		private				Func<Boolean>	m_recalculate_keys;

		private				color_curve		m_edited_curve;
		private readonly	Dictionary<GradientStop, color_curve_key> m_key_assiciations = new Dictionary<GradientStop, color_curve_key>( );

		private				void			data_context_changed		( Object sender, DependencyPropertyChangedEventArgs e )		
		{
			if ( DataContext == null )
				return;

			m_property	= (property)DataContext;
		
			var attr	= (color_curve_editor_attribute)m_property.descriptor.Attributes[ typeof( color_curve_editor_attribute ) ];
			if( attr != null )
			{
				attr.limit_setter		= set_limit;
				m_recalculate_keys		= attr.recalculate_keys;
			}

			fill_color_ramp( );
		}
		private				void			color_key_removed			( Object sender, color_ramp.key_event_args e )				
		{
			m_edited_curve.keys.Remove(m_key_assiciations[e.stop]);
			m_edited_curve.fire_edit_completed();
		}
		private				void			color_key_modified			( Object sender, color_ramp.key_event_args e )				
		{
			var range_width		= m_max_color_offset - m_min_color_offset;
			var key				= m_key_assiciations[e.stop];
			key.color			= e.color;
			key.position		= (Single)( e.stop.Offset * range_width - m_min_color_offset );
		}
		private				void			color_key_added				( Object sender, color_ramp.key_event_args e )				
		{
			var range_width = m_max_color_offset - m_min_color_offset;
			var key = new color_curve_key(
				(Single)(e.stop.Offset * range_width - m_min_color_offset),
				e.color
			);

			m_edited_curve.keys.Add(key);
			m_key_assiciations.Add(e.stop, key);
			m_edited_curve.fire_edit_completed();
		}
		private				void			color_key_modification_done	( Object sender, color_ramp.key_event_args e )				
		{
			m_edited_curve.fire_edit_completed();
		}

		private				void			fill_color_ramp				( )															
		{
			m_key_assiciations.Clear( );
			m_color_ramp.clear		( );

			var curve		= (color_curve)m_property.value;
			m_edited_curve	= curve;

			m_min_color_offset = curve.min_time;
			m_max_color_offset = curve.max_time;

			var range_width = m_max_color_offset - m_min_color_offset;

			foreach ( var key in curve.keys )
			{
				var new_stop			= m_color_ramp.add_key( ( key.position - m_min_color_offset) / range_width, key.color );
				m_key_assiciations.Add	( new_stop, key );
			}
			
			m_color_ramp.invalidate( );
		}
		private				void			set_limit					( Double new_limit )										
		{
			m_max_color_offset			= new_limit;
			m_ruler.InvalidateVisual	( );

			var range_width		= m_max_color_offset - m_min_color_offset;

			if( m_recalculate_keys( ) )
			{
				foreach( var stop in m_color_ramp.gradient.GradientStops )
				{
					var key				= m_key_assiciations[stop];
					key.position		= (Single)( stop.Offset * range_width - m_min_color_offset );
				}
			}
			else
			{
				foreach( var stop in m_color_ramp.gradient.GradientStops )
				{
					var key				= m_key_assiciations[stop];
					stop.Offset			= ( key.position - m_min_color_offset ) / range_width;

					if( stop.Offset > 1 )
					{
						stop.Offset = 1;
						key.position		= (Single)( stop.Offset * range_width - m_min_color_offset );
					}
				}
			}

			m_color_ramp.invalidate	( );
		}

		public override		void			update						( )															
		{
			fill_color_ramp( );
		}
	}
}
