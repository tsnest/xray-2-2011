////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2010
//	Author		: 
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARTICLE_SOURCES_PANEL_H_INCLUDED
#define PARTICLE_SOURCES_PANEL_H_INCLUDED

#include "particle_source_nodes_container.h"

namespace xray {
namespace editor_base {enum images92x25;}
namespace editor {
	public ref class particle_sources_panel	:	public System::Windows::Forms::Panel {
		typedef System::Collections::Generic::List<particle_source_nodes_container^> particle_nodes_containers;
		
public:
		particle_sources_panel(particle_editor^ editor){
			in_constructor(editor);
		};

protected:
		~particle_sources_panel()
		{
			DELETE(m_particle_nodes_config);
			for each(Control^ c in this->Controls){
				delete(c);
			}

			if (components)
				delete components;
		}

		System::ComponentModel::Container^			components;

private:
		void			in_constructor(particle_editor^ editor);
	
		System::Void	on_container_click(System::Object^  sender, System::Windows::Forms::MouseEventArgs^ );

public:
		particle_source_nodes_container^		add_container			(String^ container_name);
		particle_graph_node^					add_node_to_container	(particle_source_nodes_container^ container,String^ node_name, String^ node_type, xray::editor_base::images92x25 image_type_id);
		void									load_tools_panel();
		void									on_toolbar_panel_loaded	(xray::resources::queries_result& data);


		
		


private:
		configs::lua_config_value*					m_particle_nodes_config;
		particle_nodes_containers^					m_particle_nodes_containers;
		particle_editor^							m_editor;

public:
	property configs::lua_config_value*				particle_nodes_config{
		configs::lua_config_value*					get(){return m_particle_nodes_config;}
	}

}; // class particle_sources_panel

} // namespace editor
} // namespace xray

#endif // #ifndef PARTICLE_SOURCES_PANEL_H_INCLUDED