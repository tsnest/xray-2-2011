////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COLLISION_GEOMETRY_ADD_REMOVE_MESH_COMMANDS_H_INCLUDED
#define OBJECT_COLLISION_GEOMETRY_ADD_REMOVE_MESH_COMMANDS_H_INCLUDED

using namespace System;

namespace xray
{
	namespace collision
	{
		enum primitive_type;
	}

	namespace editor
	{
		ref class object_collision_geometry;
		ref class project;

		ref class object_collision_geometry_add_mesh_command: public editor_base::command
		{
		public:
			object_collision_geometry_add_mesh_command		( object_collision_geometry^ geometry, collision::primitive_type type, Boolean is_anti_mesh );

			virtual bool			commit							( ) override;
			virtual void			rollback						( ) override;

		private:
					project^					m_project;
					String^						m_geometry_path;
			const	collision::primitive_type	m_type;
			const	Boolean						m_is_anti_mesh;
					Int32						m_added_collision_index;
		}; // class object_collision_geometry_add_mesh_command

		ref class object_collision_geometry_remove_mesh_command: public editor_base::command
		{
		public:
			object_collision_geometry_remove_mesh_command	( object_collision_geometry^ geometry, collision::primitive_type type, Boolean is_anti_mesh, Int32 index );
	
			virtual bool			commit							( ) override;
			virtual void			rollback						( ) override;

		private:
					project^					m_project;
					String^						m_geometry_path;
			const	collision::primitive_type	m_type;
			const	Boolean						m_is_anti_mesh;
					Int32						m_remove_collision_index;
					float4x4*					m_matrix;
					Float3						m_position;
					Float3						m_rotation;
					Float3						m_scale;

		}; // class object_collision_geometry_remove_mesh_command

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COLLISION_GEOMETRY_ADD_REMOVE_MESH_COMMANDS_H_INCLUDED