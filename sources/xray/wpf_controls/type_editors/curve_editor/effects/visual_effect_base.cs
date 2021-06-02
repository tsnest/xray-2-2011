////////////////////////////////////////////////////////////////////////////
//	Created		: 21.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using System.Windows.Shapes;

namespace xray.editor.wpf_controls.curve_editor.effects
{
	internal abstract class visual_effect_base
	{
		public visual_effect_base	( visual_curve parent_curve )
		{
			m_parent_curve	= parent_curve;
			m_evaluator		= new float_curve_evaluator( m_parent_curve.float_curve );

			m_effect_curve					= new Path{
		  		Stroke				= new SolidColorBrush( m_parent_curve.float_curve.color ),
		  		StrokeThickness		= 1,
		  	};

			m_parent_curve.path.StrokeDashArray = new DoubleCollection{ 5, 3 };
		}

		protected readonly	visual_curve			m_parent_curve;
		protected readonly	float_curve_evaluator	m_evaluator;

		protected readonly	Path					m_effect_curve;

		internal			Path					effect_curve		
		{
			get
			{
				return m_effect_curve;
			}
		}
		
		internal virtual	void					dispose					( )
		{
			if( m_parent_curve.effects.Count == 0 )
				m_parent_curve.path.StrokeDashArray = null;
		}

		internal abstract	void					update_visual			( );
		internal abstract	void					open_effect_settings	( Window get_window );
		internal abstract	void					open_effect_settings	( Window get_window, List<visual_effect_base> effects );
		
	}
}