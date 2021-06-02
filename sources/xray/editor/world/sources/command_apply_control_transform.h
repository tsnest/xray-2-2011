////////////////////////////////////////////////////////////////////////////
//	Created		: 02.04.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_APPLY_CONTROL_TRANSFORM_H_INCLUDED
#define COMMAND_APPLY_CONTROL_TRANSFORM_H_INCLUDED

#include "project_defines.h"


namespace xray{
namespace editor{

ref class level_editor;

public ref class command_apply_object_transform_base abstract: public editor_base::command
{
public:
						command_apply_object_transform_base	( editor_base::transform_control_base^ control, object_base_list^ selection );
	virtual				~command_apply_object_transform_base( );
	virtual void 		rollback		( ) override	{ m_first_run=false; commit(); }
	virtual bool 		end_preview		( ) override	{ m_first_run=false; return true; }

protected:
	editor_base::transform_control_base^	m_transform_control;
	id_matrices_list						m_id_matrices;
	bool									m_first_run;
}; // command_apply_object_transform_base

public ref class command_apply_object_transform : public command_apply_object_transform_base
{
	typedef command_apply_object_transform_base super;

public:
						command_apply_object_transform( editor_base::transform_control_base^ c, object_base_list^ selection ):super(c, selection){};
	virtual bool 		commit			( ) override;
}; // command_apply_object_transform

public ref class command_apply_object_pivot : public command_apply_object_transform_base
{
	typedef command_apply_object_transform_base super;
public:
						command_apply_object_pivot	( editor_base::transform_control_base^ control, object_base_list^ selection );
	virtual bool 		commit						( ) override;
}; // command_apply_object_pivot

public ref class command_apply_folder_transform : public command_apply_object_transform_base
{
	typedef command_apply_object_transform_base super;

public:
						command_apply_folder_transform	( editor_base::transform_control_base^ c, object_base_list^ selection, project_item_folder^ f );
	virtual				~command_apply_folder_transform	( );
	virtual bool 		commit							( ) override;
private:
	id_matrices_list	m_id_matrices_ancors;
	u32					m_folder_id;
	float3*				m_folder_pivot;
}; // command_apply_folder_transform

public ref class command_apply_folder_pivot : public editor_base::command
{
	typedef editor_base::command super;
public:
						command_apply_folder_pivot	( editor_base::transform_control_base^ control, project_item_folder^ f );
	virtual				~command_apply_folder_pivot	( );

	virtual bool 		commit						( ) override;

	virtual void 		rollback		( ) override	{ m_first_run=false; commit(); }
	virtual bool 		end_preview		( ) override	{ m_first_run=false; return true; }

protected:
	editor_base::transform_control_base^	m_transform_control;
	float3*									m_folder_pivot;
	u32										m_id;
	bool									m_first_run;
}; // command_apply_folder_pivot


} // namespace editor
} // namespace xray

#endif // #ifndef COMMAND_APPLY_CONTROL_TRANSFORM_H_INCLUDED