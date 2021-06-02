////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_ADD_LIBRARY_OBJECT_H_INCLUDED
#define COMMAND_ADD_LIBRARY_OBJECT_H_INCLUDED

namespace xray {
namespace editor {
ref class tool_base;

ref class level_editor;

ref class project_item_object;

public delegate void object_added_callback( project_item_object^ obj );

public ref class command_add_library_object: public editor_base::command
{
public:
	command_add_library_object	(	level_editor^ le, 
									tool_base^ tool, 
									System::String^ library_name, 
									math::float4x4 transform,
									object_added_callback^ callback,
									bool select_after_adding);

	virtual ~command_add_library_object			();

	virtual bool commit		() override;
	virtual void rollback	() override;

private:	// Data for execute
	level_editor^				m_level_editor;
	tool_base^					m_tool;
	System::String^				m_library_name;
	math::float4x4*				m_transform;
	System::String^				m_parent_folder_name;
	bool						m_select_after_adding;

	object_added_callback^		m_object_added_callback;
	u32							m_object_id;
}; // class command_add_library_object

public ref class command_add_project_folder: public editor_base::command
{
public:
					command_add_project_folder	( level_editor^ le, System::String^ name );

	virtual bool	commit		( ) override;
	virtual void	rollback	( ) override;

	u32							m_added_folder_id;
private:
	level_editor^				m_level_editor;
	System::String^				m_name;
	u32							m_parent_folder_id;
};

}// namespace editor
}// namespace xray

#endif // #ifndef COMMAND_ADD_LIBRARY_OBJECT_H_INCLUDED