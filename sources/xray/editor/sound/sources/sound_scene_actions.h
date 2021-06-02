////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_SCENE_ACTIONS_H_INCLUDED
#define SOUND_SCENE_ACTIONS_H_INCLUDED

#pragma managed(push, off)
#include <xray/collision/space_partitioning_tree.h>
#pragma managed(pop)

using namespace System;

namespace xray {
namespace sound_editor {

	ref class sound_scene_document;
	ref class sound_object_instance;
	enum class enum_selection_method
	{ 
		enum_selection_method_set,
		enum_selection_method_add, 
		enum_selection_method_invert, 
		enum_selection_method_subtract, 
		enum_selection_method_NONE 
	};
	struct check_object_predicate
	{
		check_object_predicate():m_result(NULL, 0.0f){}
		bool callback(xray::collision::ray_object_result const& r)
		{
			if(r.object==m_result.object)
			{
				m_result = r;
				return true;
			}

			return false;
		}

		xray::collision::ray_object_result m_result;
	}; // struct check_object_predicate
	
	bool check_picked_object(xray::collision::space_partitioning_tree* collision_tree, xray::collision::object const* object, float3 const& cam_origin, float3 const& cam_direction);

	public ref class action_select_active_control : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
		action_select_active_control(String^ name, sound_scene_document^ doc, editor_base::editor_control_base^ c);
		virtual bool			do_it								( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		sound_scene_document^				m_document;
		editor_base::editor_control_base^	m_control;
	}; // class action_select_active_control

	public ref class mouse_action_editor_control : public editor_base::action_continuous
	{
		typedef editor_base::action_continuous	super;
	public:
		mouse_action_editor_control	(String^ name, sound_scene_document^ doc, int button_id, bool plane_mode);

		virtual bool		capture	() override;
		virtual bool		execute	() override;
		virtual void		release	() override;

	private:
		sound_scene_document^				m_document;
		editor_base::editor_control_base^	m_active_handler;
		bool								m_plane_mode;
		int									m_button_id;
	}; // class mouse_action_editor_control

	public ref class mouse_action_select_object : public editor_base::action_continuous
	{
		typedef editor_base::action_continuous	super;
	public:

		mouse_action_select_object	(String^ name, sound_scene_document^ doc, enum_selection_method method);

		virtual bool		capture	() override;
		virtual bool		execute	() override;
		virtual void		release	() override;

	private:
		sound_scene_document^	m_document;
		sound_object_instance^	m_selected;
		System::Drawing::Point	m_start_xy;
		enum_selection_method	m_select_method;
	}; // class mouse_action_select_object

	public ref class start_all_sound_action : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
								start_all_sound_action				(String^ name, sound_scene_document^ doc);
		virtual bool			do_it								( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		sound_scene_document^	m_document;
	}; // class start_all_sound_action

	public ref class stop_all_sound_action : public editor_base::action_single
	{
		typedef editor_base::action_single	super;
	public:
								stop_all_sound_action				(String^ name, sound_scene_document^ doc);
		virtual bool			do_it								( ) override;
		virtual xray::editor_base::checked_state	checked_state	( ) override;

	private:
		sound_scene_document^	m_document;
	}; // class stop_all_sound_action
} // namespace sound_editor
} // namespace xray
#endif // #ifndef SOUND_SCENE_ACTIONS_H_INCLUDED