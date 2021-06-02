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
namespace particle_editor {
	public ref class particle_sources_panel	:	public System::Windows::Forms::Panel {
		typedef System::Collections::Generic::List<particle_source_nodes_container^>		particle_nodes_containers;

		
public:
		particle_sources_panel(particle_editor^ editor){
			in_constructor(editor);			
		};

protected:
	!particle_sources_panel()
	{
	
	}
		~particle_sources_panel()
		{
			this->!particle_sources_panel();
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
		particle_source_nodes_container^	add_container			( String^ container_name );

		particle_graph_node^				add_node_to_container	(	particle_source_nodes_container^ container,
																		String^ node_name,
																		configs::lua_config_value& node_config );
		
		void								load					( configs::lua_config_value config );


		
		


private:
		particle_nodes_containers^					m_particle_nodes_containers;
		particle_editor^							m_editor;		
		
}; // class particle_sources_panel

} // namespace particle_editor
} // namespace xray

#endif // #ifndef PARTICLE_SOURCES_PANEL_H_INCLUDED