////////////////////////////////////////////////////////////////////////////
//	Created		: 16.06.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_ACTIONS_H_INCLUDED
#define TRANSFORM_CONTROL_ACTIONS_H_INCLUDED

#include "action_continuous.h"
#include "transform_control_base.h"

namespace xray {
namespace editor_base {

ref class transform_control_translation;
ref class scene_view_panel;
public ref class mouse_action_editor_control_snap : public action_continuous
{
	typedef action_continuous	super;

public:
	mouse_action_editor_control_snap	(	System::String^ name, 
											active_control_getter^ getter, 
											editor_base::snap_mode snap_mode, 
											scene_view_panel^ p );

	virtual bool		capture			( ) override;
	virtual bool		execute			( ) override;
	virtual void		release			( ) override;

private:
	scene_view_panel^						m_scene_view_panel;
	active_control_getter^					m_active_control_getter;
	transform_control_translation^			m_active_handler;
	snap_mode								m_snap_mode;
}; // class mouse_action_editor_control_snap


} // namespace editor_base
} // namespace xray

#endif // #ifndef TRANSFORM_CONTROL_ACTIONS_H_INCLUDED