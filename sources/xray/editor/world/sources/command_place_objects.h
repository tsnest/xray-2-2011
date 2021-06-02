////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_PLACE_OBJECTS_H_INCLUDED
#define COMMAND_PLACE_OBJECTS_H_INCLUDED

#include "command_snap_base.h"
#include "project_defines.h"

namespace xray {
namespace editor {

ref class level_editor;

public ref class command_place_objects :	public command_snap_base
{
public:
				command_place_objects	( level_editor^ le );
	virtual		~command_place_objects	( );


	virtual bool commit					( ) override;
	virtual void rollback				( ) override;
	virtual bool end_preview			( ) override;

private:
	level_editor^			m_level_editor;	
	id_matrices_list		m_id_matrices;
	bool					m_preview_mode;
}; // class command_place_objects

} // namespace editor
} // namespace xray

#endif // #ifndef COMMAND_PLACE_OBJECTS_H_INCLUDED