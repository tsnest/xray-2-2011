////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2009
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DOCK_PANEL_SERIALIZER_H_INCLUDED
#define DOCK_PANEL_SERIALIZER_H_INCLUDED

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;
using namespace WeifenLuo::WinFormsUI::Docking;

namespace xray {
namespace editor {
namespace controls{

	public ref class serializer
	{
	public:
		static void serialize_dock_panel_root(Form^ root, DockPanel^ panel, String^ window_name);
		static bool deserialize_dock_panel_root(Form^ root, DockPanel^ panel, String^ window_name, DeserializeDockContent^ get_panel_for_setting_callback, bool reload);
	};

}//namespace controls
}//namespace editor
}//namespace xray

#endif // #ifndef DOCK_PANEL_SERIALIZER_H_INCLUDED