////////////////////////////////////////////////////////////////////////////
//	Created		: 30.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_delete_in_logic_hypergraph.h"
#include "object_job.h"
namespace xray {
namespace editor {

command_delete_in_logic_hypergraph::command_delete_in_logic_hypergraph( object_job^  job )
:m_parent_job( job )
{
	
}

bool command_delete_in_logic_hypergraph::commit							( )
{
	System::Collections::Generic::List< wpf_controls::hypergraph::node^ > selected_nodes( m_parent_job->logic_view->logic_hypergraph->selected_nodes );

	for each( wpf_controls::hypergraph::node^ node in selected_nodes )
	{
		m_parent_job->remove_behaviour( node->id );
	}
 	for each( wpf_controls::hypergraph::link^ link in m_parent_job->logic_view->logic_hypergraph->selected_links )
 	{
		if (m_parent_job->logic_view->logic_hypergraph->links->ContainsValue(link))
 			m_parent_job->remove_link(link);
 	}
	return true;
}
void command_delete_in_logic_hypergraph::rollback						( )
{

}

} // namespace editor
} // namespace xray
