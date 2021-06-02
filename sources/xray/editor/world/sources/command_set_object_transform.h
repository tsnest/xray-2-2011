////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_TRANSLATE_H_INCLUDED
#define COMMAND_TRANSLATE_H_INCLUDED

#include "project_defines.h"


namespace xray {
namespace editor {

ref class level_editor;


public ref class command_set_object_transform : public xray::editor_base::command
{
public:
							command_set_object_transform	( level_editor^ le, u32 object, math::float4x4 matrix );
	virtual					~command_set_object_transform	( );
	virtual bool			commit							( ) override;
	virtual void			rollback						( ) override;

private:

	level_editor^			m_level_editor;
	id_matrices_list		m_id_matrices;

}; // class command_set_object_transform

public ref class command_set_object_name : public xray::editor_base::command
{
public:
	command_set_object_name	( level_editor^ le, u32 object, System::String^ name );

	virtual bool			commit		() override;
	virtual void			rollback	() override;

private:
	level_editor^			m_level_editor;
	u32						m_object_id;
	System::String^			m_name;
	System::String^			m_rollback_name;
}; // class command_set_object_name

} // namespace editor
} // namespace xray


#endif // #ifndef COMMAND_TRANSLATE_H_INCLUDED