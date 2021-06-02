////////////////////////////////////////////////////////////////////////////
//	Created		: 11.03.2011
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATION_EDITOR_ACTIONS_H_INCLUDED
#define ANIMATION_EDITOR_ACTIONS_H_INCLUDED

namespace xray {
namespace animation_editor {

	ref class animation_editor;

	public ref class action_select_active_control : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
		action_select_active_control(System::String^ name, animation_editor^ ae, editor_base::editor_control_base^ c);
		virtual bool			do_it								( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		animation_editor^					m_animation_editor;
		editor_base::editor_control_base^	m_control;
	}; // class action_select_active_control

	public ref class mouse_action_editor_control : public editor_base::action_continuous
	{
		typedef editor_base::action_continuous	super;
	public:
		mouse_action_editor_control			(System::String^ name, animation_editor^ ae, int button_id, bool plane_mode);

		virtual bool		capture			() override;
		virtual bool		execute			() override;
		virtual void		release			() override;

	private:
		animation_editor^					m_animation_editor;
		editor_base::editor_control_base^	m_active_handler;
		bool								m_plane_mode;
		int									m_button_id;
	}; // class mouse_action_editor_control
	
	public ref class action_navigation_mode : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
		action_navigation_mode(System::String^ name, animation_editor^ ae);
		virtual bool			do_it								( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		animation_editor^					m_animation_editor;
	}; // class action_select_active_control} // namespace animation_editor

	public ref class action_navigation_pause : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
		action_navigation_pause(System::String^ name, animation_editor^ ae);
		virtual bool			do_it								( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		animation_editor^					m_animation_editor;
	}; // class action_select_active_control

	public ref class action_switch_saving_navigation_scene : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
		action_switch_saving_navigation_scene						(System::String^ name, animation_editor^ ae);
		virtual bool								do_it			( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		animation_editor^					m_animation_editor;
	}; // class action_select_active_control

	public ref class action_change_navigation_vector : public editor_base::action_continuous
	{
		typedef editor_base::action_continuous	super;
	public:
		action_change_navigation_vector								(System::String^ name, animation_editor^ ae, float delta_x, float delta_y, bool dir);

		virtual bool								enabled			() override;
		virtual bool								capture			() override;
		virtual bool								execute			() override;
		virtual void								release			() override;

	private:
		animation_editor^					m_animation_editor;
		float								m_delta_x;
		float								m_delta_y;
		bool								m_change_direction;
	}; // class action_select_active_control

	public ref class action_navigation_jump : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
		action_navigation_jump										(System::String^ name, animation_editor^ ae);
		virtual bool								do_it			( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		animation_editor^					m_animation_editor;
	}; // class action_select_active_control

	public ref class action_render_debug_info : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
		action_render_debug_info									(System::String^ name, animation_editor^ ae);
		virtual bool								do_it			( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		animation_editor^					m_animation_editor;
	}; // class action_select_active_control
} // namespace animation_editor
} // namespace xray
#endif // #ifndef ANIMATION_EDITOR_ACTIONS_H_INCLUDED