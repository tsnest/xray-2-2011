////////////////////////////////////////////////////////////////////////////
//	Created		: 26.01.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.color_picker
{
	internal abstract class color_part_worker_base
	{
		public abstract		Double		color_to_part	( color_hsv color );
		public abstract		color_hsv	part_to_color	( Double part );
	}
}
