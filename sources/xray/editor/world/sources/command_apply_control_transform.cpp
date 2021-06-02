////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "command_apply_control_transform.h"
#include "object_base.h"
#include "level_editor.h"
#include "project.h"

namespace xray {
namespace editor {

object_base^ object_by_id( u32 id)
{
	project_item_base^ item	= project_item_base::object_by_id( id );
	R_ASSERT				(item->get_object());
	return item->get_object ();
}

command_apply_object_transform_base::command_apply_object_transform_base( xray::editor_base::transform_control_base^ control, object_base_list^ selection )
:m_transform_control	(control),
m_first_run				(true)
{
	ASSERT( control != nullptr );

	for each(object_base^ o in selection)
	{
		id_matrix			itm;
		itm.id				= o->id();
		itm.matrix			= NEW (float4x4)(o->get_transform());
		itm.pivot			= NEW (float3)(o->get_pivot());
		m_id_matrices.Add	( itm );
	}
}

command_apply_object_transform_base::~command_apply_object_transform_base( )
{
	for each(id_matrix^ idm in m_id_matrices)
	{
		DELETE( idm->matrix );
		DELETE( idm->pivot );
	}
}

bool command_apply_object_transform::commit( )
{
	for each(id_matrix^ idm in m_id_matrices)
	{
		object_base^ object			= object_by_id(idm->id);

		if( m_first_run )
		{
			float4x4 const XF		= *idm->matrix;

			float4x4 const P		= create_translation(*idm->pivot) * XF;
			float4x4 P_i			= math::invert4x3( P );
			
			math::float4x4 const m	= m_transform_control->calculate( P );
			
			float4x4 m_i			= P_i * m;
			float4x4 mr				= XF * m_i;
	

			object->set_transform	( mr );
		}else
		{
			float4x4 bk				= object->get_transform();
			object->set_transform	(*idm->matrix);
			*idm->matrix			= bk;
		}
	}

	if(m_id_matrices.Count)
	{
		id_matrix^ idm		= m_id_matrices[0];
		object_base^ o		= object_by_id(idm->id);
		o->get_project()->get_level_editor()->update_object_inspector();
	}

	return			true;
}

command_apply_object_pivot::command_apply_object_pivot( xray::editor_base::transform_control_base^ control, object_base_list^ selection )
:super( control, selection )
{}

bool command_apply_object_pivot::commit( )
{
	for each(id_matrix^ idm in m_id_matrices)
	{
		object_base^ object				= object_by_id(idm->id);

		if( m_first_run )
		{
			float3 const current_pivot	= *idm->pivot;
			float4x4 const& XF			= object->get_transform();
			float4x4 XF_i				= math::invert4x3( XF );

			float4x4 const P			= create_translation(current_pivot) * XF;

			float4x4 const m			= m_transform_control->calculate( P );
			
			float4x4 const mi					= m * XF_i;
			object->set_pivot					( mi.c.xyz() );
		}else
		{
			float3 bk							= object->get_pivot();
			object->set_pivot					( *idm->pivot );
			*idm->pivot							= bk;
		}
	}
	return			true;
}

command_apply_folder_transform::command_apply_folder_transform( editor_base::transform_control_base^ c, 
																object_base_list^ selection,
																project_item_folder^ f )
:super(c, selection),
m_folder_id(f->id()),
m_folder_pivot( NEW(float3)(*f->m_pivot) )
{
	float4x4 const a = create_translation(*f->m_pivot);

	for each( id_matrix^ itm in m_id_matrices )
	{// for each object calc pivot point
		id_matrix copy				= *itm;
		
		float4x4 XF_i				= math::invert4x3( *itm->matrix );

		copy.matrix					= NEW(float4x4)( a * XF_i );
		copy.pivot					= NULL;
		m_id_matrices_ancors.Add	( copy );
	}
}

command_apply_folder_transform::~command_apply_folder_transform( )
{
	for each(id_matrix^ idm in m_id_matrices_ancors)
		DELETE( idm->matrix );

	DELETE( m_folder_pivot );
}

bool command_apply_folder_transform::commit( )
{
	project_item_folder^ folder = safe_cast<project_item_folder^>(project_item_base::object_by_id(m_folder_id));

	u32 count = m_id_matrices.Count;
	for(u32 i = 0; i<count; ++i)
	{
		id_matrix^ idm_orig		= m_id_matrices[i];
		id_matrix^ idm_ancor	= m_id_matrices_ancors[i];

		object_base^ object		= object_by_id(idm_orig->id);

		if( m_first_run )
		{
			float4x4 const XF		= *idm_orig->matrix;

			float4x4 const P		= *idm_ancor->matrix * XF;
			float4x4 P_i			= math::invert4x3( P );
			
			float4x4 const m		= m_transform_control->calculate( P );
			
			float4x4 m_i			= P_i * m;
			float4x4 mr				= XF * m_i;

			object->set_transform	( mr );

			// folder position
			float4x4 pivot			= create_translation(*m_folder_pivot);
			float4x4 const mp		= m_transform_control->calculate( pivot );
			*folder->m_pivot		= mp.c.xyz();
		}else
		{
			// objects
			float4x4 bk				= object->get_transform();
			object->set_transform	(*idm_orig->matrix);
			*idm_orig->matrix		= bk;

			// folder position
			float3 bk3					= *folder->m_pivot;
			*folder->m_pivot			= *m_folder_pivot;
			*m_folder_pivot				= bk3;
		}
	}

	if(count)
	{
		id_matrix^ idm		= m_id_matrices[0];
		object_base^ o = object_by_id(idm->id);
		o->get_project()->get_level_editor()->update_object_inspector();
	}

	return			true;
}




command_apply_folder_pivot::command_apply_folder_pivot( editor_base::transform_control_base^ control, project_item_folder^ f )
:m_transform_control	( control ),
m_id					( f->id() ),
m_first_run				( true ),
m_folder_pivot			( NEW(float3)() )
{
	*m_folder_pivot		= *f->m_pivot;
}

command_apply_folder_pivot::~command_apply_folder_pivot()
{
	DELETE(m_folder_pivot);
}

bool command_apply_folder_pivot::commit( )
{
	project_item_folder^ folder = safe_cast<project_item_folder^>(project_item_base::object_by_id(m_id));

	if( m_first_run )
	{
		float4x4 const P			= create_translation(*m_folder_pivot);
		float4x4 const m			= m_transform_control->calculate( P );
		*folder->m_pivot			= m.c.xyz() ;
	}else
	{
		float3 bk					= *folder->m_pivot;
		*folder->m_pivot			= *m_folder_pivot;
		*m_folder_pivot				= bk;
	}
	return			true;
}

} // namespace editor
} // namespace xray
