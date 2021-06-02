////////////////////////////////////////////////////////////////////////////
//	Created		: 10.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_ACTIONS_H_INCLUDED
#define INPUT_ACTIONS_H_INCLUDED

namespace xray {
namespace particle_editor {

ref class particle_editor;

public ref class mouse_action_editor_control : public editor_base::action_continuous
{
public:

	mouse_action_editor_control			( System::String^ name, particle_editor^ le, int button_id, bool plane_mode );

	virtual bool		capture			( ) override;
	virtual bool		execute			( ) override;
	virtual void		release			( ) override;

private:

	particle_editor^			m_particle_editor;
	editor_base::editor_control_base^	m_active_handler;
	bool					m_plane_mode;
	int						m_button_id;
}; // class mouse_action_editor_control

public ref class action_select_active_control : public editor_base::action_single
{
	typedef editor_base::action_single	super;
public:
	action_select_active_control ( System::String^ name, particle_editor^ le, editor_base::editor_control_base^ c );

	virtual bool			do_it								( ) override;
	virtual xray::editor_base::checked_state	checked_state	( ) override;

private:
	particle_editor^					m_particle_editor;
	editor_base::editor_control_base^	m_control;

}; // class action_select_active_control


public ref class action_focus : public editor_base::action_single
{
	typedef editor_base::action_single super;
public:
	action_focus( System::String^ name, particle_editor^ pe);

	virtual bool	do_it					( ) override;
	virtual bool	enabled					( ) override;

	particle_editor^	m_particle_editor;	
}; // class action_focus


} // namespace particle_editor
} // namespace xray

#endif // #ifndef INPUT_ACTIONS_H_INCLUDED