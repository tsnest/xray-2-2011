////////////////////////////////////////////////////////////////////////////
//	Created		: 30.06.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_SCENE_COMMANDS_H_INCLUDED
#define SOUND_SCENE_COMMANDS_H_INCLUDED

#include "sound_editor.h"

using namespace System;
using namespace System::Collections::Generic;

namespace xray {
namespace sound_editor {

	ref class sound_object_instance;
	ref class sound_scene_document;

	public ref class sound_scene_add_objects_command: public xray::editor_base::command
	{
		typedef List<Guid> ids_list;
		typedef List<String^> instances_list;
		typedef List<Float3> positions_list;
	public:
		sound_scene_add_objects_command		(sound_scene_document^ d, sound_object_type t, float3 position, List<String^>^ obj_list);
		~sound_scene_add_objects_command	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		sound_scene_document^	m_document;
		sound_object_type		m_objects_type;
		ids_list^				m_ids_list;
		instances_list^			m_instances;
		positions_list^			m_positions;
	}; // class animation_groups_add_group_command

	public ref class sound_scene_remove_objects_command: public xray::editor_base::command
	{
		typedef List<Guid> ids_list;
		typedef List<KeyValuePair<String^, sound_object_type>> instances_list;
		typedef List<Float3> positions_list;
	public:
		sound_scene_remove_objects_command	(sound_scene_document^ d, List<sound_object_instance^>^ obj_list);
		~sound_scene_remove_objects_command	();

		virtual bool	commit				() override;
		virtual void	rollback			() override;

	private:
		sound_scene_document^	m_document;
		ids_list^				m_ids_list;
		instances_list^			m_instances;
		positions_list^			m_positions;
		positions_list^			m_directions;
	}; // class sound_scene_remove_objects_command

	public ref class sound_scene_change_object_transform_command: public xray::editor_base::command
	{
	public:
		sound_scene_change_object_transform_command		(sound_scene_document^ d, sound_object_instance^ inst, float4x4 old_val, float4x4 new_val);
		~sound_scene_change_object_transform_command	();

		virtual bool	commit							() override;
		virtual void	rollback						() override;

	private:
		sound_scene_document^	m_document;
		Guid					m_instance_id;
		float4x4*				m_old_val;
		float4x4*				m_new_val;
	}; // class sound_scene_change_object_transform_command

	public ref class sound_scene_change_objects_property_command: public xray::editor_base::command
	{
		typedef List<Guid> ids_list;
	public:
		sound_scene_change_objects_property_command	(sound_scene_document^ d, List<sound_object_instance^>^ obj_list, String^ prop_name, Object^ old_val, Object^ new_val);
		~sound_scene_change_objects_property_command();

		virtual bool	commit						() override;
		virtual void	rollback					() override;

	private:
		sound_scene_document^	m_document;
		ids_list^				m_instance_ids;
		String^					m_property_name;
		Object^					m_old_val;
		Object^					m_new_val;
	}; // class sound_scene_change_objects_property_command
} // namespace sound_editor
} // namespace xray
#endif // #ifndef SOUND_SCENE_COMMANDS_H_INCLUDED