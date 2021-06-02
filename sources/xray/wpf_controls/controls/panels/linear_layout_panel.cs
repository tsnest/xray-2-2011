////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;
using System.Windows;
using System.Windows.Controls;

namespace xray.editor.wpf_controls.panels
{
	public class linear_layout_panel: Panel
	{
		private					Size				m_desired_size;

		public static readonly	DependencyProperty	orientation_property = DependencyProperty.Register( "orientation", typeof(Orientation), typeof(linear_layout_panel), new FrameworkPropertyMetadata( Orientation.Horizontal, FrameworkPropertyMetadataOptions.AffectsMeasure ) );

		public					Orientation			orientation				
		{
			get
			{
				return (Orientation)GetValue( orientation_property );
			}
			set
			{
				SetValue( orientation_property, value );
			}
		}

		protected override	Size		ArrangeOverride			( Size arrange_size )				
		{
			Double	offset			= 0;
			var		multiplicator	= ( orientation == Orientation.Horizontal ) ? arrange_size.Width / m_desired_size.Width : arrange_size.Height / m_desired_size.Height;

			foreach ( UIElement element in InternalChildren )
			{
				if ( element == null )
					continue;

				switch( orientation )
				{
					case Orientation.Horizontal:
						element.Arrange( new Rect( new Point( offset, 0 ), new Size( element.DesiredSize.Width * multiplicator ,arrange_size.Height ) ) );
						offset += element.DesiredSize.Width * multiplicator;
						break;
					case Orientation.Vertical:
						element.Arrange( new Rect( new Point( 0, offset ), new Size( arrange_size.Width, element.DesiredSize.Height * multiplicator ) ) );
						offset += element.DesiredSize.Height * multiplicator;
						break;
				}
			}
			
			return arrange_size;
		}
		protected override	Size		MeasureOverride			( Size constraint )					
		{
			var available_size	= new Size( Double.PositiveInfinity, Double.PositiveInfinity );
			m_desired_size		= new Size( );
			
			foreach ( UIElement element in InternalChildren )
			{
				if( element == null )
					continue;

				element.Measure( available_size );
				switch( orientation )
				{
					case Orientation.Horizontal:
						m_desired_size.Width += element.DesiredSize.Width;
						m_desired_size.Height = Math.Max( m_desired_size.Height, element.DesiredSize.Height );
						break;
					case Orientation.Vertical:
						m_desired_size.Width = Math.Max( m_desired_size.Width, element.DesiredSize.Width );
						m_desired_size.Height += element.DesiredSize.Height;
						break;
				}
			}
			return m_desired_size;
		}
	}
}
