////////////////////////////////////////////////////////////////////////////
//	Created		: 30.05.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef INPUT_ACTIONS_H_INCLUDED
#define INPUT_ACTIONS_H_INCLUDED

namespace xray{
namespace model_editor{

ref class model_editor;

ref class me_transform_control_helper : public editor_base::transform_control_helper
{
	typedef editor_base::transform_control_helper	super;
public:
						me_transform_control_helper( model_editor^ pe );
	virtual float4x4	get_inverted_view_matrix	( ) override;
	virtual float4x4	get_projection_matrix		( ) override;
	virtual void		get_mouse_ray				( float3& origin, float3& direction) override;

private:
	model_editor^ m_model_editor;
};

ref class mouse_action_editor_control : public editor_base::action_continuous
{
public:

	mouse_action_editor_control			( System::String^ name, model_editor^ me, int button_id, bool plane_mode );

	virtual bool		capture			( ) override;
	virtual bool		execute			( ) override;
	virtual void		release			( ) override;

private:

	model_editor^						m_model_editor;
	editor_base::editor_control_base^	m_active_handler;
	bool								m_plane_mode;
	int									m_button_id;
}; // class mouse_action_editor_control

ref class action_select_active_control : public editor_base::action_single
{
	typedef editor_base::action_single	super;
public:
	action_select_active_control ( System::String^ name, model_editor^ me, editor_base::editor_control_base^ c );

	virtual bool			do_it								( ) override;
	virtual xray::editor_base::checked_state	checked_state	( ) override;

private:
	model_editor^						m_model_editor;
	editor_base::editor_control_base^	m_control;

}; // class action_select_active_control


} //namespace model_editor
} //namespace xray

#endif // #ifndef INPUT_ACTIONS_H_INCLUDED