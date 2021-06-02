////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls
{
	public static class math
	{
		public static			Single		epsilon_3 = 0.0001f;
		public static			Single		epsilon_4 = 0.00001f;
		public static			Single		epsilon_5 = 0.000001f;
		public static			Single		epsilon_6 = 0.0000001f;
		public static			Single		epsilon_7 = 0.00000001f;

		public static			Double		GetNormal		( Random rnd )
		{
	        // Use Box-Muller algorithm
            var u1		= rnd.NextDouble( );
			var u2		= rnd.NextDouble( );
		    var r		= Math.Sqrt( -2.0 * Math.Log( u1 ) );
	        var theta	= 2.0 * Math.PI * u2;
            return r * Math.Sin( theta );
		}
          
	    // Get normal (Gaussian) random sample with specified mean and standard deviation
		public static			Double		GetNormal		( Double mean, Double standardDeviation, Random rnd )
		{
			return mean + standardDeviation * GetNormal( rnd );
		}
	}
}
