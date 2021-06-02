////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COLLISION_GEOMETRY_MESH_MOVE_COMMAND_H_INCLUDED
#define OBJECT_COLLISION_GEOMETRY_MESH_MOVE_COMMAND_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		ref class level_editor;

		ref class object_collision_geometry_mesh_move_command: editor_base::command
		{
		public:
			object_collision_geometry_mesh_move_command( object_collision_geometry^ geometry, Int32 mesh_index, Boolean is_anti_mesh, editor_base::transform_control_base^ transform_control );

		public:
			virtual bool	commit			( ) override;
			virtual void	rollback		( ) override;

			virtual void	start_preview	( )	override;
			virtual bool	end_preview		( )	override;

		private:
			Float3							m_start_position;
			Float3							m_start_rotation;
			Float3							m_start_scale;

			Float3							m_end_position;
			Float3							m_end_rotation;
			Float3							m_end_scale;

			editor_base::transform_control_base^ m_transform_control;
			level_editor^					m_level_editor;
			String^							m_geometry_full_name;
			Boolean							m_is_anti_mesh;
			Boolean							m_is_preview;
			Int32							m_mesh_index;
		}; // class object_collision_geometry_mesh_move_command
	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COLLISION_GEOMETRY_MESH_MOVE_COMMAND_H_INCLUDED