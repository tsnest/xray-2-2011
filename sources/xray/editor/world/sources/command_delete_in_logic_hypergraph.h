////////////////////////////////////////////////////////////////////////////
//	Created		: 30.05.2011
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_DELETE_IN_LOGIC_HYPERGRAPH_H_INCLUDED
#define COMMAND_DELETE_IN_LOGIC_HYPERGRAPH_H_INCLUDED

#include "project_defines.h"
#include "terrain_quad.h"

namespace xray {
	namespace editor {

ref class object_job;

public ref class command_delete_in_logic_hypergraph :	public editor_base::command
{
public:
	command_delete_in_logic_hypergraph		( object_job^ job );

	virtual bool			commit							( ) override;
	virtual void			rollback						( ) override;
private:
	object_job^ m_parent_job;
}; // class terrain_modifier_command

} // namespace editor
} // namespace xray

#endif // #ifndef COMMAND_DELETE_IN_LOGIC_HYPERGRAPH_H_INCLUDED