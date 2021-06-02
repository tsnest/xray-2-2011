////////////////////////////////////////////////////////////////////////////
//	Created		: 20.10.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GENERIC_BEHAVIOUR_H_INCLUDED
#define GENERIC_BEHAVIOUR_H_INCLUDED

using namespace xray::editor::wpf_controls::hypergraph;

namespace xray {
namespace editor {
	
	
ref class generic_behaviour {



public:
	generic_behaviour( System::String^ type );
	
	wpf_controls::hypergraph::hypergraph_host^ get_behaviour_edit_view ( );

private:
	void					on_link_creating					( System::Object^, link_event_args^ /*e*/ ){};
	void					on_hypergraph_selecting				( System::Object^, selection_event_args^ /*e*/ ){};
	void					on_hypergraph_context_menu_opening	( ){};
	void					on_hypergraph_menu_item_click		( System::Object^, menu_event_args^ /*args*/ ){};
		
private:
	System::String^ m_type;

	System::Windows::Controls::MenuItem^	m_delete_selected_item;	
	
}; // class generic_behaviour

} // namespace editor
} // namespace xray

#endif // #ifndef GENERIC_BEHAVIOUR_H_INCLUDED