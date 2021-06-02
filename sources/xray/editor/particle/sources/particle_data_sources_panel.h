////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_DATA_SOURCES_PANEL_H_INCLUDED
#define PARTICLE_DATA_SOURCES_PANEL_H_INCLUDED

using namespace WeifenLuo::WinFormsUI;
using namespace System::Windows::Forms;
using namespace System::Collections;
using namespace System;

namespace xray {
namespace particle_editor {
	ref class particle_editor;
	ref class particle_graph_node;
	interface class particle_data_source_base;

public ref class particle_data_sources_panel : public Docking::DockContent
{

typedef System::Collections::Generic::List< particle_data_source_base^ >			data_sources_list;
typedef System::Collections::Generic::Dictionary< String^, data_sources_list^ >		data_sources_lists_by_types_dictionary;

	
public:
	particle_data_sources_panel(particle_editor^ ed):m_editor(ed)
	{
		in_constructor();
	}

protected: 
	particle_editor^										m_editor;	
	Docking::DockPanel^										m_dock_panel;	
	Generic::Dictionary< String^, Docking::DockContent^>^	m_panels_dictionary;

public:
	
#pragma region | InnerTypes |

#pragma endregion

#pragma region | Initialize |

#pragma endregion

#pragma region | Fields |

#pragma endregion

#pragma region | Methods |
	void				in_constructor();

public:
	void				show_data_sources		( data_sources_lists_by_types_dictionary^ data_sources );
	void				initialize_panels		( Generic::Dictionary< String^, String^ >^ data_source_types );
	void				show_panel_sources		( data_sources_list^ data_sources, System::String^ panel_name );
	void				add_data_source_to_panel( particle_data_source_base^ data_source, System::String^ panel_name );

#pragma endregion

#pragma region | Properties |

public:
	property 	Docking::DockPanel^		dock_panel
	{
		Docking::DockPanel^				get(){return m_dock_panel;}	
	}

#pragma endregion

#pragma region | Events |

#pragma endregion

}; // class particle_data_sources_panel

} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_DATA_SOURCES_PANEL_H_INCLUDED