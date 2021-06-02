////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_set_object_transform.h"
#include "object_base.h"
#include "level_editor.h"

namespace xray {
namespace editor {

command_set_object_transform::command_set_object_transform(level_editor^ le, u32 id, math::float4x4 transform)
:m_level_editor	(le)
{
	id_matrix	idm;
	idm.id		= id;
	idm.matrix	= NEW(float4x4)(transform);
	m_id_matrices.Add(idm);
}

command_set_object_transform::~command_set_object_transform( )
{
	for each(id_matrix^ itm in m_id_matrices)
		DELETE ( itm->matrix	);

	m_id_matrices.Clear( );
}

bool command_set_object_transform::commit	()
{
	math::float4x4 tmp_mat;

	for each (id_matrix^ idm in m_id_matrices)
	{
		object_base^ curr_obj	= m_level_editor->object_by_id(idm->id);
		tmp_mat					= curr_obj->get_transform();
		curr_obj->set_transform( *idm->matrix );
		*idm->matrix			= tmp_mat;
	}

	return true;
}

void command_set_object_transform::rollback	()
{
	commit();
}

command_set_object_name::command_set_object_name( level_editor^ le, u32 id, System::String^ name ):
m_level_editor	(le),
m_object_id		(id)
{
	m_name		= name;
}

bool command_set_object_name::commit	()
{
	object_base^ curr_obj	= m_level_editor->object_by_id(m_object_id);
	R_ASSERT				(curr_obj, "object %d not found", m_object_id);
	m_rollback_name			= curr_obj->get_name();
	curr_obj->set_name		(m_name, true);
	return					true;
}

void command_set_object_name::rollback	()
{
	object_base^ curr_obj	= m_level_editor->object_by_id(m_object_id);
	curr_obj->set_name		(m_rollback_name, true);
}

} // namespace editor
} // namespace xray
