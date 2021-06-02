////////////////////////////////////////////////////////////////////////////
//	Created		: 04.05.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

using xray.editor.wpf_controls.color_picker;
using xray.editor.wpf_controls.hypergraph;
using xray.editor.wpf_controls.markup_extensions;
using xray.editor.wpf_controls.property_editors;
using xray.editor.wpf_controls.property_grid;

namespace xray.editor.wpf_controls
{
	public static class library_initializer
	{
		public static void initialize( )
		{
			color_palette.initialize				( );
			default_editors_lists.initialize		( );
			link_point.initialize					( );
			thisExtension.initialize				( );
			property_grid_item.initialize			( );
		}
	}
} // xray.editor.wpf_controls