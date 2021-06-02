////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.color_range.shaders
{
	internal static class utils
	{
		private static String _assembly_short_name;

		public static Uri make_pack_uri( String relative_file ) {
            var uri_string = "pack://application:,,,/" + assembly_short_name + ";component/" + relative_file;
            return new Uri( uri_string );
        }

        private static string assembly_short_name
        {
            get
            {
                if ( _assembly_short_name == null )
                {
                    var a = typeof(utils).Assembly;

                    // Pull out the short name.
                    _assembly_short_name = a.ToString().Split(',')[0];
                }

                return _assembly_short_name;
            }
        }

        
	}
}
