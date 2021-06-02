////////////////////////////////////////////////////////////////////////////
//	Created		: 30.03.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_CREATE_LINK_H_INCLUDED
#define COMMAND_CREATE_LINK_H_INCLUDED

#include "project_defines.h"



namespace xray {
namespace editor {

ref class link_storage;

class editor_world;

enum class enum_action{ enum_link_action_create, enum_link_action_remove };

public ref class command_create_remove_link : public xray::editor_base::command
{
public:

	command_create_remove_link	( enum_action action, link_storage^ link_storage, object_base^ src_object, System::String^ src_name, object_base^ dst_object, System::String^ dst_name );

	virtual bool			commit		( ) override;
	virtual void			rollback	( ) override;

private:
	enum_action				m_action;
	link_storage^			m_link_storage;
	u32						m_src_id;
	u32						m_dst_id;
	System::String^			m_src_name;
	System::String^			m_dst_name;

}; // class command_create_remove_link

} // namespace editor
} // namespace xray


#endif // #ifndef COMMAND_CREATE_LINK_H_INCLUDED