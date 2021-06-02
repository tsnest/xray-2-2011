////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCES_PANEL_H_INCLUDED
#define PARTICLE_DATA_SOURCES_PANEL_H_INCLUDED

#include "property_holder.h"

using namespace WeifenLuo::WinFormsUI::Docking;
using namespace System::Windows::Forms;

namespace xray {
namespace editor {
	ref class particle_editor;
	ref class particle_graph_node;
public ref class particle_data_sources_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
{
public:
	particle_data_sources_panel(particle_editor^ ed):m_editor(ed)
	{
		in_constructor();
	}

protected: 
	particle_editor^	m_editor;
	WeifenLuo::WinFormsUI::Docking::DockPanel^			m_dock_panel;

public:
	DockContent^			fc_page;
	DockContent^			cc_page;

#pragma region | InnerTypes |

#pragma endregion

#pragma region | Initialize |

#pragma endregion

#pragma region | Fields |

#pragma endregion

#pragma region | Methods |
	void			in_constructor();

public:
	void				add_new_control(Control^ c, DockContent^ to_page);
	
	System::String^		get_float_curves(s32 idx);
	s32					get_float_curves_count();

	System::String^		get_color_curves(s32 idx);
	s32					get_color_curves_count();

	void				save(configs::lua_config_value config);
	void				load(configs::lua_config_value config);
#pragma endregion

#pragma region | Properties |

#pragma endregion

#pragma region | Events |

#pragma endregion

}; // class particle_data_sources_panel

} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCES_PANEL_H_INCLUDED