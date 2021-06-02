////////////////////////////////////////////////////////////////////////////
//	Created		: 23.06.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls
{
	public class float_curve_noise_effect: float_curve_effect
	{
		public float_curve_noise_effect( )
		{
			m_seed		= 32;
			m_frequency	= 0.1f;
			m_strength	= 5;
		}

		public			Int32			m_seed;
		public			Single			m_strength;
		public			Single			m_frequency;
		public			Single			m_fade_in;
		public			Single			m_fade_out;
	}
}
