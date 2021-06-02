////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_ACTIONS_H_INCLUDED
#define INPUT_ACTIONS_H_INCLUDED

#include "project_defines.h"
#include <xray/editor/base/transform_control_defines.h>

namespace xray {

namespace collision { struct result; }

namespace editor {

ref class window_ide;
ref class level_editor;

class editor_world;
class collision_object_dynamic;


public ref class mouse_action_editor_control : public editor_base::action_continuous
{
	typedef editor_base::action_continuous	super;

public:
	mouse_action_editor_control			( System::String^ name, level_editor^ le, int button_id, bool plane_mode );

	virtual bool		capture			( ) override;
	virtual bool		execute			( ) override;
	virtual void		release			( ) override;

private:

	level_editor^			m_level_editor;
	editor_base::editor_control_base^	m_active_handler;
	bool					m_plane_mode;
	int						m_button_id;

}; // class mouse_action_editor_control

public ref class mouse_action_select_object : public editor_base::action_continuous
{
	typedef editor_base::action_continuous	super;
	static mouse_action_select_object	( );

public:
	mouse_action_select_object		( System::String^ name, level_editor^ le, enum_selection_method method );

	virtual bool		capture		( ) override;
	virtual bool		execute		( ) override;
	virtual void		release		( ) override;
	virtual bool		enabled		( ) override;

public:
	static bool						s_is_enabled;

private:
	level_editor^					m_level_editor;
	object_base^					m_selected;
	System::Drawing::Point			m_start_xy;
	enum_selection_method			m_select_method;
}; // class mouse_action_select_object


public ref class input_action_add_library_object : public editor_base::action_continuous
{
	typedef editor_base::action_continuous	super;

public:
	input_action_add_library_object		( System::String^ name, level_editor^ le);

	virtual bool		enabled		( ) override;
	virtual bool		capture		( ) override;
	virtual bool		execute		( ) override;
	virtual void		release		( ) override {};

private:
	level_editor^			m_level_editor;
}; // class input_action_add_library_object

public ref class mouse_action_control_properties : public editor_base::action_continuous
{
	typedef editor_base::action_continuous super;

public:
					mouse_action_control_properties	( System::String^ action_name, level_editor^ le, System::String^ prop_name, float scale );
public:
	virtual bool 	enabled					( ) override;
	virtual bool 	capture					( ) override;
	virtual void	release					( ) override;
	virtual bool	execute					( ) override;

protected:
	System::String^			m_prop_name;
	float					m_scale;
	level_editor^			m_level_editor;
	System::Drawing::Point	m_mouse_capture_pos;
};//mouse_action_control_properties

public ref class action_control_properties : public editor_base::action_single
{
	typedef editor_base::action_single	super;
public:

	action_control_properties		( System::String^ action_name, level_editor^ le, System::String^ prop_name );

	virtual bool		enabled		( )	override;
	virtual bool		do_it		( ) override;

private:
	System::String^			m_prop_name;
	level_editor^			m_level_editor;
}; // class action_control_properties


public ref class action_control_step_modify : public editor_base::action_single
{
	typedef editor_base::action_single	super;
public:

	action_control_step_modify		( System::String^ action_name, level_editor^ le, xray::editor_base::enum_transform_axis, float value );

	virtual bool		enabled		( )	override;
	virtual bool		do_it		( ) override;

private:
	xray::editor_base::enum_transform_axis		m_axis;
	float										m_value;
	level_editor^								m_level_editor;
}; // class action_control_properties


} // namespace editor
} // namespace xray


#endif // #ifndef INPUT_ACTIONS_H_INCLUDED