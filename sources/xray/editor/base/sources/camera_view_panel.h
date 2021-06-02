////////////////////////////////////////////////////////////////////////////
//	Created 	: 12.04.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CAMERA_VIEW_PANEL_H_INCLUDED
#define CAMERA_VIEW_PANEL_H_INCLUDED

#include "scene_view_panel.h"

namespace xray
{
	namespace editor_base
	{
		public ref class  camera_view_panel: scene_view_panel
		{
		public:
			camera_view_panel( )
			{
				Text = "Camera View";
			}

		};//class  camera_view_panel
	}
}

#endif //CAMERA_VIEW_PANEL_H_INCLUDED